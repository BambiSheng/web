#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXLEN 1024
#define MAXTHR 10

const int port=8888;
const char *ip="127.0.0.1";

typedef struct mysocketinfo{
    int socketcon;
    char *ipaddr;
    uint16_t port;
}_mysocketinfo;

pthread_t arrthrreceiveclient[10];
struct mysocketinfo arrconsocket[10];
int arr=0;

int checkthriskill(pthread_t thr)
{
       //1存在,0不存在
        int res=1;
        //判断线程是否存在
        int kill_rc=pthread_kill(thr,0);

        if(kill_rc==3)
            printf("指定的线程不存在或已退出\n");
        else if(kill_rc==2)
            printf("信号无效\n");
        else
            printf("指定线程是活着的\n");

        if(kill_rc==0)
        {
        res=0;
        }
        return res;
}

//接收消息函数
void *fun_thrreceivehandler(void *socketcon){
    char buffer[MAXLEN];
    int buffer_length;
    int socketcon1;


    socketcon1=*((int*)socketcon);
    while(1){
        memset(buffer,'\0',sizeof(buffer));//或者使用函数bzero(buffer,20);

        printf("接收套接字：%d\n",socketcon1);
        buffer_length=read(socketcon1,buffer,MAXLEN-1);
        if(buffer_length<0)
        {
            printf("接收客户端失败了\n");
        }
        else if(buffer==NULL)
        {
            pthread_exit("线程退出");   
        }
        if(strcmp(buffer,"quit")==0)
        {
            int i=0;
            int k=0;

            fprintf(stdout,"套接字:%d close",socketcon1);
            for(i=0;i<=arr;i++)
            {
                if(arrconsocket[i]==socketcon1)
                {
                    for(k=i;k<=arr;k++)
                    {
                        arrconsocket[k]=arrconsocket[k+1];
                        arrthrreceiveclient[k]=arrthrreceiveclient[k+1];
                    }
                }
            }

        }
        printf("buffer:%s\n",buffer);
        buffer[buffer_length]='\0';
        printf("客户端%d:%s\n",socketcon1,buffer);
        sleep(1);
    }
     printf("接受数据线程结束\n");
}

//删除杀死的线程
int delete_client(void *fp,int num)
{
    /*
    int pthreadnum=0;
    int thrreceiveclientcount=0;//接收消息的线程ID
    int conclientcount=0;//在线人数
    */

    int i=0;

    pthread_t *ps=(pthread_t *)fp;

    if(num<1)
    {
        return 0;   
    }

    for(i=num;i<=arr;i++)
    {
        ps[i]=ps[i+1];  
    arrconsocket[i]=arrconsocket[i+1];
    }

    return 1;
}

//处理acceot
void *fun_thraccepthander(void *socketlisten){
    char buf[MAXLEN];
    ssize_t size;
    int sockaddr_in_size=sizeof(struct sockaddr_in);//sockaddr_in结构体的大小
    struct sockaddr_in client_addr;
    int socklisten1=*((int*)socketlisten);
    int socketcon;

    while(1){
    socketcon=accept(socklisten1,(struct sockaddr*)(&client_addr),(socklen_t *)(&sockaddr_in_size));
    if(socketcon<0)
    {
        perror("accept error\n");
        exit(5);
    }   
    else{
        printf("accept success\n");
            printf("ip:%s,port:%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    }


    printf("在线人数:%d\n",++arr);
        printf("连接套接字:%d\n",socketcon);

    arrconsocket[arr].socketcon=socketcon;
    arrconsocket[arr].ipaddr=inet_ntoa(client_addr.sin_addr);
    arrconsocket[arr].port=client_addr.sin_port;

    //接收的消息
    pthread_t thrreceive=0;
    if(pthread_create(&thrreceive,NULL,fun_thrreceivehandler,&socketcon)!=0)
    {
        perror("pthread_create error\n");
        exit(6);
    }
    arrthrreceiveclient[arr]=thrreceive;
    sleep(1);
    }

}

void *fun_thrfile(void *socketcon)
{
    int _socketcon=*((int *)socketcon);
    int sockfd,addrlen,n;
    struct sockaddr_in addr;
    char buf[MAXLEN];
    int sock_tmp;
    int count=0;
    int k=0;
    int m=0;

    sockfd=socket(AF_INET,SOCK_DGRAM,0);//服务端的数据报
    if(sockfd<0)
    {
        fprintf(stderr, "socket failed\n");  
            exit(EXIT_FAILURE);
    }   

    addrlen=sizeof(struct sockaddr_in);
    bzero(&addr,addrlen);
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(9999);

    if(bind(sockfd,(struct sockaddr*)(&addr),sizeof(addr))<0)
    {
        fprintf(stderr, "socket failed\n");  
            exit(EXIT_FAILURE);
    }
    puts("bind success");

    bzero(buf,MAXLEN);
    while(1)
    {
        n=recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)(&addr),&addrlen);
        if(n>0)
        {
            sock_tmp=atoi(buf);
            fprintf(stdout,"sock_tmp:%d",sock_tmp);
        }
        else
        {
            count++;
        }
        if(count==3)
        {
            for(k=0;k<=arr;k++)
            {
                if(arrconsocket[k].socketcon==sock_tmp)
                {
                    for(m=k;m<=arr;m++)
                    {
                        arrconsocket[k]=arrconsocket[k+1];
                        arrthrreceiveclient[k]=arrthrreceiveclient[k+1];
                    }
                    arr-=1;
                    break;
                }
            }
        }           
        sleep(3);
    }
}


