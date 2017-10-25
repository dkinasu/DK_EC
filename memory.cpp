/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "memory.h"


/*operation for the fp_node*/

struct fp_node *Init_fp_node(char *fingerprint, unsigned pblk_nr)
{
	struct fp_node *new_node = NULL;
	new_node = (struct fp_node*)malloc(sizeof(struct fp_node)); 
	
	if (!new_node)
		return NULL;

	if (fingerprint != NULL)
	{
		memcpy(new_node->fingerprint, fingerprint, SHA1SIZE);
		new_node->fingerprint[SHA1SIZE] = '\0';
		rb_init_node(&new_node->node);
		new_node->identity = true;
		new_node->hit = 0;
		//new_node->pos = NULL;
		new_node->pblk_nr = pblk_nr;
		//new_node->status = FP_IS_NORM;
		return new_node;
	} else 
	{
		printf("fingerprint doesn't exist!");
		return NULL;
	}
}





void Add_fp(struct fp_node *s)
{
	HASH_ADD_STR(fp_store, fingerprint ,s);
}


struct fp_node * Find_fp(char * fp)
{
	struct fp_node *s;

    HASH_FIND_STR( fp_store, fp, s);  
    return s;
}


void Del_fp(struct fp_node* s)
{
	 HASH_DEL(fp_store, s);
}

void Print_fps()
{
	struct fp_node *s;

    for(s = fp_store; s != NULL; s = (struct fp_node *) s->hh.next) {
        printf("Fingerprint is %s:  pblk_nr%u\n", s->fingerprint, s->pblk_nr);
    }

}


/*
//add the fp into the list head with lru policy
static inline void fp_lru_add(struct list_head *lru_list, struct in_mem_pos *fp)
{
	list_add(&fp->lru_list, lru_list);
	//fp_lru_print(lru_list);
}*/



/*search a file in a rb_tree*/
struct laddr_node *Init_laddr_node(char *file_path)
{
	struct laddr_node *new_node = NULL;

	struct rb_root *page_tree;

	new_node = (struct laddr_node *) malloc(sizeof(struct laddr_node));

	if (!new_node)
	{
		return NULL;
	}

	/*Initialiation*/
	new_node->file_path = (char *) malloc(strlen(file_path) + 1);
	memcpy(new_node->file_path, file_path, strlen(file_path));
	new_node->file_path[strlen(file_path)] = '\0';

	/*new_node itself is a rb_node to be inserted in the laddr_tree*/	
	rb_init_node(&new_node->node);
	page_tree = (struct rb_root*) malloc(sizeof(struct rb_root));
	page_tree->rb_node = NULL;
	new_node->page_tree = page_tree;

	return new_node;
}

struct laddr_node *Find_filepath(struct rb_root *root, char *file_path)
{
	struct rb_node *new_node = root->rb_node;

	int result;

	/*iterate the rb_tree*/
	while(new_node)
	{
		//container is just like convert new_node->node into struct ladd_node *
		struct laddr_node *tmp = container_of(new_node, struct laddr_node, node);

		if (strlen(file_path) < strlen(tmp->file_path))
		{
			result = -1;
		}
		else if (strlen(file_path) > strlen(tmp->file_path))
			result = 1;
		else
			result = memcmp(file_path, tmp->file_path, strlen(tmp->file_path));

		if(result < 0)
			new_node = new_node->rb_left;
		else if (result > 0)
			new_node = new_node->rb_right;
		else
		{
			assert((result == 0) && (strlen(file_path) == strlen(tmp->file_path)));
	  		return tmp;
		}

	}

	return NULL;
}

/*insert the node into the ladd_tree. */
void Add_laddr_node(struct rb_root *root, struct laddr_node *laddr_node)
{
	struct rb_node **new_node = &(root->rb_node), *parent = NULL;

	int result;

	while (*new_node) 
  	{
  		
  		struct laddr_node *tmp = container_of(*new_node, struct laddr_node, node);
		parent = *new_node;

		if ((strlen(laddr_node->file_path) < strlen(tmp->file_path)))
  			result = -1;
  		else if ((strlen(laddr_node->file_path) > strlen(tmp->file_path)))
  			result = 1;
  		else
  			result = memcmp(laddr_node->file_path, tmp->file_path, strlen(tmp->file_path));
		
		if (result < 0)
			new_node = &((*new_node)->rb_left);
		else if (result > 0)
			new_node = &((*new_node)->rb_right);
  		else 
  		{
	  		assert(0);
  		}
  	}

  	rb_link_node(&laddr_node->node, parent, new_node);
  	rb_insert_color(&laddr_node->node, root);

}

