#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#define BUFSIZE 50

//为了简化代码，封装的发送数据以及报错函数
void msgsend(char *msg,int clnt_sock){
  int ret = write(clnt_sock, msg, sizeof(msg));
  if (ret < 0){
    printf("Write error ...\n");
  }
}
//这是判断胜负结果的函数
void  judge(char *board){
  char *temp=board+3;
  if(!strcmp(temp,"111")||!strcmp(temp,"222")||!strcmp(temp,"333")||!strcmp(temp,"123")||!strcmp(temp,"132")||!strcmp(temp,"213")||!strcmp(temp,"231")||!strcmp(temp,"312")||!strcmp(temp,"321")){
    board[0]='0';
    board[1]='0';
    board[2]='0';
  }
  else if(!strcmp(temp,"122")||!strcmp(temp,"233")||!strcmp(temp,"311")){
    board[0]='1';
    board[1]='0';
    board[2]='0';
  }
  else if(!strcmp(temp,"212")||!strcmp(temp,"323")||!strcmp(temp,"131")){
    board[0]='0';
    board[1]='1';
    board[2]='0';
  }
  else if(!strcmp(temp,"221")||!strcmp(temp,"332")||!strcmp(temp,"113")){
    board[0]='0';
    board[1]='0';
    board[2]='1';
  }
  else if(!strcmp(temp,"112")||!strcmp(temp,"223")||!strcmp(temp,"331")){
    board[0]='1';
    board[1]='1';
    board[2]='0';
  }
  else if(!strcmp(temp,"121")||!strcmp(temp,"232")||!strcmp(temp,"313")){
    board[0]='1';
    board[1]='0';
    board[2]='1';
  }
  else if(!strcmp(temp,"211")||!strcmp(temp,"322")||!strcmp(temp,"133")){
    board[0]='0';
    board[1]='1';
    board[2]='1';
  }
}

int main(){
  //一些初始化
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.0.24.3");
    serv_addr.sin_port = htons(1100);
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));


    listen(serv_sock, 20);

    //连接三个客户端的套接字，并发送欢迎信息
    struct sockaddr_in clnt_addr_1;
    socklen_t clnt_addr_size_1 = sizeof(clnt_addr_1);
    int clnt_sock_1 = -1;
    do {
         clnt_sock_1 = accept(serv_sock, (struct sockaddr*)&clnt_addr_1, &clnt_addr_size_1);
         if (clnt_sock_1 < 0) {
             printf("Connected Error, re-try to get connected with client ... \n");
         }
    } while(clnt_sock_1 < 0);
    
    
    char cln_1_login_msg[]={"1"};
    msgsend(cln_1_login_msg,clnt_sock_1);
    
    
    struct sockaddr_in clnt_addr_2;
    socklen_t clnt_addr_size_2 = sizeof(clnt_addr_2);
    int clnt_sock_2 = -1;
    do {
         clnt_sock_2 = accept(serv_sock, (struct sockaddr*)&clnt_addr_2, &clnt_addr_size_2);
         if (clnt_sock_2 < 0) {
             printf("Connected Error, re-try to get connected with client ... \n");
         }
    } while(clnt_sock_2 < 0);

    char cln_2_login_msg[]={"2"};
    msgsend(cln_2_login_msg,clnt_sock_2);
    
    struct sockaddr_in clnt_addr_3;
    socklen_t clnt_addr_size_3 = sizeof(clnt_addr_3);
    int clnt_sock_3 = -1;
    do {
         clnt_sock_3 = accept(serv_sock, (struct sockaddr*)&clnt_addr_3, &clnt_addr_size_3);
         if (clnt_sock_3 < 0) {
             printf("Connected Error, re-try to get connected with client ... \n");
         }
    } while(clnt_sock_3 < 0);
    
    //向客户端发送开始信息
     char cln_3_login_msg[]={"3"};
    msgsend(cln_3_login_msg,clnt_sock_3);

    
    char start[]={"start"};
    msgsend(start,clnt_sock_1);
    msgsend(start,clnt_sock_2);
    msgsend(start,clnt_sock_3);
    char board[]={"000000"};
    while(1) {
        

        //接受来自三个客户端的数据

        char strBuffer1[BUFSIZE];
        int dataLength1 = read(clnt_sock_1, strBuffer1, sizeof(strBuffer1)-1);
        if (dataLength1 < 0) {
            printf("Read Error ... \n");
            continue;
        }
        printf("The server has already got the data from player 1: %s\n", strBuffer1);

        char strBuffer2[BUFSIZE];
        int dataLength2 = read(clnt_sock_2, strBuffer2, sizeof(strBuffer2)-1);
        if (dataLength2 < 0) {
            printf("Read Error ... \n");
            continue;
        }
        printf("The server has already got the data from player 2: %s\n", strBuffer2);
        
        char strBuffer3[BUFSIZE];
        int dataLength3 = read(clnt_sock_3, strBuffer3, sizeof(strBuffer3)-1);
        if (dataLength3 < 0) {
            printf("Read Error ... \n");
            continue;
        }
        printf("The server has already got the data from player 3: %s\n", strBuffer3);
        //说明：发回的board共有六个有效字节，前三位存储胜负信息（胜者记为1）；后三位存储选择信息（1/2/3）
        board[3]=strBuffer1[0];
        board[4]=strBuffer2[0];
        board[5]=strBuffer3[0];
        //调用判断胜负的函数
        judge(board);
        //发回数据
        msgsend(board,clnt_sock_1);
        msgsend(board,clnt_sock_2);
        msgsend(board,clnt_sock_3);
        //对缓冲区清零
        memset(board,0,sizeof(board));
        memset(strBuffer1, 0, BUFSIZE);
        memset(strBuffer2, 0, BUFSIZE);
        memset(strBuffer3, 0, BUFSIZE);
    }
    //关闭套接字
    close(clnt_sock_1);
    close(clnt_sock_2);
    close(serv_sock);

    return 0;
}