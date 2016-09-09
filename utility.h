//
// Created by longmensparkle on 16/8/5.
//

#ifndef MYSHELL_UTILITY_H
#define MYSHELL_UTILITY_H

#define SHELL_NAME "myshell"
#define SHELL_QUIT 4
#define SHELL_COMMAND_ERROR 3
#define SHELL_PARSE_ERROR 1
#define SHELL_INTERNAL_ERROR 2

#include <errno.h>
#include "main.h"
#include <unistd.h>
#include <pwd.h>

typedef struct {
    char cmd[80][80];
    size_t n;
} cmd_t;

typedef struct {
    pid_t jobs[80];
    size_t n;
} job_t;

void exec_line(cmd_t *);

int erase_job(job_t * job, pid_t pid);

int add_job(job_t *job, pid_t pid);

void err(char *);

char * get_uname();

char * get_hostname();

void string_explode(char *, cmd_t *);

#endif //MYSHELL_UTILITY_H
