/*
Brandon Espina
06/29/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
client.cpp


NOTE: The following code is heavily influenced by Dr. Rincon's socket example code
      provided in CANVAS and TEAMS.
      I will be using this a frame of reference.
*/
/*
    need socket connection function to call in ptrhead_create
    send input -> 1 function
    receive server output -> 1function
    possibly create pthread_create callf unction
*/
//flow of thought: wanted to create socket creation void pionter function for ptc.
// ran into issue where server connect cannot be achieved without argv[] arguments.
// need another function that returns an int to establish connection.
// should then be called within void function?
// need another function that is int. takes argv[] values as arguments. returns socket();

//openSock works. next is to move the write to void function. call openSock into void func
// create threads with void.

// Please note this is a C program
// It compiles without warnings with gcc
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>

struct threadData { //defining struct of data passed to thread
    std::string inputStr;
    std::string rleStr;
    std::vector<int> rleFreq; 
    int portNum;  //add variable for portno to pass into openSock in void function call.
    char* servIPAddr; //need to also pass this in void thread function
};


int openSock(int portno, char* serverIp) {

    int clientSock, n;
    struct sockaddr_in serv_addr;
    struct hostent *host;
    //portno = atoi(argv[2]);
    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock < 0)
        std::cerr << "ERROR opening socket";
    host = gethostbyname(serverIp);
    if (host == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)host->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          host->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(clientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting";
        exit(1);
    }
    return clientSock;
}

void * threadSock(void * ptr) {
    struct threadData * tSock = (struct threadData*) ptr; // added int portNum
    int threadSockfd, n, sizeMess;
    threadSockfd = openSock(tSock->portNum, tSock->servIPAddr);
    sizeMess = (tSock->inputStr).length();
    n = write(threadSockfd, &sizeMess, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    n = write(threadSockfd, (tSock->inputStr).c_str(), sizeMess);
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    int size;
    n = read(threadSockfd, &size, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    n = read(threadSockfd, buffer, size);
    tSock->rleStr = buffer;
    std::cout << buffer << std::endl;
    delete [] buffer;
    // call to openSock in order to write
    // n = write(size of inputstr)
    // n = write(input string to client)
    // here we should only be calling write or send.
    // should be called within ptc
    // the socket connection should be already created
    close(threadSockfd);
    return NULL;
}

int main(int argc, char *argv[])
{
    std::vector<std::string> strVect;
    std::string temp;
    // read in the input strings from user into vector of strings
    while(std::cin >> temp)
        strVect.push_back(temp);

    if (argc < 3)
    {
        std::cerr << "usage " << argv[0] << "hostname port\n";
        exit(0);
    }

    threadData* x = new threadData[strVect.size()]; 
    pthread_t* tid = new pthread_t[strVect.size()];

    for (int i = 0; i < strVect.size(); i++) {
        x[i].inputStr = strVect[i];
        x[i].portNum = atoi(argv[2]);
        x[i].servIPAddr = argv[1];
        std::cout << &x[i] << std::endl;

        if (pthread_create(&tid[i], NULL, threadSock, &x[i])) {  //calling algorithm but also halting if evaluated as TRUE.
            std::cerr << "Error creating thread" << std::endl;
            return 1;
        }
    	//pthread_join(tid[i], NULL);
    }
	
	// Wait for the other threads to finish.
	for (int i = 0; i < strVect.size(); i++)
        	pthread_join(tid[i], NULL);
    // int sMessage = message.length();
    // n = write(sockfd, &sMessage, sizeof(int));
    // if (n < 0)
    // {
    //     std::cerr << "ERROR writing to socket";
    //     exit(1);
    // }
    // n = write(sockfd, message.c_str(), sMessage);
    // if (n < 0)
    // {
    //     std::cerr << "ERROR writing to socket";
    //     exit(1);
    // }
    // int size;
    // n = read(sockfd, &size, sizeof(int));
    // if (n < 0)
    // {
    //     std::cerr << "ERROR reading from socket";
    //     exit(1);
    // }
    // char *buffer = new char[size + 1];
    // bzero(buffer, size + 1);
    // n = read(sockfd, buffer, size);
    // std::cout << buffer << std::endl;
    // delete [] buffer;
    // del
    // close(sockfd);
    delete[] tid;
    delete[] x;
    return 0;
}
