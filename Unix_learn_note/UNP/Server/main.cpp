#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include "unp.h"

#define SERV_PORT 6670
#define LISTENQ 1024 //listen's backlog

using namespace std;


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
