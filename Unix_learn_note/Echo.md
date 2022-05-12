### target:

熟悉服务端被动接收新连接，收发数据，被动处理连接断开。

### 调试：

```c++
netstat -a|grep 6670
tcpdump port 6670
```

#### Debug时的一些个人总结：

-在屏蔽信号前，出现SIGPIPE很可能是连接没连上。这时与其去用tcpdump查看，还不如在连接关键处打输出来调试。

- htos很重要，在addr.sin_port的赋值的时候不能忘，不然连不上。
- C++的string类的c_str()，在涉及C语言指针移动的时候，会有难以预料的问题。因此目前个人认为在涉及字符串处理的时候，底层用C风格语言处理的时候，外层函数最好不要用string混用。https://www.cplusplus.com/reference/string/string/c_str/ 中提到：“The pointer returned may be invalidated by further calls to other member functions that modify the object.”
- getline(os,str)函数，得到一个string，这里的string和普通的str一样，末尾是不含有'\0'的。这是因为string类中含有一个length成员(我们写过)，可以通过这个量确定指针末尾，故不需要有结束符。

### 代码：

[代码连接](UNP)

**unp.h**

```c++
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
```

**Server-main.cpp**

```c++
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include "unp.h"

using namespace std;

#define SERV_PORT 6670
#define LISTENQ 1024 //listen's backlog

void str_echo(int sockfd){
    ssize_t n;
    char buf[MAXLINE];

    again:
    while( (n = read(sockfd,buf,MAXLINE)) > 0){
        cout<<"server receive "<<n<<" chars"<<endl;
        writen(sockfd,buf,n);
    }
    
    if(n < 0 && errno == EINTR){
        goto again;
    }else if(n < 0){
        err_sys("str_echo:read error");
    }

}

int main() {
    pid_t childpid;
    socklen_t client_len;
    struct sockaddr_in client_addr,servaddr;

    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    cout<<"listenfd is "<<listenfd<<endl;
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons((unsigned short )SERV_PORT);
    
    bind(listenfd,(const struct sockaddr*)&servaddr,sizeof(servaddr));
    if (listen(listenfd,LISTENQ) == -1)
        cout<<"listen fail"<<endl;
    cout<<"listen finished... server is running..."<<endl;
    
    while(1){
        client_len = sizeof(client_addr);
        //accept是值-结果型函数，内核要返回传入参数的，所以参数三传入一个int指针
        int accpet_fd = accept(listenfd,(struct sockaddr*)&client_addr,&client_len);
        cout<<"now accpet a fd "<<accpet_fd<<endl;
    
        if( (childpid = fork()) == 0){  //进入子进程
            close(listenfd);
            str_echo(accpet_fd);
    
            cout<<"child pid is "<<childpid<<endl;
            exit(0);
        }
    
        close(accpet_fd);
    }
    return 0;

}
```

**Client-main.cpp**

```c++
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <stdio.h>

#include "unp.h"

using namespace std;

#define SERV_PORT 6670
#define SERV_IP "127.0.0.1"


void str_cli(FILE *fp,int sockfd){
    //string line;
    char sendline[MAXLINE],recvline[MAXLINE];
    while(fgets(sendline,MAXLINE,fp)){
        //把stdcin读到的line写到acceptfd中
        writen(sockfd, sendline, strlen(sendline));

        if(readline(sockfd,recvline,MAXLINE) == 0){
            err_sys("readline fail");
        }
    
        fputs(recvline,stdout);
    }

}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,SERV_IP,&servaddr.sin_addr);
    
    connect(sockfd,(sockaddr*)&servaddr,sizeof(servaddr));
    cout<<"Client connected..."<<endl;
    
    str_cli(stdin,sockfd);
    
    return 0;

}
```



