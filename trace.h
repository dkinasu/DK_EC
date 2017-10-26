/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   trace.h
 * Author: dkinasu
 *
 * Created on October 19, 2017, 11:11 AM
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include "memory.h"
#include "storage.h"

#ifndef TRACE_H
#define TRACE_H


#define TASK_COMM_LEN 20
#define SIZE_PER_FPRECORD 40
#define DATA_SIZE 4096


#define HASHVALSIZE 40
#define SHA1SIZE 20
#define MAX_PATH_SIZE 5120
#define MAX_META_SIZE 100000

#define METRICS_NUM 7
#define REQUESTS 0
#define WRITES_NUM 1
#define WRITES_BYTES 2
#define DELETE_NUM 3
#define FINGERPRINT_NUM 4
#define FINGERPRINT_DELETE_NUM 5
#define PAGE_SIZE_WRONG 6 

extern char ** files;
extern int trace_start; 
extern int trace_end;  
extern int node_num;
extern int max_fp_num;
extern int max_blk_num;
extern struct node* cluster;
extern struct metric metrics[METRICS_NUM];
extern int total_line, line_count, write_count, read_count, delete_count, other_count;


//a struct for each line of trace
struct traceline {
	char *file_path;
	long unsigned int data_size;
	long int pos;
	long unsigned int inode_no;
	char *databuf;
	int rw;//read or write
	char pname[TASK_COMM_LEN];
	unsigned long long time_stamp;
};

int TraceLine_Parse(char * buffer, struct traceline *T_line);
int Clear_Traceline(struct traceline *T_line);
void Print_traceline(struct traceline *T_line);



//define a metric for output, like total_write_num.
struct metric {
	char *mtrk_name;
	long long unsigned total;
	long long unsigned fails;
	long long unsigned seek_fails;
	long long unsigned open_fails;
};





void Output_Result(char *result);
int Argv_Parse(int argc, char ** argv, char *file_prefix);

void Create_Default_Setting(char *** p);

#endif /* TRACE_H */

