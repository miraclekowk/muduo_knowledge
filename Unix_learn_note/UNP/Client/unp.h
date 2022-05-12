//
// Created by nins on 2022/5/12.
//

#ifndef CLIENT_UNP_H
#define CLIENT_UNP_H

#include <cstddef>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <cstring>


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


static int read_cnt;
static char* read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd,char* ptr){
    if(read_cnt <= 0){
        again:
            if( (read_cnt = read(fd,read_buf,sizeof(read_buf))) < 0){
                if(errno == EINTR){
                    goto again; //处理中断
                }else{
                    return -1;
                }
            }else if(read_cnt == 0){
                return 0;
            }
            read_ptr = read_buf;
    }

    read_cnt--; //readline中输入一个到结果中，这里就要减少一个
    *ptr = *read_ptr++;
    return 1;
}

ssize_t readline(int fd,void* vptr,size_t maxlen){
    ssize_t n,rc;
    char c,*ptr;

    ptr = (char*)vptr;
    for(n = 1;n < maxlen;n++){
        if( (rc = my_read(fd,&c)) == 1) {
            *ptr++ = c; //顺序读取，赋值
            if (c == '\n')
                break;
        }else if(rc == 0){
            *ptr = 0;
            return (n-1);
        }else{
            return -1;
        }
    }


    *ptr = 0;
    return n;
}

ssize_t readlinebuf(void **vptrptr){
    if(read_cnt){
        *vptrptr = read_ptr;
    }
    return read_cnt;
}



#endif //CLIENT_UNP_H
