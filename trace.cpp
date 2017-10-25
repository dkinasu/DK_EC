/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "trace.h"


//Parse each traceline and load into &Tline
int TraceLine_Parse(char * buffer, struct traceline *T_line)
{
    //printf("------------------------Parsing One TraceLine----------------------------\n");
	int i = 0;
	char *result = NULL;

	//strsep::  seperate the str with the deliminater ","
	//result is NULL
	//get every single part of the traceline, and put them into replay_node
	while((result = strsep(&buffer, ",")) != NULL) {
		
		switch(i) {
		case 0:
			//get the time_stamp, and put into the &replay_node->time_stamp
			sscanf(result, "%llu", &T_line->time_stamp);
			break;
		case 1:
			T_line->file_path = (char *)malloc(strlen(result) + 1);
			strncpy(T_line->file_path, result, strlen(result));
			T_line->file_path[strlen(result)] = '\0';
			break;
		case 2:
			sscanf(result, "%lu", &T_line->inode_no);
			break;
		case 3:
			sscanf(result, "%lu", &T_line->data_size);
			break;
		case 4:
			sscanf(result, "%ld", &T_line->pos);
			break;
		case 5:
			//this decides read or write
			sscanf(result, "%d", &T_line->rw);
			break;
		case 6:
			strcpy(T_line->pname, result);
			break;
		case 7:
			if ((strlen(result)) != 41) 
			{
				printf("result: %lu\n", strlen(result));
				T_line->databuf = NULL;
				return 0;
			} 
			else 
			{
				
				T_line->databuf = (char *)malloc(sizeof(char) * (SIZE_PER_FPRECORD) + 1);
				
				if (!T_line->databuf) 
				{
					printf("low memory on host!\n");
					exit(-1);
				}

				strncpy(T_line->databuf, result, SIZE_PER_FPRECORD);
				
				if (SIZE_PER_FPRECORD != (strlen(result) - 1)) 
				{
					
					metrics[PAGE_SIZE_WRONG].total ++;
				}
				
				T_line->databuf[SIZE_PER_FPRECORD] = '\0';
			}

			break;
		}

		i++;
	}
	return 1;



    //printf("------------------------Finish Parsing----------------------------------------\n");
}



//Clear the Tline
int Clear_Traceline(struct traceline *T_line)
{
	if (T_line->file_path)
		free(T_line->file_path);

	T_line->file_path = NULL;
	T_line->data_size = 0;
	T_line->pos = 0;
	T_line->inode_no = 0;

	if (T_line->databuf)
		free(T_line->databuf);
	T_line->databuf = NULL;
	
	T_line->rw = -1;
	memset(T_line->pname, '\0', TASK_COMM_LEN);
	T_line->time_stamp = 0;

}

/*print_trace*/
void Print_traceline(struct traceline *T_line)
{
	printf("T_line->file_path: %s\n", T_line->file_path);
	printf("T_line->data_size: %lu\n", T_line->data_size);
	printf("T_line->pos: %ld\n", T_line->pos);
	printf("T_line->inode_no: %lu\n", T_line->inode_no);
	printf("T_line->databuf: %s\n", T_line->databuf);
	printf("T_line->rw: %d\n", T_line->rw);
	printf("T_line->pname: %s\n", T_line->pname);
	printf("T_line->time_stamp: %llu\n", T_line->time_stamp);
}




//Parse the args of input
int Argv_Parse(int argc, char ** argv, char *file_prefix)
{
    
    printf("------------------------Doing Argv_Parsing----------------------------\n");
	
	//check the parameter number.
	if (argc < 8) {
		printf("Do not have enough parameters!\n");
		printf("Usage: ./main [tracefolder_path/] [start_num] [end_num] [result_file] [node_num] [max_blk_num] [maxFp_num] [cache_size]\n");
		return -1;
	}
    
    //printf("Enough args: %d\n", argc);

	//open the dir, checking the tracefoler_path
	//printf("argv[1] =  %s\n", argv[1]);
                   
    DIR *sp;

	if((sp = opendir(argv[1])) == 0) 
    {
        printf("open dir Failed!\n");
		perror("fail to open dir");
		return -1;
	}
    closedir(sp);

	//assign args
	trace_start = atoi(argv[2]);
    trace_end = atoi(argv[3]);
    node_num = atoi(argv[5]);
    max_fp_num = atoi(argv[6]);
    max_blk_num = atoi(argv[7]);
    cache_size = atoi(argv[8]);
    
    // form the legal tracefile address
    files = (char **)malloc((trace_end - trace_start)*sizeof(char *));
    
    for(int i = trace_start; i < trace_end; i++)
    {
    	files[i] = (char *)malloc(500 * sizeof(char));
    	memset(files[i], '\0', 500);
    	sprintf(files[i], "%s%s%d", argv[1], file_prefix, i);
    	//printf("file[%d]: %s\n", i, files[i]);
   	}
    
    printf("------------------------Finish Argv_Parsing----------------------------\n");

    //printf("trace_start: %d trace_end: %d, node_num: %d max_fp_num: %d max_radix_leaf: %lu\n", trace_start, trace_end, node_num, max_fp_num, max_radix_leaf);
};

//write the results to the place of *result
void Output_Result(char *result)
{
    FILE *file_result;
    char result_buf[40960];
    int i;

    //open result
    file_result = fopen(result, "wb+");

    
    //Output the result to result!
    sprintf(result_buf, "********************\n");
    
    result_buf[strlen(result_buf)] = '\0';
    
    fwrite(result_buf, 1, strlen(result_buf), file_result);

    fclose(file_result);

}


void Create_Default_Setting(char *** p)
{
    (*p) = (char **)malloc(9 * sizeof(char *));


    (*p)[0] = (char *)"./main";
    (*p)[1] = (char *)"/home/dkinasu/NetBeansProjects/Diff_EC_Sim//Tracefile/";
    //(*p)[1] = (char *)"/home/dkinasu/NetBeansProjects/Diff_EC_Sim//Tracefile/";
    (*p)[2] = (char *)"0";
    (*p)[3] = (char *)"3";
    (*p)[4] = (char *)"1.txt";
    (*p)[5] = (char *)"1";
    (*p)[6] = (char *)"5000";
    (*p)[7] = (char *)"200000";
    (*p)[8] = (char *)"5";


    /*
    for (int i = 0; i < 8; ++i)
    {
        printf("p[%d]: %s\n", i, p[i]);

    }*/
}
