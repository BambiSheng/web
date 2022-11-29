#include<stdio.h>
#include<winsock2.h>
#include<stdlib.h>
#pragma comment(lib, "ws2_32.lib")

void error_die(const char*str){
  perror(str);
  exit(1);
}

void printf_str(const char* str){
  printf("[%s - %d]%s",__func__,__LINE__,str);
}

int startup(unsigned short * port){
  //wsastartup
  WSADATA data;
  int ret = WSAStartup(
    MAKEWORD(1,1),
    &data);
  if(ret){
    error_die("WSAStarup");
  }
  //socket
  int server_socket= socket(PF_INET,
  SOCK_STREAM,
  IPPROTO_TCP);
  if(server_socket ==-1){
    error_die("socket");
  }
  //reuse
  int opt=-1;
  ret= setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR, (const char*)&opt,sizeof(opt));
  if(ret==-1)
  {
    error_die("setsockopt");
  }

  //server address
  struct sockaddr_in server_addr;
  memset(&server_addr,0,sizeof(server_addr));
  server_addr.sin_family = PF_INET;
  server_addr.sin_port = htons(*port);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  //bind
  ret = bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr));
  if(ret<0)
  {
    error_die("bind");
  }

  //dynamic ports
  int nameLen = sizeof(server_addr);
  if(*port == 0){
      int ret = getsockname(server_socket,
      (struct sockaddr*)&server_addr,
      &nameLen);
      if(ret < 0)
      {
        error_die("getsockname");
      }
     *port = server_addr.sin_port;
  }


  //list
  ret = listen(server_socket, 5);
  if(ret < 0)
  {
    error_die("listen");
  }

  return server_socket;
}
int get_line(int sock, char* buff,int size){
  char c=0;
  int i =0;
  while(i<size-1 && c!='\n')
  {
    int n =  recv(sock,&c,1,0);
    if(n>0){
      if (c=='\r'){
        n = recv(sock,&c,1,MSG_PEEK);
        if( n>0 && c == '\n'){
          recv(sock , &c, 1,0);
        }
        else{
          c='\n';
        }
      }
      buff[i] = c;
      i++;
    }
    else{
      c='\n';
    }
  }
  buff[i] = 0;
  return i;
}
DWORD WINAPI accept_request(LPVOID arg){
  char buff[1024];
  int client = (SOCKET)arg;
  //read
  int numberchars = get_line(client, buff, sizeof(buff));
  printf_str(buff);
  


  return 0;
}

int main(void){
  unsigned short port=8000;
  int server_sock = startup(&port);
  printf("%hu loading...\n",port);

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  while(1){
      //waiting
      int client_sock = accept(server_sock, (struct sockaddr*)&client_addr,&client_addr_len);
      if (client_sock == -1){
        error_die("socket accept");
      }
      DWORD threadid = 0;
      CreateThread(0, 0, accept_request,(void*)client_sock,0, &threadid);
  }


  closesocket(server_sock);
  return 0;
}