
// 简单的  hello server, 演示 socket 的联机
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[]){

    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char message[] = "hello world";

    if (argc !=2 )
    {
       printf("Usage: %s <port>\n", argv[0]);
       exit(1);
    }

    // 第一步，调用 socket 函数创建套接字
    serv_sock = socket(PF_INET,SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1])); // hton 中的 s 指的是 short,htonl 的 l 指的是 long

    // 第二步， 调用 bind 函数分配 IP 地址和端口号
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
       error_handling("bind() error");
    }
    
    // 第三步，调用 listen 函数转为可接收请求状态
    if (listen(serv_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    clnt_addr_size = sizeof(clnt_addr);
    // 第四步，调用 accept 函数受理连接请求；
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
    {
        error_handling("accept() error");
    }

    // 第五步， 数据交换
    write(clnt_sock, message, sizeof(message)); // 向文件描述符 clnt_sock 写入信息
    // 第六步，断开连接
    close(clnt_sock); // 关闭
    close(serv_sock);

    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}