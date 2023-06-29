/*
Brandon Espina
06/27/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
client.cpp


NOTE: this is prof rincons example code from microsoft teams
      I will be using this a frame of reference.
*/

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
};

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
    // call to openSock in order to write
    // n = write(size of inputstr)
    // n = write(input string to client)
    // here we should only be calling write or send.
    // should be called within ptc
    // the socket connection should be already created
    return NULL;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

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
    portno = atoi(argv[2]);
    sockfd = openSock(portno, argv[1]);

    // this needs to be the input string.
    std::string message = strVect[0];

    int sMessage = message.length();
    n = write(sockfd, &sMessage, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    n = write(sockfd, message.c_str(), sMessage);
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    int size;
    n = read(sockfd, &size, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    n = read(sockfd, buffer, size);
    std::cout << buffer << std::endl;
    delete [] buffer;
    close(sockfd);
    return 0;
}
