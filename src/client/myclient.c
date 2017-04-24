#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAXLINE 4096

typedef struct{
    char **IPs;
    int *ports;
    int num;
} serverinfo_t;

typedef struct 
{
    char *filename;
    int startbyte;
    int size;
    int id;
} threadinfo_t;

serverinfo_t *serverinfo;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void readServerFile(char** argv);
int getfilesize(char** argv);
void *downloadfunction(void *threadInfo); //thread function

int getfilesize(char** argv){
    
    int sockfd = 0,filesize=0,n;
    char recvline[MAXLINE+1],buf[MAXLINE],recv;
    char flag;
    struct sockaddr_in serv_addr;

    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
  

    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverinfo->ports[0]); // port
    serv_addr.sin_addr.s_addr = inet_addr(serverinfo->IPs[0]);

     /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
     int received_int=0;


    write(sockfd, argv[3],strlen(argv[3]));


    if((read(sockfd,&flag,1))>0){
       if(strcmp(&flag,"1")==0){
            printf("no such file\n");
       }else{
            printf("file open succeed\n");
       }

    }
    write(sockfd,"0",1); //send getfilesize or readfile flag
    read(sockfd,&received_int,sizeof(received_int));
    filesize=ntohl(received_int);
    
    return filesize;

}



void readServerFile(char** argv){//read server information from servrer-infor file
    int servernum=atoi(argv[2]);
    FILE *readfp;
    int lines_allocated =128;
    int max_line_len = 100;
    /*allocate lines of text*/
    char **words =(char**)malloc(sizeof(char*)*lines_allocated);
    if(words==NULL){
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }
    readfp = fopen(argv[1],"r");
    if(readfp ==NULL){
        fprintf(stderr, "Error opening file");
        exit(2);
    }
    int count=0;
    for (int i=0;1;i++){
        /* Allocate space for the next line */
        words[i] = malloc(max_line_len);
        if (words[i]==NULL){
            fprintf(stderr,"Out of memory (3).\n");
            exit(3);
        }
        if (fgets(words[i],max_line_len-1,readfp)==NULL)
            break;

        /* Get rid of CR or LF at end of line */
        for (int j=strlen(words[i])-1;j>=0 && (words[i][j]=='\n' || words[i][j]=='\r');j--)
            words[i][j+1]='\0';
        count++;//check how many servers in <server_info.text>
    }
    /* Close file */
    fclose(readfp);

    if(servernum>count) servernum =count; //If there are less number of server in <server_info.text>, then use the less number

    serverinfo = calloc(1,sizeof(serverinfo_t));
    if(serverinfo==NULL){
        fprintf(stderr,"Out of memory (4).\n");
        exit(4);
    }
    serverinfo->IPs=(char**)malloc(sizeof(char*)*30);
    if(serverinfo->IPs==NULL){
        fprintf(stderr,"Out of memory (5).\n");
        exit(5);
    }
    serverinfo->ports=malloc(sizeof(int)*20);
    if(serverinfo->ports==NULL){
        fprintf(stderr,"Out of memory (6).\n");
        exit(6);
    }
    serverinfo->num=servernum;
    for(int i=0;i<servernum;i++){
        serverinfo->IPs[i]=malloc(50);
        if(serverinfo->IPs[i]==NULL){
        fprintf(stderr,"Out of memory (7).\n");
        exit(7);
    }
        serverinfo->IPs[i]=strtok(words[i]," ");
        serverinfo->ports[i]=atoi(strtok(NULL," "));
        
    }
    

}


void *downloadfunction(void *threadInfo){//thread function
    threadinfo_t *info = (threadinfo_t *) threadInfo;
    int id =info->id;
    char recvline[MAXLINE+1],buf[MAXLINE],flag;
    int sockfd = 0,filesize=0,n;
    struct sockaddr_in serv_addr;
    int received_int=0;
    int bytesReceived=0;
    char *data_string = calloc(info->size, 1);
    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        exit(0);
    }
  
    
    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverinfo->ports[id]); // port
    serv_addr.sin_addr.s_addr = inet_addr(serverinfo->IPs[id]);

     /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
    
   
    
    if((write(sockfd, info->filename,strlen(info->filename)))==-1){
        printf("error sending data");
        exit(0);
    }
     if((read(sockfd,&flag,1))>0){
       if(strcmp(&flag,"1")==0){
            printf("no such file\n");
       }else{
            printf("file open succeed\n");
       }

    }
    write(sockfd,"1",1);
    int temp=htonl(info->startbyte);

    if((write(sockfd,&temp,sizeof(temp)))==-1){  //send download start byte
        printf("error sending data");
        exit(0);
    }

    temp =htonl(info->size);

    if((write(sockfd,&temp,sizeof(temp)))==-1){  //send download size
        printf("error sending data");
        exit(0);
    }

    
    while((bytesReceived = read(sockfd, data_string, MAXLINE)) > 0)
    {
        printf("Thread %d Bytes received %d\n",id,bytesReceived); 
       // strcpy(data_string,recvline);      
    }
    printf("%s\n",data_string);
   //  FILE *Writefp;
   //  FILE *Writefp;
   //  Writefp=fopen("sample_file.txt","w");

   //  fwrite(recvline,1,strlen(recvline),Writefp);  //write byte to file
    
 
    
   // fclose(Writefp);

    printf("thread %d done\n",info->id);
    pthread_exit(0);
}


int main(int argc, char **argv)
{
    int filesize=0;
    if (argc != 4){
        printf("usage: a.out <server_info.text> <num-connections> <filename>"); 
        exit(1);
    }
    

    readServerFile(argv);
    
    filesize = getfilesize(argv);
    printf("File size :%d\n",filesize);
    pthread_t tid[serverinfo->num];
    threadinfo_t *threadinfo[serverinfo->num];
    void *vptr_return[serverinfo->num];

    int i=0;
    for(int i =0;i<serverinfo->num;i++){
        threadinfo[i]=(threadinfo_t *) malloc(sizeof(threadinfo_t));
        threadinfo[i]->filename = argv[3];
        threadinfo[i]->id = i;

    //the last server should take care of the rest of the file content
        if(i==(serverinfo->num-1)){
            threadinfo[i]->startbyte = i * (filesize/serverinfo->num);
            threadinfo[i]->size = (filesize / serverinfo->num) 
                               + (filesize - (filesize / serverinfo->num) * serverinfo->num);
        }
        else{
            threadinfo[i]->startbyte = i*(filesize/serverinfo->num);
            threadinfo[i]->size = filesize / serverinfo->num;
        }
        if(pthread_create(&tid[i],NULL, downloadfunction, (void* ) threadinfo[i])!= 0){
            printf("error creating thread number: %d", i);
            exit(1);
        }

    }


    for(i=0;i<serverinfo->num;i++){
        pthread_join(tid[i],NULL);
    }

   

    for (i = 0; i < serverinfo->num; i++) {
       
        free(threadinfo[i]);
    }
    for(int i=0;i<atoi(argv[2]);i++){
        free(serverinfo->IPs[i]);
    }
    free(serverinfo->IPs);
    free(serverinfo->ports);
    free(serverinfo);
    return 0;
}