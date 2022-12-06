// WindowsClient.cpp
 
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib") 
 
#define BUF_SIZE 100

//report是根据服务器返回的数据，报告本轮游戏各个玩家的选择情况与获胜情况
void report(char* board,int* points){
  int draw=1;
  for(int i=0;i<=2;i++){
    
    if(board[i+3]=='1'){
      printf("Player %d:rock\n",i+1);
    }
    if(board[i+3]=='2'){
      printf("Player %d:scissors\n",i+1);
    }
    if(board[i+3]=='3'){
      printf("Player %d:paper\n",i+1);
    }
  }
  for(int i=0;i<=2;i++){
    if(board[i]=='1'){
      (points[i])++;
      draw=0;
      printf("player %d ",i+1);
    } 
  }
  
  if(draw){
    printf("draw!\n");
  }
  else{
  printf("wins!\n");
  }
}

int main(){
    //初始化DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
 
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
 	printf("Try to connect to the server : 43.143.253.223 ...\n");
    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("43.143.253.223");
    sockAddr.sin_port = htons(1100);
    while (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("Connection Error! Now re-try to get connected ... \n");		
	}

    char welcome[BUF_SIZE] = {0};
	  int rec_welcome = recv(sock, welcome, BUF_SIZE, 0);
	 	if (rec_welcome < 0) {
		 	printf("Receive error, PLS check your network ...\n");
		 }
	  printf("Welcome! You are player %s.\n", welcome);   //欢迎界面，根据与服务器连接的先后顺序分别命名为player1，2，3
    if(welcome[0] == '1'){
      printf("Waiting for player 2 & player 3...\n");
    }
    else if (welcome[0] == '2'){
      printf("Waiting for player 3...\n");
    }

    char start[BUF_SIZE] = {0};
	  int rec_start = recv(sock, start, BUF_SIZE, 0);
	 	if (rec_start < 0) {
		 	printf("Receive error, PLS check your network ...\n");
		 }
	  printf("Let's start!\n");
    
    int round=0;
    int points[3]={0,0,0};

    while(1) {
    round++;            //这是记分牌
    printf("Round%d\npoints:\nplayer1: %d\nplayer2: %d\nplayer3: %d\n",round,points[0],points[1],points[2]);
    printf("rock --> 1\nscissors --> 2\npaper --> 3\nyour choice:\n");

    char bufSend[BUF_SIZE] = {0};
    scanf("%s", bufSend);                     
	  while(strcmp(bufSend,"1") && strcmp(bufSend,"2") && strcmp(bufSend,"3")){
    printf("Input error.Enter 1/2/3:\n");                //有检查输入合法性的机制
    scanf("%s", bufSend);
    }
    //向服务器发送数据
    int sendLen = send(sock, bufSend, strlen(bufSend), 0);
	  if (sendLen < 0) {
		printf("Send Error, PLS Check your network ...\n");
		continue;
	  }
    
    printf("Waiting for other players...\n");
    //所有玩家选择情况全部发送完毕后，服务器会发回结果，输赢情况  
    char board[BUF_SIZE] = {0};
	  memset(board,0, sizeof(board));
    int rec_board = recv(sock, board, BUF_SIZE, 0);//接收数据
	 	if (rec_board < 0) {
		 	printf("Receive error, PLS check your network ...\n");
		}

    report(board,points);//向玩家报告结果


		

 	}
    //关闭套接字
    closesocket(sock);
 
    //终止使用 DLL
    WSACleanup();
	printf("Message has been achieved, the Client now gonna cloese");
    system("pause");
    return 0;
}
