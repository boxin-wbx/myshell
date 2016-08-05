//
// Created by longmensparkle on 16/8/5.
//

#include <netdb.h>
#include "utility.h"

void err(char *error) {
    fflush(stdout);
    fprintf(stderr, "%s: ", SHELL_NAME);
    perror(error);
    errno = 0;
}



char *get_uname() {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (errno) {
        return NULL;
    }
    return pw->pw_name;
}


char *get_hostname() {
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    return strdup(hostname);
}

