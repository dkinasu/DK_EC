/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "process.h"


void binary_to_hex(char *dst, char *src, size_t src_size)
{
	int x;

	for (x = 0; x < src_size; x++)
		sprintf(&dst[x * 2], "%.2x", (unsigned char)src[x]);
	return;
}

char *hex_to_binary(char *src, size_t src_size)
{
	int x;

	char *result = (char *)malloc((src_size + 1) / 2);
	for (x = 0; x < src_size; x++){
		char c = src[x];
		unsigned int bin = (c > '9') ?
		   (tolower(c) - 'a' + 10) : (c - '0');
		if (x % 2 == 0) {
			result[x / 2] = bin << 4;
		} else {
			result[x / 2] |= bin;
		}
	}
	return result;
}





/*Process Write Request: 6 cases*/
void Write_Process(struct traceline *T_line)
{
	char *dst;
	int w_case;
	
	//keep track of total write num for all the tracefiles
	metrics[WRITES_NUM].total++;
	metrics[WRITES_BYTES].total += DATA_SIZE;
	//metrics[FINGERPRINT_NUM].total ++;
	
	T_line->pos = T_line->pos / DATA_SIZE;

	/*convert the sha1value(in hex) into binary*/
	dst = hex_to_binary(T_line->databuf, HASHVALSIZE);	
	memset(T_line->databuf, '\0', HASHVALSIZE);
	memcpy(T_line->databuf, dst, SHA1SIZE);
	T_line->databuf[SHA1SIZE] = '\0';
	//printf("after hex: %s\n", dst);
	free(dst);



	//search for the file_path
	struct laddr_node *laddr_node = NULL;
	struct page_node *page_node = NULL;
	struct fp_node *fp = NULL;

	//struct disk_pair *disk_pair = NULL;
	unsigned old_pblk_nr;
	int found_file, found_page, found_fp;
	found_file = -1;
	found_page = -1;
	found_fp = -1;

	unsigned pblk_nr;
	int ref_count = 1;
	int ret;

	/*try to find the file_path in the laddr_tree( which is a rb_tree, and this rb_tree is in memory)*/
	laddr_node = Find_filepath(&laddr_tree, T_line->file_path);
	fp = Find_fp(T_line->databuf);

	if (laddr_node != NULL)
	{
		found_file = 1;
		//trying to find if the page exists
		page_node = Find_page(laddr_node->page_tree, T_line->pos);
		
		if (page_node != NULL)
		{
			//printf("Found page!\n");	
			found_page = 1;		
			
			if (fp != NULL)
			{
				found_fp = 1;
				w_case_3++;
				//printf("Case 3[%d]: Same page in Same file with Same fp!\n", w_case_3);
				/*1. data_LRU_adjust*/

				if(!Pblk_is_in_mem(page_node->pblk_nr))
				{
					if(Is_cache_full())
					{
						printf("Cache is full! Need evict\n");
						Page_lru_evict(1);
						/*need evict a page*/
					}
					Page_lru_add(page_node);
				}

				Page_lru_accessed_adjust(page_node);

				fp->hit++;
			}
			else
			{
				w_case_4++;
				//printf("Case 4[%d]: Same page in Same file with New fp!\n", w_case_4);
				/*1.add fp 2. data_LRU_adjust*/
				pblk_nr = Init_pblk_node();
				fp = Init_fp_node(T_line->databuf, pblk_nr);
				Add_fp(fp);
				storage[pblk_nr]->fp = fp;

				Del_page_node(laddr_node->page_tree, page_node);

				page_node = Init_page_node(T_line->pos, pblk_nr);
				//page_node->fp_node = fp;
				Add_page_node(laddr_node->page_tree, page_node);
				
				if(Is_cache_full())
				{
					printf("Cache is full! Need evict\n");
					Page_lru_evict(1);
					/*need evict a page*/
				}

				Page_lru_add(page_node);
				Page_lru_accessed_adjust(page_node);
			}
				
		}
		else
		{
			//printf("New page!\n");
			found_page = 0;
			
			if (fp != NULL)
			{
				found_fp = 1;
				w_case_1++;
				
				//printf("Case 1[%d]: New page in Same file with Same fp!\n", w_case_1);

				/*1. add new page but no new pblk 2. increase ref_count 3. data_LRU_adjust*/
				fp->hit++;
				page_node = Init_page_node(T_line->pos, fp->pblk_nr);
				storage[page_node->pblk_nr]->ref_count++;
				Add_page_node(laddr_node->page_tree, page_node);

				if(!Pblk_is_in_mem(page_node->pblk_nr))
				{
					if(Is_cache_full())
					{
						printf("Cache is full! Need evict\n");
						Page_lru_evict(1);
						/*need evict a page*/
					}

					Page_lru_add(page_node);
				}

				Page_lru_accessed_adjust(page_node);
							
			}
			else
			{
				w_case_2++;
				//printf("Case 2[%d]: New page in Same file with New fp!\n", w_case_2);
				/*1. add new page 2. distribute new pblk 3. add fp 4. data_LRU_adjust*/

				pblk_nr = Init_pblk_node();
				fp = Init_fp_node(T_line->databuf, pblk_nr);
				Add_fp(fp);
				storage[pblk_nr]->fp = fp;

				page_node = Init_page_node(T_line->pos, fp->pblk_nr);
				Add_page_node(laddr_node->page_tree, page_node);

				if(Is_cache_full())
				{
					printf("Cache is full! Need evict\n");
					Page_lru_evict(1);
					/*need evict a page*/
				}

				Page_lru_add(page_node);
				Page_lru_accessed_adjust(page_node);


			}
		}
	}
	else
	{
		//printf("New file\n");
		found_file = 0;

		laddr_node = Init_laddr_node(T_line->file_path);
		Add_laddr_node(&laddr_tree,laddr_node);	

		if (fp != NULL)
		{
			found_fp = 1;
			w_case_5++;
			//printf("Case 5[%d]: New file with old fp!\n", w_case_5);
			/*0. add new file 1. add new page but no new pblk 4. data_LRU_adjust*/
			page_node = Init_page_node(T_line->pos, fp->pblk_nr);
			storage[fp->pblk_nr]->ref_count++;
			Add_page_node(laddr_node->page_tree, page_node);
				
			if(!Pblk_is_in_mem(page_node->pblk_nr))
			{
				if(Is_cache_full())
				{
					printf("Cache is full! Need evict\n");
					/*need evict a page*/
					Page_lru_evict(1);
				}
				Page_lru_add(page_node);
			}

			Page_lru_accessed_adjust(page_node);							
		}
		else
		{
			w_case_6++;
			//printf("Case 6[%d]: Mew file with New fp!\n", w_case_6);
			/*0. add new file 1. add new page 2. distribute new pblk 3. add fp 4. data_LRU_adjust*/
			pblk_nr = Init_pblk_node();
			fp = Init_fp_node(T_line->databuf, pblk_nr);
			Add_fp(fp);
			storage[pblk_nr]->fp = fp;
			//printf("Finishing adding fp & pblk!\n");

			page_node = Init_page_node(T_line->pos, fp->pblk_nr);
			Add_page_node(laddr_node->page_tree, page_node);

			if(Is_cache_full())
			{
					printf("Cache is full! Need evict\n");
					/*need evict a page*/
					Page_lru_evict(1);
			}
			//printf("Finishing adding page!\n");
			Page_lru_add(page_node);
			Page_lru_accessed_adjust(page_node);

		}		
	}


}

