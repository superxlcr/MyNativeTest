//
// Created by Administrator on 2017/5/13.
//

#include <jni.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "../daemon/log.h"
#include "../constant.h"

extern "C"
JNIEXPORT void JNICALL Java_com_app_superxlcr_mynativetest_Daemon_start
        (JNIEnv *env, jclass clazz, jstring jPackageName, jstring jWakeUpClassName,
         jstring jBinaryFilePath) {
    // 获取Java参数
    const char *packageName = env->GetStringUTFChars(jPackageName, 0);
    const char *wakeUpClassName = env->GetStringUTFChars(jWakeUpClassName, 0);
    const char *binaryFilePath = env->GetStringUTFChars(jBinaryFilePath, 0);
    LOGD(MY_NATIVE_TAG, "packageName is %s", packageName);
    LOGD(MY_NATIVE_TAG, "wakeUpCLassName is %s", wakeUpClassName);
    LOGD(MY_NATIVE_TAG, "binaryFilePath is %s", binaryFilePath);
    // 建立管道，pipe1用于父进程监听子进程，pipe2用于子进程监听父进程
    int pipe1Fd[2];
    int pipe2Fd[2];
    if (pipe(pipe1Fd) == -1) {
        LOGE(MY_NATIVE_TAG, "create pipe1 error");
    }
    if (pipe(pipe2Fd) == -1) {
        LOGE(MY_NATIVE_TAG, "create pipe2 error");
    }
    // 执行二进制文件
    pid_t result = fork();
    if (result > 0) { // 父进程
        // 关闭pipe1的写端
        close(pipe1Fd[1]);
        // 关闭pipe2的读端
        close(pipe2Fd[0]);
        // 监听子进程情况
        char readBuffer[100];
        int readResult = read(pipe1Fd[0], readBuffer, 100);
        LOGD(MY_NATIVE_TAG, "readResult is %d, errno is %d", readResult, errno);
        // 阻塞中断，子进程已退出
        LOGD(MY_NATIVE_TAG, "child process is dead");
        // 释放Java参数内存
        env->ReleaseStringUTFChars(jPackageName, packageName);
        env->ReleaseStringUTFChars(jWakeUpClassName, wakeUpClassName);
        env->ReleaseStringUTFChars(jBinaryFilePath, binaryFilePath);
        // 拉活处理，回调Java方法
        jmethodID methodID = env->GetStaticMethodID(clazz, "onDaemonDead", "()V");
        env->CallStaticVoidMethod(clazz, methodID, NULL);
        return;
    } else if (result == 0) { // 子进程
        // 管道描述符转为字符串
        char strP1r[10];
        char strP1w[10];
        char strP2r[10];
        char strP2w[10];
        sprintf(strP1r, "%d", pipe1Fd[0]);
        sprintf(strP1w, "%d", pipe1Fd[1]);
        sprintf(strP2r, "%d", pipe2Fd[0]);
        sprintf(strP2w, "%d", pipe2Fd[1]);
        // 执行二进制文件
        LOGD(MY_NATIVE_TAG, "execute binary file");
        LOGD(MY_NATIVE_TAG, "binary file argv ：%s %s %s %s %s %s %s %s %s %s %s %s %s",
             BINARY_FILE_NAME,
             PACKAGE_NAME, packageName,
             SERVICE_CLASS_NAME, wakeUpClassName,
             PIPE_1_READ, strP1r,
             PIPE_1_WRITE, strP1w,
             PIPE_2_READ, strP2r,
             PIPE_2_WRITE, strP2w);
        execlp(binaryFilePath,
               BINARY_FILE_NAME,
               PACKAGE_NAME, packageName,
               SERVICE_CLASS_NAME, wakeUpClassName,
               PIPE_1_READ, strP1r,
               PIPE_1_WRITE, strP1w,
               PIPE_2_READ, strP2r,
               PIPE_2_WRITE, strP2w,
               NULL);
        // 函数返回，执行失败
        LOGE(MY_NATIVE_TAG, "execute binary file fail, errno is %d", errno);
    } else {
        LOGE(MY_NATIVE_TAG, "fork fail!");
    }
    // 释放Java参数内存
    env->ReleaseStringUTFChars(jPackageName, packageName);
    env->ReleaseStringUTFChars(jWakeUpClassName, wakeUpClassName);
    env->ReleaseStringUTFChars(jBinaryFilePath, binaryFilePath);
}

