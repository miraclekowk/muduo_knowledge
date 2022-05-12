//
// Created by nins on 2022/5/12.
//

#ifndef SERVER_UNP_H
#define SERVER_UNP_H

#include <cstddef>
#include <unistd.h>
#include <errno.h>

#define MAXLINE 3000

void err_sys(const char* x)
{
    perror(x); //打印错误信息
    exit(1); //不成功的exit
}


ssize_t readn(int fd,void* userbuf,size_t n){
    size_t nleft = n;
    ssize_t nread;
    char* ptr;

    ptr = (char*)userbuf;
    while(nleft > 0){
        if( (nread = read(fd,ptr,nleft)) < 0 ){
            //出错的情况下
            if(errno == EINTR){
                nread = 0;  //if interrupted,call read() again.
            }else{
                return -1;
            }
        }else if(nread = 0){
            break;  //没出错且没得读了，跳出
        }

        nleft -= nread;
        ptr += nread;
    }

    return (n-nleft); //return>=0,读到的字节数比请求的少
}

ssize_t writen(int fd,void *userbuf,size_t n){
    size_t nleft = n;
    ssize_t nwrite;
    char* ptr = (char*)userbuf;

    while(nleft > 0){
        if((nwrite = write(fd,ptr,nleft) ) <= 0){
            if(errno == EINTR){
                nwrite = 0; //call write() again;
            }else{
                return -1;
            }
        }

        nleft -= nwrite;
        ptr += nwrite;
    }

    return n;
}



#endif //SERVER_UNP_H
