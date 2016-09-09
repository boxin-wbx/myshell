//
// Created by longmensparkle on 16/8/5.
//

#ifndef MYSHELL_BUILTIN_H
#define MYSHELL_BUILTIN_H

#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "main.h"


bool is_builtin(char *);

void clr();

void show_time();

void shell_exit();

void shell_quit();

void pwd();

void change_dir(char * pathname);

void show_environ();

void exec_builtin_cmd(char **argv);


#endif //MYSHELL_BUILTIN_H
