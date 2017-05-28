//
// Created by Administrator on 2017/5/13.
//

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/wait.h>
#include "log.h"
#include "../constant.h"

static void cleanZombieProcess(char *nowProcessName);

int main(int argc, char *argv[]) {
    // 参数个数检查
    if (argc < 13) {
        LOGE(MY_NATIVE_TAG, "argc number is %d, expected 13!", argc);
        return 0;
    }
    char *processName = argv[0];
    char *packageName = NULL;
    char *wakeUpClassName = NULL;
    int pipe1Fd[2];
    int pipe2Fd[2];
    for (int i = 0; i < argc; i++) {
        if (!strcmp(PACKAGE_NAME, argv[i])) {
            packageName = argv[i + 1];
            LOGD(MY_NATIVE_TAG, "packageName is : %s", packageName);
        }
        if (!strcmp(SERVICE_CLASS_NAME, argv[i])) {
            wakeUpClassName = argv[i + 1];
            LOGD(MY_NATIVE_TAG, "wakeUpClassName is : %s", wakeUpClassName);
        }
        if (!strcmp(PIPE_1_READ, argv[i])) {
            pipe1Fd[0] = atoi(argv[i + 1]);
            LOGD(MY_NATIVE_TAG, "pipe1r is : %d", pipe1Fd[0]);
        }
        if (!strcmp(PIPE_1_WRITE, argv[i])) {
            pipe1Fd[1] = atoi(argv[i + 1]);
            LOGD(MY_NATIVE_TAG, "pipe1w is : %d", pipe1Fd[1]);
        }
        if (!strcmp(PIPE_2_READ, argv[i])) {
            pipe2Fd[0] = atoi(argv[i + 1]);
            LOGD(MY_NATIVE_TAG, "pipe2r is : %d", pipe2Fd[0]);
        }
        if (!strcmp(PIPE_2_WRITE, argv[i])) {
            pipe2Fd[1] = atoi(argv[i + 1]);
            LOGD(MY_NATIVE_TAG, "pipe2w is : %d", pipe2Fd[1]);
        }
    }

    // 成为守护进程
    int forkResult = fork();
    if (forkResult == 0) {
        // 成为会话组长
        setsid();
        // 改变工作目录
        chdir("/");

        // 清理僵尸进程
        cleanZombieProcess(processName);

        // 管道处理
        // 关闭pipe1的读端
        close(pipe1Fd[0]);
        // 关闭pipe2的写端
        close(pipe2Fd[1]);

        // 管道监听，监听父进程情况
        char readBuffer[100];
        int readResult = read(pipe2Fd[0], readBuffer, 100);
        LOGD(MY_NATIVE_TAG, "readResult is %d, errno is %d", readResult, errno);
        // 阻塞中断，父进程已退出
        LOGD(MY_NATIVE_TAG, "parent process is dead");
        // 拉活处理
        pid_t childPid = fork();
        if (childPid == 0) {
            // 唤醒广播拉活
            char *wakeUpName = new char[strlen(packageName) + strlen(wakeUpClassName) + 1];
            sprintf(wakeUpName, "%s/.%s", packageName, wakeUpClassName);
            LOGD(MY_NATIVE_TAG, "wakeUpName is %s", wakeUpName);
            int result = execlp("am", "am", "broadcast",
                                "--user", "0", "-n", wakeUpName, (char *) NULL);
            LOGD(MY_NATIVE_TAG, "execute am broadcast result is %d", result);
        } else if (childPid > 0) {
            waitpid(childPid, NULL, 0);
            LOGD(MY_NATIVE_TAG, "execute am broadcast over");
        } else {
            LOGE(MY_NATIVE_TAG, "fork fail!");
        }
        return 0;
    } else if (forkResult > 0) {
        return 0;
    } else {
        LOGE(MY_NATIVE_TAG, "fork fail!");
    }
}

static void cleanZombieProcess(char *nowProcessName) {
    pid_t nowPid = getpid();
    DIR *dir;
    dirent *next;

    dir = opendir("/proc");
    // 打开失败
    if (!dir) {
        return;
    }

    while ((next = readdir(dir)) != NULL) {
        FILE *cmdlineFile;
        char procFileName[BUFFER_SIZE];
        char fileBuffer[BUFFER_SIZE];
        char processName[BUFFER_SIZE];
        pid_t pid;

        // 排除..文件
        if (!strcmp(next->d_name, "..")) {
            continue;
        }

        // 进程目录为数字
        if (!isdigit(*next->d_name)) {
            continue;
        }
        pid = atoi(next->d_name);

        // 打开cmdline文件，查看执行命令
        sprintf(procFileName, "/proc/%s/cmdline", next->d_name);
        // 打开失败
        if (!(cmdlineFile = fopen(procFileName, "r"))) {
            continue;
        }
        // 获取文件内容
        if (fgets(fileBuffer, BUFFER_SIZE - 1, cmdlineFile) == NULL) {
            fclose(cmdlineFile);
            continue;
        }
        fclose(cmdlineFile);

        // 获取执行命令（进程名字）
        sscanf(fileBuffer, "%[^-]", processName);
        // 进程名称相同 且 进程id不是当前进程
        if (!strcmp(processName, nowProcessName) && pid != nowPid) {
            LOGD(MY_NATIVE_TAG, "find zombie process, id is : %d, cmdline is : %s",
                 pid, processName);
            // 清除僵尸进程
            int result = kill(pid, SIGTERM);
            if (!result) {
                LOGD(MY_NATIVE_TAG, "kill zombie process %d success!", pid);
            } else {
                LOGD(MY_NATIVE_TAG, "kill zombie process %d fail!", pid);
                exit(0);
            }
        }
    }

    // 关闭目录
    closedir(dir);
}