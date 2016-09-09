#include <wordexp.h>
#include "main.h"
#include "utility.h"

//打印带颜色的命令提示符
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


bool background = false;

job_t * job;


//jobs funciton
void show_jobs() {
    if (job->n == 0) {
        printf("No current job!\n");
        return;
    }
    printf("total %d are running.\n", job->n);
    for (int i = 0; i < 80; i++)
        if (job->jobs[i] != 0) {
            printf("[%d] %d running\n", i + 1, job->jobs[i]);
        }
}

//fg function
void fg(int job_no) {
    int status;
    waitpid(job->jobs[job_no - 1], &status, 0);
}

//set 命令提示符
void set_title() {
    char *euname = get_uname();
    if (errno) {
        err("geteuname");
    }

    char *cwd = getcwd(NULL, 0);
    if (errno) {
        err("getcwd");
    }
    printf(YEL "%s" RESET BLU " %s " RESET"$ ", euname, cwd);
    free(cwd);
}

// 生成临时文件
char *make_temp() {
    char pathname[80] = "/tmp/pipeXXXXXX";
    int fd = mkstemp(pathname);
    char pathfile[80];
    char *pathfile2;
    pathfile2 = malloc(80);
    sprintf (pathfile, "/proc/self/fd/%d", fd);
    ssize_t null_byte = readlink(pathfile, pathfile2, 80);
    pathfile2[null_byte] = 0;
    close(fd);
    return pathfile2;
}

// mac bug
//输入重定向
void input_redirection(char *pathname) {
    int fd = open(pathname, O_RDONLY);
    if (errno) {
        err(pathname);
        exit(SHELL_INTERNAL_ERROR);
    }
    dup2(fd, STDIN_FILENO);
    if (errno) {
        err("dup2");
        exit(SHELL_INTERNAL_ERROR);
    }
    close(fd);
    if (errno) {
        err("close");
        exit(SHELL_INTERNAL_ERROR);
    }
}

//输出重定向
void output_redirection(char *pathname) {
    int fd = creat(pathname, S_IRWXU);
    if (errno) {
        err(pathname);
        exit(SHELL_INTERNAL_ERROR);
    }
    dup2(fd, STDOUT_FILENO);
    if (errno) {
        err("dup2");
        exit(SHELL_INTERNAL_ERROR);
    }
    close(fd);
    if (errno) {
        err("close");
        exit(SHELL_INTERNAL_ERROR);
    }
}

//追加重定向
void append_redirection(char *pathname) {
    int fd = open(pathname, O_WRONLY | O_APPEND, S_IRWXU);
    if (errno) {
        errno = 0;
        fd = creat(pathname, S_IRWXU);
        if (errno) {
            err(pathname);
            exit(SHELL_INTERNAL_ERROR);
        }
    }
    dup2(fd, STDOUT_FILENO);
    if (errno) {
        err("dup2");
        exit(SHELL_INTERNAL_ERROR);
    }
    close(fd);
    if (errno) {
        err("close");
        exit(SHELL_INTERNAL_ERROR);
    }
}

//执行外部命令
int exec_sys(const char *program, redirect in, redirect out) {
    wordexp_t result;
    pid_t pid;
    int status, i;

    /* Expand the string for the program to run.  */
    switch (wordexp(program, &result, 0)) {
        case 0:            /* Successful.  */
            break;
        case WRDE_NOSPACE:
            /* If the error was WRDE_NOSPACE,
               then perhaps part of the result was allocated.  */
            wordfree(&result);
        default:                    /* Some other error.  */
            return -1;
    }

    pid = fork();
    if (pid == 0) {
        pid_t child_pid = getpid();
        if (background) printf("[%d] %d\n", add_job(job, child_pid), child_pid);
        /* This is the child process.  Execute the command. */
        pid_t pid2 = fork();
        if (pid2 == 0) {
            if (in.do_redirection) input_redirection(in.pathname);
            if (out.do_redirection == 1) output_redirection(out.pathname);
            if (out.do_redirection == 2) append_redirection(out.pathname);
            execvp(result.we_wordv[0], result.we_wordv);
            exit(SHELL_COMMAND_ERROR);
        }
        else if (pid2 < 0) exit(SHELL_INTERNAL_ERROR);
        else {
            int status2;
            if (waitpid(pid2, &status2, 0) != pid2)
                exit(EXIT_FAILURE);
            if (background) printf("[%d] %d done!\n", erase_job(job, child_pid), child_pid);
            if (WEXITSTATUS(status2) == 3)
                fprintf(stderr, "%s: %s\n", SHELL_NAME, "Command not found");
            exit(WEXITSTATUS(status2));
        }
    }
    else {
        if (pid < 0) // report fork error
        {
            err("fork");
            return SHELL_INTERNAL_ERROR;
        }
        else {/* This is the parent process.  Wait for the child to complete.  */
            status = 0;
            if (!background) if (waitpid(pid, &status, 0) != pid) err("wait");
            wordfree(&result);
            return status;
        }
    }
}


