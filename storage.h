/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   storage.h
 * Author: dkinasu
 *
 * Created on October 23, 2017, 4:47 PM
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <cstdlib>
#include <cstdio>
#include "memory.h"

#define PBLK_SIZE 20971520
#define MAX_BLK_NUM 200000



/*get the fp_node of the pblk*/
#define pblk_get_addr(pblk_list, pblock)	\
					(struct fp_node *)((pblk_list)[(pblock)]->fp_node)
/*set the fp_node of the pblk*/
#define pblk_set_addr(pblk_list, pblock, node)	\
					(((pblk_list)[(pblock)])->fp = (void *)node)				




/* define physical block_node struct */
struct pblk_node
{
	int ref_count;
	struct fp_node* fp;
	int in_mem; /*whether it is in the cache*/
};


long Init_pblk_node();
int Pblk_is_free(unsigned pblk_nr);
int Mark_pblk_free(unsigned pblk_nr);
int Pblk_is_in_mem(unsigned pblk_nr);
int Mark_pblk_in_mem(unsigned pblk_nr);
void Del_pblk_node(unsigned pblk_nr);



extern struct pblk_node **storage;
extern long pblk_used;
extern long pblk_in_mem_count;



void Init_storage(int max_pblk_num);
void Destroy_storage(int max_pblk_num);


#endif /* STORAGE_H */

