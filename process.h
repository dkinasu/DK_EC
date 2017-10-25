/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   process.h
 * Author: dkinasu
 *
 * Created on October 23, 2017, 10:54 PM
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"
#include "storage.h"
#include "trace.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cerrno>

void Process(char **files, int trace_start, int trace_end, struct traceline *T_line);

void Write_Process(struct traceline *T_line);
void Read_Process(struct traceline *T_line);
void Delete_Process(struct traceline *T_line);

/*define 6 situations of write*/
extern unsigned w_case_1;
extern unsigned w_case_2;
extern unsigned w_case_3;
extern unsigned w_case_4;
extern unsigned w_case_5;
extern unsigned w_case_6;

/*define 6 situations of read*/
extern unsigned r_case_1;
extern unsigned r_case_2;
extern unsigned r_case_3;
extern unsigned r_case_4;
extern unsigned r_case_5;
extern unsigned r_case_6;


#endif /* PROCESS_H */

