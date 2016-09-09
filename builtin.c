//
// Created by longmensparkle on 16/8/5.
//

#include "builtin.h"
#include "utility.h"
#include "main.h"

const char *BUILTIN[] = {
        "cd",
        "exit",
        "pwd",
        "time",
        "bg",
        "fg",
        "jobs",
        "shift",
        "clr",
        "dir",
        "echo",
        "environ",
        "help",
        "umask",
        "quit",
        "myshell",
        NULL
};


bool is_builtin(char *argv) {
    for (const char **builtin_i = BUILTIN, *builtin; (builtin = *builtin_i); builtin_i++) {
        if (strcmp(argv, builtin) == 0) {
            return true;
        }
    }
    return false;
}

void clr() {
    printf("\033[2J\033[1;1H");
}

void show_time() {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current local time and date: %s", asctime(timeinfo));
}

void shell_exit() {
    exit(SHELL_QUIT);
}

void shell_quit() {
    shell_exit();
}

void show_environ() {
    extern char **environ;

    int i = 0;
    while (environ[i]) {
        printf("%s\n", environ[i++]);
    }
}

void pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else {
        err("cd");
        exit(SHELL_INTERNAL_ERROR);
    }
}

void change_dir(char *newDir) {
    if (chdir(newDir) == 0) {
        setenv("PWD", getcwd(NULL, 0), 1);
//        printf("Directory changed. The present working directory is \"%s\" \"%s\"\n", getenv("PWD"), getcwd(NULL, 0));
    }
    else {
        err("cd");
        exit(SHELL_INTERNAL_ERROR);
    }
}

void show_dir(char *pathname) {
    DIR *d;
    struct dirent *dir;
    d = opendir(pathname);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    else {
        err("opendir");
    }
}

