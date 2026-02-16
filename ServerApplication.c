/*
    Server apllication
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

void SendFileToClient(int ClinetSocket,char * Filename)
{
    int fd = 0;
    struct stat sobj;
    char Buffer[1024];
    int BytesRead = 0;
    char Headder[64]={'\0'};

    printf("File name is:%s\n:%ld\n",Filename,strlen(Filename));

    fd = open(Filename,O_RDONLY);

    if(fd <0) // unable to open the file
    {
        printf("Unable to open the file");

        // send error message to client
        write(ClinetSocket,"ERR\n",4);
        return ;
    }

    stat(Filename,&sobj);

    // Header : "Ok 1700"

    snprintf(Headder,sizeof(Headder),"OK %ld\n",(long)sobj.st_size);

    // write header to client
    write(ClinetSocket,Headder,strlen(Headder));

    // Send Actual file content

    while(BytesRead = read(fd,Buffer,sizeof(Buffer) > 0))
    {
        // send the data to client 
        write(ClinetSocket,Buffer,BytesRead);

    }
    close(fd);

}

///////////////////////////////////////////
//  Commandline Argument Application
//  1st Argument: Port Number
//  argv[0]  argv[1]
///////////////////////////////////////////

int main(int argc , char * argv[])
{
    int ServerSocket = 0;
    int ClinetSocket = 0;
    int Port = 0;
    int iRet = 0;
    char Filename[50] = {'\0'};

    pid_t pid = 0;  // creating a variable

    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClinetAddr;

    socklen_t AddrLen = sizeof(ClinetAddr);


    if((argc < 2)||(argc > 2))
    {
        printf("Unable to proced as invalid number of arguments:\n");
        printf("Please provide the port number:\n");
        return -1;

    }
    // Port number of server
     Port= atoi(argv[1]);

    ///////////////////////////////////////////
    //  step 1 : createe TCP socket
    ///////////////////////////////////////////

    ServerSocket = socket(AF_INET,SOCK_STREAM,0);

    if(ServerSocket < 0)
    {
        printf("Unable to create the socket:\n");
        return -1;
    }

    ///////////////////////////////////////////
    //  step 2 : Bind socket to IP and Port
    ///////////////////////////////////////////


    memset(&ServerAddr,0,sizeof(ServerAddr));

    // Initilize the structure

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    iRet = bind(ServerSocket,(struct sockaddr*)&ServerAddr,sizeof(ServerAddr));

    if(iRet == -1)
    {
        printf("Unable to bind\n");
        close(ServerSocket);
        return -1;
    }
    ///////////////////////////////////////////
    //  step 3 : Listen for client connections
    ///////////////////////////////////////////

   

    iRet = listen(ServerSocket,11);

    if(iRet == -1)
    {
        printf("Server unable to listen the request\n");
        close(ServerSocket);
        return -1;
    }

    printf("Server is Running on port :%d\n",Port);

    //////////////////////////////////////////
    // Loop which accept client request contineously
    ///////////////////////////////////////////

    while(1)
    {
        ///////////////////////////////////////////
        //  step 4 : Accept the client request
        ///////////////////////////////////////////
        memset(&ClinetAddr,0,sizeof(ClinetAddr));

        printf("Server is waiting for client request\n");

        ClinetSocket = accept(ServerSocket,(struct sockaddr*)&ClinetAddr,&AddrLen);

        if(ClinetSocket < 0)
        {
            printf("Unable to accept client request\n");
            continue; //used for while

        }
        printf("Client gets connected :%s\n",inet_ntoa(ClinetAddr.sin_addr));

        /////////////////////////////////////////////////////////
        //  step 5 : Create new process to handel client request
        /////////////////////////////////////////////////////////

        pid = fork();

        if(pid < 0)
        {
            printf("Unabel to create a new process for client request\n");
            close(ClinetSocket);
            continue;
        }

        // New process gets created for client
        if(pid == 0)
        {
            printf("New process is created for client request\n");
            close(ServerSocket);
             
            iRet = read(ClinetSocket,Filename,sizeof(Filename));

            printf("Requested file by client:%s\n",Filename);
            Filename[strcspn(Filename,"\r\n")] = '\0';

            SendFileToClient(ClinetSocket,Filename);
           
            close(ClinetSocket);

            printf("File Transfer done & Client Disconnected\n");

            exit(0); // kill the child process

        }//End of if(fork)
        else // parent process(Server)
        {
            close(ClinetSocket);

        }// End of else

    }//End of while

    close(ServerSocket);

    return 0;
}// End of main
