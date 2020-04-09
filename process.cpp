//
// Created by 谢仲涛 on 2020/4/6.
//

#include <cstdio>
#include <zconf.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdlib>
#include "process.h"

bool process::exec(const char *__file, char * const argv []) {
    int rpipes[2],wpipes[2];
    pid_t pid;

    if (pipe(rpipes) < 0||pipe(wpipes))
        return false;    /* errno set by pipe() */

    if ((pid = fork()) < 0)
        return false;    /* errno set by fork() */
    else if (pid == 0) {                            /* child */
        close(rpipes[0]);
        if (rpipes[1] != STDOUT_FILENO) {
            dup2(rpipes[1], STDOUT_FILENO);
            close(rpipes[1]);
        }
        close(wpipes[1]);
        if (wpipes[0] != STDIN_FILENO) {
            dup2(wpipes[0], STDIN_FILENO);
            close(wpipes[0]);
        }
        execvp(__file, argv);
        _exit(127);
    }
    /* parent */
    close(rpipes[1]);
    if ((read_fp = fdopen(rpipes[0], "r")) == NULL)
        return (NULL);
    close(wpipes[0]);
    if ((write_fp = fdopen(wpipes[1], "w")) == NULL)
        return (NULL);
    childpid = pid;    /* remember child pid for this fd */
    return true;
}

size_t process::write(const void * __restrict __ptr, size_t __size) {

    int size= fwrite(__ptr,1,__size,write_fp);
    fflush(write_fp);
    return size;
}

char *process::gets(char *data, size_t size) {
    return fgets(data,size,read_fp);
}

process::~process() {
    if (childpid){
        kill(childpid,9);
    }
    if (write_fp){
        fclose(write_fp);
    }
    if (read_fp){
        fclose(read_fp);
    }

}

void process::wait() {
    int stat;
    waitpid(childpid,&stat,0);

}

