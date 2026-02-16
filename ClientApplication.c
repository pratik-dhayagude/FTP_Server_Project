/*
    Client apllication
*/
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdbool.h>

int ReadLine(int Sock , char * line,int max)
{
    int i = 0;
    char ch = '\0';
    int n = 0;
    while(i < max-1)
    {
        n = read(Sock,&ch,1);

        if(n <= 0)
        {
            break;
        }
        line[i++] = ch;
        if(ch == '\n')
        {
            break;
        }


    }//End of while
    line[i] = '\0';
    return i;
}
// End of While


///////////////////////////////////////////
//  
// Commandline Argument Application
//  1st Argument: IP address
//  2st Argument: Port number
//  3st Argument: Targeted file
//  4st Argument: new file name
//  
//  ./client 127.0.0.1  9000    Demo.txt A.txt
//   argv[0] argv[1]   argv[2] argv[3]  argv[4] 
//  
//      argc = 5;
///////////////////////////////////////////

int main(int argc , char * argv[])
{
    int Sock = 0;
    int iRet = 0;
    int Port = 0;              // argv [2]

    char * ip = NULL;          //argv[1]
    char * Filename = NULL;    //argv[3]
    char * Outfilename = NULL; //argv[4]

    struct sockaddr_in ServerAddr;

    char headder[64]={'\0'};

    if(argc < 5 || argc > 5)
    {
        printf("Unable to procced as invalid number of arguments\n");
        printf("Please provide below argument:\n");
        printf("1st Argument: IP address\n");
        printf("2st Argument: Port number\n");
        printf("3st Argument: Targeted file\n");
        printf("4st Argument: new file name\n");

        return -1;
    }

    // store command line arguments into variables
    ip = argv[1];
    Port = atoi(argv[2]);
    Filename = argv[3];
    Outfilename = argv[4];

    //////////////////////////////////////////////////
    // Step 1 : Create a TCP socket
    //////////////////////////////////////////////////

    Sock = socket(AF_INET,SOCK_STREAM,0);

    if(Sock<0)
    {
        printf("Unable to create the client socket\n");
        return -1;
    }
    //////////////////////////////////////////////////
    // Step 2 : Connect with server
    //////////////////////////////////////////////////

    memset(&ServerAddr,0,sizeof(ServerAddr));
    
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);

    //Convert the ip address into binary format

    inet_pton(AF_INET,ip,&ServerAddr.sin_addr);

    iRet = connect(Sock,(struct sockaddr*)&ServerAddr,sizeof(ServerAddr));

    if(iRet == -1)
    {
        printf("Unable t connect\n");
        close(Sock);

        return -1;
    }

    //////////////////////////////////////////////////
    // Step 3 : send file name
    //////////////////////////////////////////////////
    write(Sock,Filename,strlen(Filename));
    write(Sock,"\n",1);

    //////////////////////////////////////////////////
    // Step 4 : read the header
    //////////////////////////////////////////////////

    iRet = ReadLine(Sock,headder,sizeof(headder));

    if(iRet <= 0)
    {
        printf("Server gets disconnected abnormally\n");
        close(Sock);
        return -1;
    }
    long FileSize = 0;

    sscanf(headder,"OK %ld",&FileSize);

    printf("File size is :%ld\n",FileSize);

    //////////////////////////////////////////////////
    // Step 5 : create new file
    //////////////////////////////////////////////////

    int outfd = 0;

    outfd = open(Outfilename,O_CREAT|O_WRONLY|O_TRUNC,0777);
    if(outfd<0)
    {
        printf("Unable to create down file\n");
        return -1;
    }

    char Buffer[1024]={'\0'};

    long recived = 0;
    long remaining = 0;
    int n= 0;
    int toRead = 0;

    while(recived < FileSize)
    {
        remaining = FileSize - recived;
        if(remaining > 1024)
        {
            toRead = 1024;

        }
        else
        {
            toRead = remaining;
        }
        n = read(Sock,Buffer,toRead);

        write(outfd,Buffer,n);
        recived = recived +n;

    }
    close(outfd);
    close(Sock);

    if(recived == FileSize)
    {
        printf("Download complect...\n");
        return 0;
    }
    else
    {
        printf("Download failed..\n");
        return -1;
    }

    return 0;
}// End of main
