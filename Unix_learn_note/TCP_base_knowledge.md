#### TCP选项

-MSS选项。发送SYN的TCP一端使用本选项通告对端它的最大分节大小(maximum segment size)，即在本连接的每个TCP分节中愿意接收的最大数据量。
-窗口规模选项。TCP连接任何一段能够通告对端的最大窗口大小。可以通过SO_RCVBUF改变以增加吞吐量。

#### TIME_WAIT

  大量连接一直处于close_wait状态，啥情况，咋排查。
 TIME_WAIT状态可以通过优化服务器参数得到解决，因为发生TIME_WAIT的情况是服务器自己可控的，要么就是对方连接的异常，要么就是自己没有迅速回收资源，总之不是由于自己程序错误导致的。
        但是CLOSE_WAIT就不一样了，如果一直保持在CLOSE_WAIT状态，那么只有一种情况，就是在对方关闭连接之后服务器程序自己没有进一步发出ack信号。换句话说，就是在对方连接关闭之后，程序里没有检测到，或者程序压根就忘记了这个时候需要关闭连接，于是这个资源就一直被程序占着。个人觉得这种情况，通过服务器内核参数也没办法解决，服务器对于程序抢占的资源没有主动回收的权利，除非终止程序运行。
什么情况下，连接处于CLOSE_WAIT状态呢？
      -在被动关闭连接情况下，在已经接收到FIN，但是还没有发送自己的FIN的时刻，连接处于CLOSE_WAIT状态。通常来讲，CLOSE_WAIT状态的持续时间应该很短，正如SYN_RCVD状态。但是在一些特殊情况下，就会出现连接长时间处于CLOSE_WAIT状态的情况。
      -出现大量close_wait的现象，主要原因是某种情况下对方关闭了socket链接，但是我方忙与读或者写，没有关闭连接。代码需要判断socket，一旦读到0，断开连接，read返回负，检查一下errno，如果不是AGAIN，就断开连接。

#### 端口号

  机器上多个进程可能同时使用多个TCP，UDP，SCTP这3中传输层协议中的任何一种，端口号就是用来区分这些进程的。  端口号不等于fd号。
  端口号可以形成连接必备的四元组，再用bind可以把四元组绑定到结构体上形成一个fd，这就是监听fd。                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     

#### 缓冲区

TCP：拥有一个MSS大小的TCP分节的内核缓冲区。内核缓冲区可以保存一份用户缓冲区的副本，这样如果需要重发，内核态的数据就可以被再次利用上。write()调用后，只能说明写入内核缓冲区成功且发送了，并不保证对端能收到数据。因此write()成功后我们可以再次使用用户态的缓冲区。 只有当收到ACK后，内核态的buf才会丢弃相应数据。

UDP：因为是无连接的，不需要保存数据副本，所以没有一个真正的缓冲区。

### socket套接字编程

#### 套接字接口地址结构 -- sockaddr_in ：

这个结构体主要有sin_family,sin_addr.s_addr，sin_port参数。
形如：

```c++
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
server_addr.sin_port = htons((unsigned short)port);
```

h代表host,n代表network,l代表long，s代表short。
其中htonl:将无符号整数hostlong从主机字节顺序转换为网络字节顺序。  
**htons：**将无符号短整数hostshort从主机字节顺序转换为网络字节顺序。  
INADDR_ANY为通配地址，告诉内核去选择IP地址。

使用时，第二个参数一定要是传指针，要将sockaddr_in结构体强制类型转化，如

```c++
struct sockaddr_in server_addr;
bind(sockfd,(struct sokaddr*)&server_addr,sizeof(server_addr));
```

如果不传指针会报错：warning passing arg 2 of 'bind'from incompatible pointer type.

#### 值-结果参数：

bind,connect,sendto这些函数是将套接字地址从用户态传到内核。
accpet,recvfrom,getsockname是用户态传到内核后，内核还要返回。
因此，后面这种函数的参数被称为值-结果型参数，如accept：

```c++
accept_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
```

我们看到accpet的第二三个参数都是传的地址。这是因为内核返回时需要修改结构大小的结果。

#### 字节操作符：