//Process a Read request
void Read_Process(struct traceline *T_line)
{
    //printf("Start Reading\n");

    T_line->pos = T_line->pos / DATA_SIZE;
	struct laddr_node * laddr_node;

	laddr_node = Find_filepath(&laddr_tree, T_line->file_path);

	if(laddr_node == NULL)
	{
		r_case_1++;
		//printf("READ case 1: [Failed] NO such file\n");
		return ;
	}
	else
	{
		//printf("Found file\n");
		
		struct page_node * res;

		res = Find_page(laddr_node->page_tree, T_line->pos);

		if(res == NULL)
		{
			r_case_3++;
			//printf("READ case 3: [Failed] NO such page in the file\n");
			return ;
		}
		else
		{
			if(Pblk_is_in_mem(res->pblk_nr))
			{
				r_case_2++;
				cache_hit++;
				//printf("READ case 2: cache hit\n");
				// adjust the lru list 
				Page_lru_accessed_adjust(res);
			}
			else
			{
				r_case_4++;
				//printf("READ case 4: cache miss\n");
				
				//*1. bring the pblk into memory 2. Mark it 3. Ajust LRU
				if(Is_cache_full())
				{
					printf("Cache is full! Need evict\n");
					Page_lru_evict(1);
					/*need evict a page*/
				}

				Page_lru_add(res);
				Page_lru_accessed_adjust(res);	
			}
				
		}		

	}	

}


