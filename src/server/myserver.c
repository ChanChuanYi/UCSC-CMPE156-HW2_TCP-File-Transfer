#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#define BUFFLEN 4096
int fsize(FILE *fp);

int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}

int main(int argc, char **argv)
{
    if (argc != 2){
        printf("usage: a.out <port-num>"); 
        exit(0);
    }
    int listenfd = 0,n,filesize=0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    char filename[BUFFLEN];
    char flag;
    int numrv;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }


    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
        printf("%s\n","connect success");
        int i=0;
       if ( (n = read(connfd, filename, BUFFLEN)) > 0){
			filename[n] = 0;
        }
		  printf("%s\n",filename);
           
        /* Open the file that we wish to transfer */
        FILE *fp = fopen(filename,"rb");
        if(fp==NULL)
        {
            write(connfd,"1",1);
            printf("File open error");
        }
        else{
            write(connfd,"0",1);
        
            if(read(connfd,&flag,1)>0){
                printf("%c\n",flag);
            }
            if(strncmp(&flag,"0",1)==0){
                printf("flag %c received:return file size\n",flag);
            	filesize=fsize(fp);
                int temp=htonl(filesize);
            	write(connfd,&temp,sizeof(temp));//send file size to client
            }
            else if(strncmp(&flag,"1",1)==0){
                printf("flag %c received:return file content",flag);
                int received_int=0;
                read(connfd,&received_int,sizeof(received_int));//receive download start byte
                int offset=0;
                offset=ntohl(received_int);
                offset=offset;
                printf("Offset : %d\n",offset);
                int count=0;
                if(read(connfd,&received_int,sizeof(received_int))>0){//receive download size
                    count=ntohl(received_int);
                }
                printf("Count : %d\n",count);
                fseek(fp,offset,SEEK_SET);  
                char buffer[BUFFLEN];
                int nread = fread(buffer,1,count,fp);
                printf("%d\n",nread);
                send(connfd,buffer,nread,0);
                
                fclose(fp);
            }    
        }
        close(connfd);

       printf("%s\n","connection close");
       
    }


    return 0;
}