/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "storage.h"



void Init_storage(int max_pblk_num)
{

	storage = (struct pblk_node **)malloc(max_pblk_num * sizeof(struct pblk_node *));

	//printf("------------------------Init_Storage------------------------------\n");
	
	for(int i = 0; i < max_pblk_num; i++)
	{
		storage[i] = (struct pblk_node *)malloc(sizeof(struct pblk_node));
		
		storage[i]->ref_count = 0;
		storage[i]->fp = NULL;
		storage[i]->in_mem = 0;
	}

}


void Destroy_storage(int max_pblk_num)
{

}


/*Init a pblk_node*/
long Init_pblk_node()
{
	if(pblk_used < MAX_BLK_NUM -1)
	{
		for (int i = 0; i < MAX_BLK_NUM; ++i)
		{
			if(Pblk_is_free(i))
			{
				storage[i]->ref_count = 1;
				//storage[pblk_used]->fp = NULL;
				//storage[pblk_used]->in_mem = 0;/*Not in the memory*/

				pblk_used++;
				
				return i;
			}
		}

				
	}
	else
	{
		printf("Storage full: No space!\n");
		return -1;

	}


}


/*Increase Pblk_NR
void increase_pblk_nr(unsigned *pblk_nr)
{
	int count;

	count = 0;
	do{
		(*pblk_nr)++;
		
		if((*pblk_nr) >= (PBLK_SIZE - 1))
			(*pblk_nr) = 0;
		
		//printf("increase_pblk_nr\n");
		count++;
		
		if(count > PBLK_SIZE){
			//printf("pblk is full, allocate more\n");
			exit(-1);
		}
	}while(!pblk_is_free(storage, *pblk_nr))
}*/



int Pblk_is_free(unsigned pblk_nr)
{
	return (storage[pblk_nr]->ref_count == 0);
}

int Mark_pblk_free(unsigned pblk_nr)
{
	storage[pblk_nr]->ref_count == 0;
	storage[pblk_nr]->fp = NULL;
	storage[pblk_nr]->in_mem = 0;
	
	pblk_used--;
	
	return pblk_used;
}

int Pblk_is_in_mem(unsigned pblk_nr)
{
	return (storage[pblk_nr]->in_mem);
}

int Mark_pblk_in_mem(unsigned pblk_nr)
{
	storage[pblk_nr]->in_mem = 1;
	pblk_in_mem_count++;

	return pblk_in_mem_count++;
}








/* delete a pblk node by decreasing its ref_count*/
void Decrease_pblk_ref_count(unsigned pblk_nr)
{
	if (storage[pblk_nr]->ref_count == 0)
	{
		printf("Error: This pblk[%d] is not allocated\n", pblk_nr);
	}
	else
	{
		storage[pblk_nr]->ref_count--;

		if(storage[pblk_nr]->ref_count == 0)
		{
			/*1. mark this pblk unused 2. release fp_node */
			if (Pblk_is_in_mem(pblk_nr))
			{
				//Page_lru_del()
				storage[pblk_nr]->in_mem = 0;
			}
		
			Del_fp(storage[pblk_nr]->fp);
			
			Mark_pblk_free(pblk_nr);
		}
	}
	

	return;

}