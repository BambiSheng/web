#include<stdio.h>
float * search(float score[][4],int n){
  float *pt;
  pt=*(score+n-1);
  return pt;
}
int main(){  
  while(1){
    int n;
    scanf("%d",&n);
    float score[][4]={{1,1,1,1},{2,2,2,2},{3,3,3,3}};
    float *p;
    p=search(score,n);
    for(int i=0;i<=3;i++){
      printf("%f ",*(p+i));      
    }
  }
}