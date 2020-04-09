//
// Created by 谢仲涛 on 2020/4/6.
//

#ifndef OSIER_PROCESS_H
#define OSIER_PROCESS_H


#include <sys/types.h>

class process {
public:
    ~process();
    bool exec(const char * __file, char * const argv []);
    size_t write(const void * __restrict __ptr, size_t __size);
    char * gets(char *data,size_t size);
    void wait();
private:
    pid_t childpid;
    FILE	*read_fp,*write_fp;
};


#endif //OSIER_PROCESS_H