void Del_laddr_node(struct rb_root *root, struct laddr_node *delete_node)
{
	int x = 1;
	
	if(delete_node)
	{
		free(delete_node->file_path);
		delete_node->file_path = NULL;
		rb_erase(&delete_node->node, root);
		free(delete_node);
		delete_node = NULL;
	}
	else
	{
		assert(x == 0);
	}

}




struct page_node *Init_page_node(long int pos, unsigned pblk_nr)
{
	struct page_node * new_node = NULL;
	new_node = (struct page_node *)malloc(sizeof(struct page_node));

	if(!new_node)
		return NULL;

	new_node->lru_list.next = &(new_node->lru_list);
	new_node->lru_list.prev = &(new_node->lru_list);

	new_node->pos = pos;
	new_node->pblk_nr = pblk_nr;

	rb_init_node(&new_node->node);
	return new_node;
}

struct page_node *Find_page(struct rb_root *root, long int pos)
{
	struct rb_node *node = root->rb_node;
  	
  	while (node) 
  	{
  		struct page_node *tmp = container_of(node, struct page_node, node);
		
		if (pos < tmp->pos)
			node = node->rb_left;
		else if (pos > tmp->pos)
			node = node->rb_right;
  		else
	  		return tmp;
  	}
  	return NULL;

}

void Add_page_node(struct rb_root *root, struct page_node *page_node)
{
	struct rb_node **tmp = &(root->rb_node), *parent = NULL;
	int x = 1;

  	while (*tmp) 
  	{
  		struct page_node *this_node = container_of(*tmp, struct page_node, node);
		
		parent = *tmp;
		
		if (page_node->pos < this_node->pos)
			tmp = &((*tmp)->rb_left);
		else if (page_node->pos > this_node->pos)
			tmp = &((*tmp)->rb_right);
  		else 
  		{
	  		assert(x == 0);
  		}
  	}

  	rb_link_node(&page_node->node, parent, tmp);
  	rb_insert_color(&page_node->node, root);


}

void Del_page_node(struct rb_root *root, struct page_node *delete_node)
{


	rb_erase(&delete_node->node, root);
	free(delete_node);
	delete_node = NULL;

}


//Destroying the rb_page tree
void Destroy_page_tree(struct rb_root *root)
{
	struct page_node *page_node;
	struct rb_node *node;

	node = rb_first(root);
	page_node = rb_entry(node, struct page_node, node);

	while (page_node) 
	{
		node = rb_next(&page_node->node);
		rb_erase(&page_node->node, root);
		free(page_node);
		
		if (!node) 
		{
			page_node = NULL;
			break;
		}
		
		page_node = rb_entry(node, struct page_node, node);
	}
}


//add a page node to the data LRU list
void Page_lru_add(struct page_node *p)
{	
	list_add(&p->lru_list, &lru_page_list);
	Mark_pblk_in_mem(p->pblk_nr);
	page_count_in_cache++;
	//printf("Finish Adding new page into LRU\n");
}

void Page_lru_del(struct page_node *p)
{
	list_del(&p->lru_list);
	storage[p->pblk_nr]->in_mem = 0;
	page_count_in_cache--;
	//printf("Finish Deleting page from LRU\n");
}

void Page_lru_accessed_adjust(struct page_node *p)
{
	Page_lru_del(p);
	Page_lru_add(p);
	//printf("Finishing Adjusting LRU\n");
}

int Is_cache_full()
{
	printf("page_count_in_cache: %ld cache_size: %ld\n", page_count_in_cache, cache_size);
	return (page_count_in_cache >= cache_size);
}


/*this function has a infinite loop, need debugging*/
struct page_node * Find_page_node_lru(long int pos)
{
	int counter = 0;
	
	struct page_node *tmp;
	
	struct list_head *p, *q;

	assert(!list_empty(&lru_page_list));

	printf("iterating LRU lists\n");
	
	list_for_each_safe(p, q, &lru_page_list) 
	{
		
		counter++;
		printf("Counter = %d\n", counter);

		tmp = list_entry(p, struct page_node, lru_list);
		
		//printf("tmp->pos: %ld\n", tmp->pos);
		
		if (tmp->pos == pos)
		{
			//printf("cache hit!\n");
			return tmp;
		}
		
	}

	printf("[%d] End searching LRU\n", counter);

	return NULL;
}