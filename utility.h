//
// Created by longmensparkle on 16/8/5.
//

#ifndef MYSHELL_UTILITY_H
#define MYSHELL_UTILITY_H

#define SHELL_NAME "myshell"

#include <errno.h>
#include "main.h"
#include <unistd.h>
#include <pwd.h>

void err(char *);

char * get_uname();

char * get_hostname();

#endif //MYSHELL_UTILITY_H
