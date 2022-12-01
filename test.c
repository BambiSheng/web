#include<stdio.h>
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
int main()
{
  while(1){
    char board[99];
    scanf("%s",board);
    judge(board);
    printf("%s",board);
  }
}