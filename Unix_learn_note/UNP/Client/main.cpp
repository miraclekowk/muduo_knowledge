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
    struct sockaddr_in cliaddr;


    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&cliaddr,sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,SERV_IP,&cliaddr.sin_addr);

    connect(sockfd,(sockaddr*)&cliaddr,sizeof(cliaddr));
    cout<<"Client connected..."<<endl;

    str_cli(stdin,sockfd);


    return 0;
}
