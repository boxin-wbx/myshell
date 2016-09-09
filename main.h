//
// Created by longmensparkle on 16/8/5.
//

#ifndef MYSHELL_MAI_H
#define MYSHELL_MAI_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <wordexp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "utility.h"
#include "builtin.h"



typedef struct {
    char pathname[80];
    int do_redirection;
} redirect;

void input_redirection(char *pathname);

void init(bool);

void show_jobs();

void fg(int);

char * make_temp();

#define MAX_LINE 80

#endif //MYSHELL_MAI_H