//执行内部命令
int exec_builtin(char **argv, redirect in, redirect out) {
    pid_t pid;
    int status, i;

    pid = fork();
    if (pid == 0) {
        pid_t child_pid = getpid();
        if (background) printf("[%d] %d\n",add_job(job, child_pid), child_pid);
        /* This is the child process.  Execute the command. */
        pid_t pid2 = fork();
        if (pid2 == 0) {
            if (in.do_redirection) input_redirection(in.pathname);
            if (out.do_redirection == 1) output_redirection(out.pathname);
            if (out.do_redirection == 2) append_redirection(out.pathname);
            exec_builtin_cmd(argv);
            exit(SHELL_COMMAND_ERROR);
        }
        else if (pid2 < 0) exit(SHELL_INTERNAL_ERROR);
        else {
            int status2;
            if (waitpid(pid2, &status2, 0) != pid2)
                exit(EXIT_FAILURE);
            if (background) printf("[%d] %d done!\n",erase_job(job, child_pid), child_pid);
            if (WEXITSTATUS(status2) == 3)
                fprintf(stderr, "%s: %s\n", SHELL_NAME, "Command not found");
            exit(WEXITSTATUS(status2));
        }

    }
    else if (pid < 0) {
        /* The fork failed.  Report failure.  */
        err("fork");
        return SHELL_INTERNAL_ERROR;
    }
    else {
        /* This is the parent process.  Wait for the child to complete.  */
        status = 0;
        if (!background) if (waitpid(pid, &status, 0) != pid) err("wait");
        return status;
    }
}

//根据情况选择执行相应命令
int exec_cmd(char *cmd, redirect input_redirection, redirect output_redirection) {

    wordexp_t result;
    int status;

    /* Expand the string for the program to run.  */
    switch (wordexp(cmd, &result, 0)) {
        case 0:            /* Successful.  */
            break;
        case WRDE_NOSPACE:
            /* If the error was WRDE_NOSPACE,
               then perhaps part of the result was allocated.  */
            wordfree(&result);
        default:                    /* Some other error.  */
            return -1;
    }

    if (is_builtin(result.we_wordv[0])) {
        status = exec_builtin(result.we_wordv, input_redirection, output_redirection);
        if (strcmp(result.we_wordv[0], "cd") == 0 && WEXITSTATUS(status) == 0) {
            if (result.we_wordv[1] != NULL) {
                change_dir(result.we_wordv[1]);
            }
            else {
                char *pathname = getenv("HOME");
                change_dir(pathname);
            }
        }
        else if (strcmp(result.we_wordv[0], "umask") == 0 && result.we_wordv[1] != 0) {
            mode_t mode;
            sscanf(result.we_wordv[1], "%o", &mode);
            umask(mode);
        }
        else if (strcmp(result.we_wordv[0], "fg") == 0 &&  WEXITSTATUS(status) == 0) {
            int job_no;
            char ** argv = result.we_wordv;
            sscanf(argv[1], "%%%d", &job_no);
            fg(job_no);
        }
    }
    else
        status = exec_sys(cmd, input_redirection, output_redirection);

    wordfree(&result);
    return status;
}

bool isPipe(char *cmd) {
    return (0 == strcmp(cmd, "|"));
}

bool isInput(char *cmd) {
    return (0 == strcmp(cmd, "<"));
}

bool isOutput(char *cmd) {
    return (0 == strcmp(cmd, ">"));
}

bool isAppend(char *cmd) {
    return (0 == strcmp(cmd, ">>"));
}

bool isAnd(char *cmd) {
    return (0 == strcmp(cmd, "&&"));
}

bool isOR(char *cmd) {
    return ((0 == strcmp(cmd, "||")) || (0 == strcmp(cmd, ";")));
}

bool isBackground(char *cmd) {
    return (0 == strcmp(cmd, "&"));
}

