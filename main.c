#include "main.h"

void clear_screen() {
    printf("\033[2J\033[1;1H");
}

int expand_and_execute(const char *program) {
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
        /* This is the child process.  Execute the command. */
        execvp(result.we_wordv[0], result.we_wordv);
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
        /* The fork failed.  Report failure.  */
        status = -1;
    else
        /* This is the parent process.  Wait for the child to complete.  */
    if (waitpid(pid, &status, 0) != pid)
        status = -1;

    wordfree(&result);
    return status;
}

char *string_explode(char *line) {
    int i = 0;
    if (strlen(line) == 0) return 0;
    for (int len = strlen(line); i < len; i++)
        if (line[i] == '<' || line[i] == '>' || line[i] == '|' || line[i] == '&') {
            char *new = malloc(sizeof(char) * (len - i));
            if (i == 0) {
                exit(10);
            }
            memcpy(new, line, i - 1);
            new[i] = 0;
            memmove(line, line + i + 1, len - i);
            return new;
        }
        else if (line[i] == '\n') {
            char *new = malloc(sizeof(char) * len + 1);
            strcpy(new, line);
            new[i] = 0;
            memmove(line, line + i + 1, len - i);
            return new;
        }
}

void showpwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");
}

void changeDir(char *newDir)
{
    if( chdir(newDir)==0 )
    {
        printf("Directory changed. The present working directory is \"%s\" \"%s\"\n",getenv("PWD"),getcwd(NULL,0));
        setenv("PWD", getcwd(NULL,0), 1);
        printf("Directory changed. The present working directory is \"%s\" \"%s\"\n",getenv("PWD"),getcwd(NULL,0));

    }
    else
    {
        perror("chdir()");
    }
}

void init() {

    while (true) {
        printf("myshell $ ");
        char *line;
        size_t n = MAX_LINE;
        line = malloc(n);
        getline(&line, &n, stdin);
        pid_t pid = fork();
        if (pid == 0) {
            char *cmd[80];
            int i = 0;
            cmd[i] = string_explode(line);
            while (cmd[i] != 0) {
                cmd[++i] = string_explode(line);
            }
            for (int j = 0; j < i; j++)
                printf("%s\n", cmd[j]);
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0) {
            int status;

            printf("Child has pid %ld\n", (long) pid);

            if (wait(&status) == -1) {
                perror("wait()");
            }
            else {
                /* did the child terminate normally? */
                if (WIFEXITED(status)) {
                    printf("%ld exited with return code %d\n",
                           (long) pid, WEXITSTATUS(status));
                }
                    /* was the child terminated by a signal? */
                else if (WIFSIGNALED(status)) {
                    printf("%ld terminated because it didn't catch signal number %d\n",
                           (long) pid, WTERMSIG(status));
                }
            }
        }
    }

}

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


void set_title() {
    char *euname = get_uname();
    if (errno) {
        err("geteuname");
    }
    char *hostname = get_hostname();
    if (errno) {
        err("gethostname_malloc");
    }
    char *cwd = getcwd(NULL,0);
    printf(YEL "%s@%s" RESET BLU " %s " RESET"$", euname, hostname, cwd);
}


int main() {
//    init();
//    wordexp_t result;
//    pid_t pid;
//    int status, i;
//    wordexp("ls -l |", &result, 0);
//    for (i = 0; i < result.we_wordc; i++) {
//        printf("%s", result.we_wordv[i]);
//    }
//    fprintf(stderr, "hellp");
//    changeDir("/Users/longmen");
//    printf(RED "red%s\n" RESET, "string");
//    printf(GRN "green\n" RESET);
//    printf(YEL "yellow\n" RESET);
//    printf(BLU "blue\n" RESET);
//    printf(MAG "magenta\n" RESET);
//    printf(CYN "cyan\n" RESET);
//    printf(WHT "white\n" RESET);
    set_title();
    return 0;
}