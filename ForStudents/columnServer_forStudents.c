/*-------------------------------------------------------------------------*
 *---									---*
 *---		columnServer.c						---*
 *---									---*
 *---	    This file defines a C program that gets file-sys commands	---*
 *---	from client via a socket, executes those commands in their own	---*
 *---	threads, and returns the corresponding output back to the	---*
 *---	client.								---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

//	Compile with:
//	$ gcc columnServer.c -o columnServer -lpthread

//---		Header file inclusion					---//

#include "clientServer.h"
#include <pthread.h> // For pthread_create()

//---		Definition of constants:				---//

#define STD_ERROR_MSG "Error doing operation"

const int STD_ERROR_MSG_LEN = sizeof(STD_ERROR_MSG) - 1;

#define STD_BYE_MSG "Good bye!"

const int STD_BYE_MSG_LEN = sizeof(STD_BYE_MSG) - 1;

const int ERROR_FD = -1;

struct InfoForClient
{
  int fd_;
  int threadNum_;
};

//---		Definition of global vars:				---//

//  PURPOSE:  To be non-zero for as long as this program should run, or '0'
//	otherwise.

//---		Definition of functions:				---//

//  PURPOSE:  To cast 'vPtr' to the pointer type coming from 'doServer()'
//	that points to two integers.  Then, to use those two integers,
//	one as a file descriptor, the other as a thread number, to fulfill
//	requests coming from the client over the file descriptor socket.
//	Returns 'NULL'.


void readfile(){
  int fr = open(FILENAME,O_RDONLY,0660);
  char buffer[256];
  read(fr,buffer,BUFFER_LEN);

  
  printf("%s",buffer);
  close(FILENAME); 
}

// void exit(){
//   printf("%s",STD_BYE_MSG);
// }

void *handleClient(void *vPtr)
{  
   struct InfoForClient *kuttay =(struct InfoForClient *)vPtr;
   
  //  I.  Application validity check:

  //  II.  Handle client:
  //  YOUR CODE HERE
  int fd = kuttay->fd_;        // <-- CHANGE THAT 0!
  int threadNum = kuttay->threadNum_; // <-- CHANGE THAT 0!

  //  YOUR CODE HERE
  char	buffer[BUFFER_LEN];
  char	command;
  int 	shouldContinue	= 1;
    
   while  (shouldContinue)
  {
    read(fd,buffer,BUFFER_LEN);
    command	= buffer[0];

 
    printf("Thread %d received: %c\n",threadNum,buffer[0]);
    if(WHOLE_FILE_CMD_CHAR == buffer[0]){
      readfile();
      //pthread_cancel(threadNum);
    }

    if(QUIT_CMD_CHAR == buffer[0]){
      printf("%s",STD_BYE_MSG);
      shouldContinue = 0;
      //send(listenFd , "message" , strlen("message"),0);
      char buffer[1024] = {0}; 
      char *hello = "Hello from server"; 
      send(3 , hello , strlen(hello) , 0 );

      
    }
    
    
    command	= buffer[0];
    
    // YOUR CODE HERE

  //  III.  Finished:
  printf("Thread %d quitting.\n", threadNum);
  return (NULL);
}
}


//  PURPOSE:  To run the server by 'accept()'-ing client requests from
//	'listenFd' and doing them.
void doServer(int listenFd)
{
  //  I.  Application validity check:

  //  II.  Server clients:
  pthread_t threadId;
  pthread_attr_t threadAttr;
  int threadCount = 0;
  struct InfoForClient *infoPtr;
  

  while (1)
  {
    //  YOUR CODE HERE
   

    infoPtr = (struct InfoForClient *)malloc(sizeof(struct InfoForClient));

    infoPtr->fd_ = accept(listenFd, NULL, NULL);
    

    
    infoPtr->threadNum_ = threadCount++;
    printf("This is Thread Count %d\n",threadCount);
    printf("pointer to %d\n ",infoPtr->threadNum_ );
   

    

    pthread_create(&threadId, &threadAttr, handleClient(infoPtr), infoPtr);
    free(infoPtr);
  }

  //  YOUR CODE HERE

  //  III.  Finished:
}

//  PURPOSE:  To decide a port number, either from the command line arguments
//	'argc' and 'argv[]', or by asking the user.  Returns port number.
int getPortNum(int argc,
               char *argv[])
{
  //  I.  Application validity check:

  //  II.  Get listening socket:
  int portNum;

  if (argc >= 2)
    portNum = strtol(argv[1], NULL, 0);
  else
  {
    char buffer[BUFFER_LEN];

    printf("Port number to monopolize? ");
    fgets(buffer, BUFFER_LEN, stdin);
    portNum = strtol(buffer, NULL, 0);
  }

  //  III.  Finished:
  return (portNum);
}

//  PURPOSE:  To attempt to create and return a file-descriptor for listening
//	to the OS telling this server when a client process has connect()-ed
//	to 'port'.  Returns that file-descriptor, or 'ERROR_FD' on failure.
int getServerFileDescriptor(int port)
{
  //  I.  Application validity check:

  //  II.  Attempt to get socket file descriptor and bind it to 'port':
  //  II.A.  Create a socket
  int socketDescriptor = socket(AF_INET,     // AF_INET domain
                                SOCK_STREAM, // Reliable TCP
                                0);

  if (socketDescriptor < 0)
  {
    perror("socket()");
    return (ERROR_FD);
  }

  //  II.B.  Attempt to bind 'socketDescriptor' to 'port':
  //  II.B.1.  We'll fill in this datastruct
  struct sockaddr_in socketInfo;

  //  II.B.2.  Fill socketInfo with 0's
  memset(&socketInfo, '\0', sizeof(socketInfo));

  //  II.B.3.  Use TCP/IP:
  socketInfo.sin_family = AF_INET;

  //  II.B.4.  Tell port in network endian with htons()
  socketInfo.sin_port = htons(port);

  //  II.B.5.  Allow machine to connect to this service
  socketInfo.sin_addr.s_addr = INADDR_ANY;

  //  II.B.6.  Try to bind socket with port and other specifications
  int status = bind(socketDescriptor, // from socket()
                    (struct sockaddr *)&socketInfo,
                    sizeof(socketInfo));

  if (status < 0)
  {
    perror("bind()");
    return (ERROR_FD);
  }

  //  II.B.6.  Set OS queue length:
  listen(socketDescriptor, 5);

  //  III.  Finished:
  return (socketDescriptor);
}

int main(int argc,char *argv[])
{
  //  I.  Application validity check:

  //  II.  Do server:
  int port = getPortNum(argc, argv);
  int listenFd = getServerFileDescriptor(port);
  int status = EXIT_FAILURE;
  printf("Hassan");
  printf("Hassan Pasha %d",listenFd);

  if (listenFd >= 0)
  {
    doServer(listenFd);
    close(listenFd);
    status = EXIT_SUCCESS;
  }

  //  III.  Finished:
  return (status);
}