//执行该行命令
void exec_line(cmd_t *cmd_list) {
    int exit_code = EXIT_SUCCESS;
    char *last_command = 0;
    bool do_pipe = false;
    redirect in_redirect, out_redirect;
    in_redirect.do_redirection = out_redirect.do_redirection = false;
    int do_redirect = 0;

    for (size_t i = 0; i <= cmd_list->n; i++)
        if (i == cmd_list->n && last_command != 0) {
            exit_code = exec_cmd(last_command, in_redirect, out_redirect);
            if (WEXITSTATUS(exit_code) == SHELL_QUIT) exit(EXIT_SUCCESS);
            in_redirect.do_redirection = out_redirect.do_redirection = false;
        }
        else if (isPipe(cmd_list->cmd[i])) {
            do_pipe = true;
            out_redirect.do_redirection = true;
            char *pathname = make_temp();
            strcpy(out_redirect.pathname, pathname);
            exit_code = exec_cmd(last_command, in_redirect, out_redirect);
            if (WEXITSTATUS(exit_code) == SHELL_QUIT) exit(EXIT_SUCCESS);
            in_redirect.do_redirection = out_redirect.do_redirection = false;
            last_command = 0;
            // for the next command
            out_redirect.do_redirection = false;
            in_redirect.do_redirection = true;
            strcpy(in_redirect.pathname, pathname);
        }
        else if (isInput(cmd_list->cmd[i])) {
            in_redirect.do_redirection = 1;
            do_redirect = 1; //  input redirect
        }
        else if (isOutput(cmd_list->cmd[i])) {
            out_redirect.do_redirection = 1;
            do_redirect = 2;
        }
        else if (isAppend(cmd_list->cmd[i])) {
            out_redirect.do_redirection = 2;
            do_redirect = 2;
        }
        else if (isAnd(cmd_list->cmd[i])) {
            exit_code = exec_cmd(last_command, in_redirect, out_redirect);
            if (WEXITSTATUS(exit_code) == SHELL_QUIT) exit(EXIT_SUCCESS);
            if (exit_code) {
                break;
            }
        }
        else if (isOR(cmd_list->cmd[i])) {
            exit_code = exec_cmd(last_command, in_redirect, out_redirect);
            if (WEXITSTATUS(exit_code) == SHELL_QUIT) exit(EXIT_SUCCESS);
            in_redirect.do_redirection = out_redirect.do_redirection = false;
        }
        else if (isBackground(cmd_list->cmd[i])) {
            background = true;
            exec_cmd(last_command, in_redirect, out_redirect);
            in_redirect.do_redirection = out_redirect.do_redirection = false;
            last_command = 0;
            background = false;
        }
        else {
            if (do_pipe) {
                last_command = cmd_list->cmd[i];
//                in_redirect.do_redirection = true;
//                strcpy(in_redirect.pathname, "");
                do_pipe = false;
            }
            else if (do_redirect == 1) {
                strcpy(in_redirect.pathname, cmd_list->cmd[i]);
                do_redirect = 0;
            }
            else if (do_redirect == 2) {
                strcpy(out_redirect.pathname, cmd_list->cmd[i]);
                do_redirect = 0;
            }
            else {
                last_command = cmd_list->cmd[i];
            }

        }
}

//初始化
void init(bool redire) {
    while (true) {
        if (!redire) set_title();
        char *line;
        size_t n = MAX_LINE;
        line = malloc(n);
        getline(&line, &n, stdin);
        // share the same space
        cmd_t *cmd_list = mmap(NULL, sizeof(cmd_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        pid_t pid = fork();

        if (pid == 0) {
            string_explode(line, cmd_list);
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0) {
            int status;

            if (waitpid(pid, &status, 0) != pid) {
                err("wait()");
            }
            else {
                /* did the child terminate normally? */
                if (WIFEXITED(status)) {
                    size_t i;
                    exec_line(cmd_list);
                }
                    /* was the child terminated by a signal? */
                else if (WIFSIGNALED(status)) {
                    printf("%ld terminated because it didn't catch signal number %d\n",
                           (long) pid, WTERMSIG(status));
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }

}


int main() {
// 共享内存
    job =mmap(NULL, sizeof(job_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < 80; i++) {
        job->jobs[i] = 0;
    }

//    进入 shell 的初始目录设定为 主目录
    char * home_dir = getenv("HOME");
    change_dir(home_dir);

    init(false);
    return 0;
}