//
void Delete_Process(struct traceline *T_line)
{
	metrics[DELETE_NUM].total++;
	Del_file(&laddr_tree, T_line->file_path);
}




void Process(char **files, int trace_start, int trace_end, struct traceline *T_line)
{
	FILE *file;
	int line_count, write_count, read_count, delete_count;
	char buffer[MAX_PATH_SIZE + MAX_META_SIZE];

	printf("\n------------------------Start Processing------------------------------\n");



	for (int i = trace_start; i < trace_end; ++i)
	{
		

		file = fopen(files[i], "r");

		//printf("%s\n",files[i]);
		
		//check if open successfully
		if (file == NULL) 
		{
			printf("Open tracefile[%d] fail %s\n", i, strerror(errno));
			continue;
		}
	
		line_count = 0;
		write_count = 0;
		read_count = 0;
		delete_count = 0;

		Is_cache_full();

	    //printf("------------------------Start Processing a File-----------------------------\n");

	    while(fgets(buffer, MAX_PATH_SIZE + MAX_META_SIZE, file)) 
	    {
	    	line_count++;

		    if(TraceLine_Parse(buffer, T_line)==1)
		    {
		    	//Print_traceline(T_line);
				metrics[REQUESTS].total++;

				//if this line of request is rw = read or write   rw == 1, means write?  Yes
				if(T_line->rw == 1 && (strlen(T_line->databuf) == 40))
				{

					write_count++;
					//printf("Write_count is:%d\n", write_count);
					Write_Process(T_line);
					//printf("Write Process Done!\n");
				} 
				else if (T_line->rw == 0)
				{
					read_count++;
					//printf("Read_count is:%d\n", read_count);
					Read_Process(T_line);
					//printf("Read Process Done!\n");
				}
				else if(T_line->rw == 3)
				{
					delete_count++;
					//printf("Delete_count is:%d\n", delete_count);
					Delete_Process(T_line);
					//printf("Delete Process Done!\n");
				}

				Clear_Traceline(T_line);
				
				//printf("%s hit: %d, total: %llu, dup_rate: %f count:%d\n", path[i], hit, report[FINGERPRINT_NUM].total, (double)hit/(double)report[FINGERPRINT_NUM].total, count);
		    }

		}

		printf("file[%d] || linecount: %d || writecount: %d || readcount: %d || deletecount: %d\n", i, line_count, write_count, read_count, delete_count);
		printf("Read:  case 1: %d || case 2: %d || case 3: %d || case 4: %d\n", r_case_1, r_case_2, r_case_3, r_case_4);
		printf("Write: case 1: %d || case 2: %d || case 3: %d || case 4: %d || case 5: %d|| case 6: %d\n", w_case_1, w_case_2, w_case_3, w_case_4, w_case_5, w_case_6);

	}
	
	printf("------------------------Finish Processing------------------------------\n");

}
