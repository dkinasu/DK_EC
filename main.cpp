/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: dkinasu
 *
 * Created on October 23, 2017, 4:36 PM
 */

#include <cstdlib>
#include <cstring>
#include "trace.h"
#include "memory.h"
#include "storage.h"
#include "process.h"




using namespace std;


/* argv and tracel*/
char **files;/* file_path for all the tracefiles*/
int trace_start;
int trace_end; 
int node_num;
int max_fp_num;
int max_blk_num = 200000;

//struct node* cluster = NULL;
struct traceline T_line;
char **default_argv = NULL;
struct metric metrics[METRICS_NUM];
char *file_prefix = (char *)"mobi.trace.";





//laddr_tree is for searching the file
struct rb_root laddr_tree;

struct fp_node * fp_store = NULL;

struct pblk_node **storage = NULL;
LIST_HEAD(lru_page_list);
//struct list_head lru_page_list;
long page_count_in_cache = 0;
unsigned cache_hit = 0;
unsigned cache_miss = 0;
long cache_size = 0;

/*define 6 situations of write*/
unsigned w_case_1 = 0;
unsigned w_case_2 = 0;
unsigned w_case_3 = 0;
unsigned w_case_4 = 0;
unsigned w_case_5 = 0;
unsigned w_case_6 = 0;

/*define 6 situations of read*/
unsigned r_case_1 = 0;
unsigned r_case_2 = 0;
unsigned r_case_3 = 0;
unsigned r_case_4 = 0;
unsigned r_case_5 = 0;
unsigned r_case_6 = 0;

long pblk_used = 0;
long pblk_in_mem_count = 0;


/*
 * 
 */
int main(int argc, char** argv) {
    
    
	char buffer[MAX_PATH_SIZE + MAX_META_SIZE];
	
	
	//Argv_Parse(argc, argv);
    
    Create_Default_Setting(&default_argv);
    //printf("defaul_argv[1]: %s\n", default_argv[1]);
    Argv_Parse(8, default_argv, file_prefix);

   
    
    Init_storage(MAX_BLK_NUM);
    
    //process all the files
    Process(files, trace_start, trace_end, &T_line);

    
    Output_Result(argv[4]);
 
    return 0;
}

