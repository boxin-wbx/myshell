//
// Created by longmensparkle on 16/8/5.
//

#include <netdb.h>
#include "utility.h"



char *malloc_sync(size_t i);


//错误报告函数
void err(char *error) {
    fflush(stdout);
    fprintf(stderr, "%s: ", SHELL_NAME);
    perror(error);
    errno = 0;
}

//获得用户名
char *get_uname() {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (errno) {
        return NULL;
    }
    return pw->pw_name;
}

//获得主机名
char *get_hostname() {
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    return strdup(hostname);
}

void string_cut(char *st, size_t i) {
    size_t len = strlen(st);
    memmove(st, st + i + 1, len - i);
}

void string_left(char *st, size_t i, cmd_t *cmd_list) {
    size_t j = 0, k = i - 1;
    while (st[j] == ' ') j++;
    while (k >= j && st[k] == ' ') k--;
    memcpy(cmd_list->cmd[cmd_list->n], st + j, k - j + 1);
    cmd_list->cmd[cmd_list->n++][k - j + 1 + 1] = 0;
}

// ; || && | < >
// 将行命令分解为 cmd_t 的格式
void string_explode(char *line, cmd_t *cmd_list) {
    cmd_list->n = 0;
    size_t len = strlen(line);
    line[--len] = 0;  // delete the last \n
    size_t i = 0;

    for (; i <= strlen(line); i++)
        switch (line[i]) {
            case '|' :
                if (line[i + 1] == 0)
                    exit(SHELL_PARSE_ERROR);
                else if (line[i + 1] == '|') {
                    if (i == 0) exit(SHELL_PARSE_ERROR);
                    string_left(line, i, cmd_list);
                    strcpy(cmd_list->cmd[cmd_list->n++], "||");
                    string_cut(line, i + 1);
                    i = 0;
                }
                else {
                    if (i == 0) exit(SHELL_PARSE_ERROR);
                    string_left(line, i, cmd_list);
                    strcpy(cmd_list->cmd[cmd_list->n++], "|");
                    string_cut(line, i);
                    i = 0;

                }
                break;
            case '>' :
                if (line[i + 1] == 0)
                    exit(SHELL_PARSE_ERROR);
                else if (line[i + 1] == '>') {
                    if (i == 0) exit(SHELL_PARSE_ERROR);
                    string_left(line, i, cmd_list);
                    strcpy(cmd_list->cmd[cmd_list->n++], ">>");
                    string_cut(line, i + 1);
                    i = 0;

                }
                else {
                    if (i == 0) exit(SHELL_PARSE_ERROR);
                    string_left(line, i, cmd_list);
                    strcpy(cmd_list->cmd[cmd_list->n++], ">");
                    string_cut(line, i);
                    i = 0;

                }
                break;

            case '<':
                if (line[i + 1] == 0)
                    exit(SHELL_PARSE_ERROR);
                else {
                    if (i == 0) exit(SHELL_PARSE_ERROR);
                    string_left(line, i, cmd_list);
                    strcpy(cmd_list->cmd[cmd_list->n++], "<");
                    string_cut(line, i);
                    i = 0;

                }
                break;

            case '&':
                if (line[i + 1] == '&') {
                    if (i == 0) exit(SHELL_PARSE_ERROR);
                    string_left(line, i, cmd_list);
                    strcpy(cmd_list->cmd[cmd_list->n++], "&&");
                    string_cut(line, i + 1);
                    i = 0;

                }
                else {
                    if (i == 0) exit(SHELL_PARSE_ERROR);
                    string_left(line, i, cmd_list);
                    strcpy(cmd_list->cmd[cmd_list->n++], "&");
                    string_cut(line, i);
                    i = 0;

                }
                break;

            case ';':
                if (i == 0) exit(SHELL_PARSE_ERROR);
                string_left(line, i, cmd_list);
                strcpy(cmd_list->cmd[cmd_list->n++], ";");
                string_cut(line, i);
                i = 0;

                break;
            case 0:
                if (i == 0) break;
                string_left(line, i, cmd_list);
                break;
            default:
                break;
        }
}

//删除工作号
int erase_job(job_t *job, pid_t pid) {
    job->n--;
    for (int i = 0; i < 80; i++)
        if (pid == job->jobs[i]) {
            job->jobs[i] = 0;
            return i + 1;
        }
    return 0;
}

//增加工作号
int add_job(job_t *job, pid_t pid) {
    job->n++;
    for (int i = 0; i < 80; i++)
        if (job->jobs[i] == 0) {
            job->jobs[i] = pid;
            return i + 1;
        }
    return 0;
}