bzero和memset等价。bcopy与memcpy等价。
b开头的是BSD函数，所有支持socket函数的系统都支持它们，mem是ANSI C函数。
注意一点，bcopy可以处理复制源和复制目标内存重叠的情况，memcpy不行，需要用memmove。

#### 网络字节序 UNP - P67：

上文中套接字接口地址结构中我们提到了htonl系列函数，它们的作用是规范高低字节序。
但如果要使十进制转成二级制，需要用到inet_aton系列函数。如：192.168.44.12 转成二进制。

```c
#include <arpa/inet.h>
//ipv4
int inet_aton(const char* strptr , struct in_addr *addrptr);

//ipv4和ipv6都适用  参数：AF族，需要转换的点分十进制，sockaddr结构体指针
int inet_pton(int family,const char* strptr,void *addrptr);
```

其中，inet_pton成功返回1，输入的不是有效表达式返回0，出错则返回-1.
转化的结果存放在addrptr中。

#### 网络的读和写：

为了防止输入和输出的字节数比请求的少。少的原因是内核中的缓冲区可能到达了极限。

```c++
ssize_t rtk_io_writen(int fd,void* userbuf,size_t n){
    size_t nleft = n;
    ssize_t nwritten;
    char* bufp = (char*) userbuf; //读取用户态Buf的指针

    //还未读完buff就while一直读
    while(nleft > 0){
        //write()的第三个参数是期望count，实际可能小于这个值
        //因为底层物理空间或调用信号被处理器中断都可能导致实际值较小
        if((nwritten = write(fd,bufp,nleft)) <= 0){
            //errnor is the number of last error
            if(errno == EINTR){
                nwritten = 0;
            }else{
                return -1;
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    
    return n; //正常读取到期望的n个字符

}
```

其中EINTR是interrupted function call。我们的读写函数必须有中断处理能力，即遇到中断返回之后继续调用read()或write()读写。要有意识，在IO这种慢操作的时候很可能我们的线程会捕获中断，因此我们需要立刻处理errno是EINTR的情况。 errno在头文件<errno.h>中

### 基本套接字编程API：

#### socket()

```c++
int listenfd = socket(AF_INET,SOCK_STREAM,0);
```

#### accpet() -- 一直阻塞到客户连接到达。

```c++
//modle
int accept(int sockfd, struct sockaddr* cliaddr, socklen_t* addrlen);  
//sample
int accpet_fd = accept(listenfd,(struct sockaddr*)&client_addr,&client_len); 
```

accept是值-结果型函数，内核要返回传入参数的，所以参数3传入一个int指针。

#### connect():

客户端不bind是因为调用connect，内核会确定IP地址并选择一个临时端口。
即把本地一个fd绑定到远端(服务端)的通信协议四元组上。

```c++
connect(int sockfd, const struct sockaddr* servaddr, socklen_t addrlen);
//sample
connect(sockfd,(sockaddr*)&servaddr,sizeof(servaddr));
```

#### bind():

服务端bind:绑定服务端本地协议地址到一个socketfd上。
int bind(int sockfd, const struct sockaddr* myaddr, socklen_t addrlen);
//sample
bind(listenfd,(const struct sockaddr*)&servaddr,sizeof(servaddr));

#### listen():

内核为一个listen_fd维护两个队列。一个是完成三次握手的已完成连接队列，一个是未完成三握的队列。当进程调用accpet()后，已完成队列的队头将返回给进程。如果该队列为空，则进入睡眠，直到TCP在该队列中放入一项请求。详见UNP-P84

```c
int listen(int sockfd, int backlog);
```

其中backlog的含义在4.2BSD中定义是:the maximum length the queue of pending connetctions may grow to(由未处理连接构成的队列可能增长到的最大长度)。这里的队列是指内核为每个listen_fd维护的已连接队列和未连接队列。

#### close():

默认行为是把fd标记成已关闭，然后立即返回到调用进程中。此时这个fd已经不能被write或者read使用了。这个标记结束后，TCP才去四次挥手。

```c
int close(int sockfd);
```

#### 字符串处理函数：

-fget,fputs  C语言输入输出，如：

```c
fgets(sendline,MAXLINE,fp)) ; //fp -> FILE* fp
fputs(recvline,stdout);
```

