/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   memory.h
 * Author: dkinasu
 *
 * Created on October 23, 2017, 4:47 PM
 */

#ifndef MEMORY_H
#define MEMORY_H

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "trace.h"
#include "rbtree.h"
#include "lish.h"
#include "uthash.h"
#include "storage.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>

#define CACHE_SIZE 2000000 /*unit is pages*/


/*
struct in_mem_pos{
	struct rb_root *in_mem_tree;
	struct list_head pos_list;
	//int pair_count;
	struct list_head lru_list;
};
*/



/* define fp_node struct and all the related operation */
struct fp_node{	
	bool identity;
  	struct rb_node node;
  	int hit;
  	//struct in_mem_pos *pos;
	char fingerprint[SHA1SIZE]; //make it as the key/
	unsigned pblk_nr; /*physical block number*/
	//char status;
	UT_hash_handle hh; /* makes this structure hashable */
};

extern struct fp_node * fp_store; /* hashtable for fp_store */
extern unsigned fp_count;
extern struct list_head lru_fp_list;

struct fp_node *Init_fp_node(char *fingerprint, unsigned pblk_nr);
void Add_fp(struct fp_node *s);
struct fp_node * Find_fp(char * fp);
void Del_fp(struct fp_node* s);
void Print_fps();





/* define logic address struct for a file and all the related operation */
struct laddr_node{
  	struct rb_node node;
	char *file_path;
  	struct rb_root *page_tree; /* each laddr_node has a rb_page_tree*/
};



//search a file in a rb_tree
struct laddr_node *Init_laddr_node(char *file_path);
struct laddr_node *Find_filepath(struct rb_root *root, char *file_path);
void Add_laddr_node(struct rb_root *root, struct laddr_node *laddr_node);
void Del_laddr_node(struct rb_root *root, struct laddr_node *delete_node);


//laddr_tree is for searching the file
extern struct rb_root laddr_tree;





/* define page_node struct struct for a page */
struct page_node{
  	struct rb_node node;
  	struct list_head lru_list;
	long int pos;/*the ith numbered page in a file*/
  	unsigned pblk_nr; /*corresponding pblk number*/
};

//
struct page_node *Init_page_node(long int pos, unsigned pblk_nr);
struct page_node *Find_page(struct rb_root *root, long int pos);
void Add_page_node(struct rb_root *root, struct page_node *page_node);
void Del_page_node(struct rb_root *root, struct page_node *delete_node);

void Destroy_page_tree(struct rb_root *root);





//add the fp into the list head with lru policy
void Page_lru_add(struct page_node *p);
void Page_lru_del(struct page_node *p);
void Print_lru_cache();
int Is_cache_full();
void Page_lru_evict(int n);/*evict n lru pages out*/

struct page_node * Find_LRUed_page(); /*this function returns the least used page*/
struct page_node * Find_page_node_lru(long int pos); /*this function is not working*/

void Page_lru_accessed_adjust(struct page_node *p);


extern struct list_head lru_page_list;
extern unsigned cache_hit;
extern unsigned cache_miss;
extern long page_count_in_cache;
extern long cache_size;





#endif /* MEMORY_H */