int main()
{
    int service_socket=socket(AF_INET,SOCK_STREAM,0);//创建服务端的套接字
    if(service_socket<0)//如果创建套接字失败了
    {
        perror("service create error\n");
        exit(1);
    }

    struct sockaddr_in addr;//套接字地址结构
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);

    if(bind(service_socket,(struct sockaddr*)&addr,sizeof(addr))<0)//连接套接字结构和套接字
    {
        perror("bind error\n");
        exit(2);
    }

    int listen_socket=listen(service_socket,10);//监听套接字
    if(listen_socket<0)//失败的处理
    {
        perror("listen error\n");
        exit(3);
    }

    pthread_t thraccept;//创建的线程数组
    //pthread_create创建线程（实际上就是确定调用该线程函数的入口点），在线程创建以后，就开始运行相关的线程函数
    if(pthread_create(&thraccept,NULL,fun_thraccepthander,&service_socket)!=0)
    {
        perror("pthread_create error\n");
        exit(4);
    }
    sleep(1);
    pthread_t thrfile;
    if(pthread_create(&thrfile,NULL,fun_thrfile,&service_socket)!=0)
    {
        perror("pthread_create error\n");
        exit(4);
    }
    sleep(1);

    while(1)
    {
        int i=1;
        for(i=1;i<=arr;i++)
        {
            if(checkthriskill(arrthrreceiveclient[i])==1)
            {
                printf("have a thread is killed\n");
                if(delete_client((void*)(&arrthrreceiveclient),i)==0)
                {
                    printf("delete_client error\n");
                    exit(6);
                }
                --arr;
            }
        }
        printf("当前接收进程数据线程:%d\n",arr);
        if(arr<=0)
        {
            printf("没有客户端连接\n");
        }
        else
        {
            int i=0;
            char buf[MAXLEN];
            ssize_t size=0;

            memset(buf,'\0',MAXLEN);
            size=read(STDIN_FILENO,buf,sizeof(buf));
            if(size>0)
            {
                buf[size]='\0';
            }
            else
            {
                perror("read error\n");
                break;
            }

            for(i=1;i<=arr;i++)
            {
                int sendmsg_len=write(arrconsocket[i].socketcon,buf,size);
                if(sendmsg_len>0)
                {
                    printf("向客户端%s:%d发送成功\n",arrconsocket[i].ipaddr,arrconsocket[i].port);
                }
                else
                {
                    printf("向客户端%s:%d发送失败\n",arrconsocket[i].ipaddr,arrconsocket[i].port);
                }
            }
        }
        //pthread_join(id1,&a1);
        sleep(1);
    }
    if(arr>=1)
    {
        char *message;
        int j=1;
        int res=1;
        printf("等待线程退出\n");



            if((res=pthread_join(thraccept,(void*)&message))==0)
            {
                printf("%s\n",message);
            }
            else
            {
                printf("pthread_join error:%d\n",res);
            }

    }
    else
    {
        printf("没有线程\n");
    }
    close(service_socket);
    pthread_kill(thraccept,SIGKILL);
    return(0);
}