void exec_builtin_cmd(char **argv) {
    if (strcmp(argv[0], "clr") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "clr: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        clr();
    }
    else if (strcmp(argv[0], "time") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "time: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        show_time();
    }
    else if (strcmp(argv[0], "exit") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "exit: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        shell_exit();
    }
    else if (strcmp(argv[0], "quit") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "quit: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        shell_quit();
    }
    else if (strcmp(argv[0], "pwd") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "pwd: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        pwd();
    }
    else if (strcmp(argv[0], "cd") == 0) {
        if (argv[1] != NULL) {
            if (argv[2] != NULL) {
                fprintf(stderr, "%s: %s", SHELL_NAME, "cd: Invalid argument\n");
                exit(SHELL_PARSE_ERROR);
            }
            change_dir(argv[1]);

        }
        else {
            char *pathname = getenv("HOME");
            change_dir(pathname);
        }
    }
    else if (strcmp(argv[0], "environ") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "environ: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        show_environ();
    }
    else if (strcmp(argv[0], "myshell") == 0) {
        if (argv[1] == NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "myshell: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        else if (argv[2] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "myshell: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        else {
            input_redirection(argv[1]);
            FILE *file = fopen(argv[1], "a");
            if (file == 0) {
                err("fopen");
                exit(SHELL_INTERNAL_ERROR);
            }
            else {
                fprintf(file, "\nexit\n");
                fclose(file);
                init(true);
            }
        }
    }
    else if (strcmp(argv[0], "echo") == 0) {
        int i = 1;
        while (argv[i]) {
            printf("%s ", argv[i++]);
        }
        printf("\n");
    }
    else if (strcmp(argv[0], "dir") == 0) {
        if (argv[1] == NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "myshell: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        else if (argv[2] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "myshell: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        else {
            show_dir(argv[1]);
        }
    }
    else if (strcmp(argv[0], "umask") == 0) {
        if (argv[1] == NULL) {
            mode_t mode = umask(0);
            printf("%03o\n", mode);
            umask(mode);
        }
        else if (argv[2] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "umask: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        else {

//            printf("%o", mode);
        }
    }
    else if (strcmp(argv[0], "jobs") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "environ: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        show_jobs();
    }
    else if (strcmp(argv[0], "fg") == 0) {
        if (argv[1] == NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "fg: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        else if (argv[2] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "fg: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        else {
            int job_no;
            sscanf(argv[1], "%%%d", &job_no);
            fg(job_no);
        }
    }
    else if (strcmp(argv[0], "help") == 0) {
        if (argv[1] != NULL) {
            fprintf(stderr, "%s: %s", SHELL_NAME, "environ: Invalid argument\n");
            exit(SHELL_PARSE_ERROR);
        }
        cmd_t * cmd = malloc(sizeof(cmd_t));
        cmd->n = 3;
        char * temp = make_temp();
        FILE * file = fopen(temp, "w");
        fprintf(file, "# myshell 用户手册\n"
                "\n"
                "（注：手册使用 markdown 语法标注，使用 markdown 编辑器查看效果更好。）\n"
                "\n"
                "myshell 是由汪博欣个人独立完成的一个简单的 Shell 实现。\n"
                "\n"
                "## 命令提示符\n"
                "\n"
                "myShell 的提示字符格式为`用户名 当前工作目录 $ `，并且使用 ANSI 标准转义代码在终端显示颜色和标题。\n"
                "\n"
                "在提示后可以输入命令，按下回车命令执行完成后会重新打印提示并继续接受命令输入。\n"
                "\n"
                "## 命令\n"
                "\n"
                "Shell 命令由以下部分组成：\n"
                "\n"
                "- 命令名称\n"
                "- 零或多个命令参数\n"
                "- 零或多个输入/输出重定向\n"
                "\n"
                "多条命令间可以由以下符号连接：\n"
                "\n"
                "- |：管道\n"
                "- &：后台执行前一个命令，可以出现在复合命令的最后。\n"
                "- ||：仅当前一条命令不正确退出时执行下一条命令。\n"
                "- &&：仅当前一条命令正确退出时执行下一条命令。\n"
                "- ;：按顺序执行命令。\n"
                "\n"
                "符号可以将命令名称、参数等拆分，此时命令中的相关空格是可选的。\n"
                "\n"
                "## 管道\n"
                "\n"
                "管道（|）可以将上一条命令的输出作为下一条命令的输入。\n"
                "\n"
                "例如，`ls|sort`可以将`ls`的结果经过排序后输出。\n"
                "\n"
                "管道原先标准输入和标准输出的重定向会可以被命令中的输入/输出重定向覆盖,例如 `ls | sort > dir.txt`。\n"
                "\n"
                "## 输入/输出重定向\n"
                "\n"
                "输入/输出重定向是将命令对某个文件描述符的输入/输出操作重定向至文件（暂不支持文件描述符）的功能。\n"
                "\n"
                "支持的重定向的格式：\n"
                "\n"
                "programname arg1 arg2 < inputfile > outputfile \n"
                "\n"
                "使用arg1 和arg2 执行程序programname，输入文件流被替换为inputfile，输出文件流被替换为outputfile。\n"
                "\n"
                "如果输出重定向的符号为 >>,则为追加模式，将输出的内容追加在文件的末尾（如果文件不存在则创建）。\n"
                "\n"
                "## 后台执行\n"
                "\n"
                "默认情况下，Shell 将等待当前命令结束之后，再提示用户输入新的命令。通过在命令末尾加上`&`，可以使得 Shell 不等待此命令执行结束，即提示用户输入新的命令。\n"
                "\n"
                "## 内建命令\n"
                "\n"
                "本 Shell 实现了以下内建命令：\n"
                "\n"
                "- cd：改变当前工作目录到指定目录或家目录。\n"
                "- exit：退出 Shell。\n"
                "- pwd：显示当前目录。\n"
                "- time：显示当前时间。\n"
                "- fg：将后台的进程转到前台执行。\n"
                "- jobs：列出后台进行的程序。\n"
                "- clr：清除屏幕。\n"
                "- dir：列出指定目录或当前工作目录。\n"
                "- echo：打印所有命令参数和换行，参数在打印时以空格间隔。\n"
                "- environ：列出所有环境变量。\n"
                "- help：打印帮助。\n"
                "- umask：改变默认文件权限。\n"
                "- quit：退出 Shell。\n"
                "- myshell： 从文件执行 shell 程序。\n"
                "\n"
                "\n"
                "## 安装\n"
                "\n"
                "myshell 在开发时使用 CMake 进行构建，因此构建时可以将其作为依赖安装。\n"
                "\n"
                "使用`cmake . && make`命令进行编译即可。\n"
                "\n"
                "如果没有安装 CMake，也可以使用 myshell 附带的 Makefile。\n"
                "\n"
                "使用`make`命令进行编译即可。");
        fclose(file);
        strcpy(cmd->cmd[0], "cat ");
        strcpy(cmd->cmd[0] + 4, temp);
        strcpy(cmd->cmd[1], "|");
        strcpy(cmd->cmd[2], "more");
        exec_line(cmd);
    }
    exit(EXIT_SUCCESS);
}













