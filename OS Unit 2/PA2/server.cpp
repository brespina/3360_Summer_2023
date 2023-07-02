/*
Brandon Espina
07/02/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
server.cpp

NOTE: The code in connectServSock, handleRequest, and fireman was copied (then modified)
      from the code Dr. Rincon provided on CANVAS and TEAMS.
      Namely the code from Socket Practice from client.cpp and server.cpp files

      In addition the rle_encode function is being reused, slightly modified from my PA1.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>

// simple check function to clean up the 10 billion if statements
void check(int x, int cmp, std::string errMsg)
{
    if (x < cmp)
    {
        std::cerr << errMsg << std::endl;
        exit(1);
    }
}

// socket connection handle from incoming client request.
int connectServSock(int portno)
{
    int serverSock;
    struct sockaddr_in serv_addr;

    check(serverSock = socket(AF_INET, SOCK_STREAM, 0), 0, "SERVER ERROR: opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    check(bind(serverSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), 0, "SERVER ERROR: on binding");
    return serverSock;
}

void *rle_encode(char *charray, std::string &rleOut, std::vector<int> &freqy)
{
    int count;
    std::string str = charray; // threadData input string being fed into str

    for (int i = 0; i < str.length(); i++)
    {
        count = 1;
        while (str[i] == str[i + 1]) // compare current char with next. if same inc count and i
            count++, i++;

        if (count > 1)
        { // if count > 1 then run has occured
            rleOut += str[i];
            rleOut += str[i];       // add two instances of that char to RLE string, denoting run.
            freqy.push_back(count); // populate vector<int> freq with count of repeated char
        }
        else
            rleOut += str[i]; // if no run occured then add 1 instance of that char
    }
    return NULL;
}

void handleRequest(int sock)
{
    // initialize temp variables
    std::string rle_str = "";
    std::vector<int> frequency;
    frequency.clear();

    check(sock, 0, "SERVER ERROR: on accept"); // validate socket acceptance

    // read in size of input str from client. create temp char array buffer. read in input str.
    int size;
    check(read(sock, &size, sizeof(int)), 0, "SERVER ERROR: reading from socket");
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    check(read(sock, buffer, size), 0, "SERVER ERROR: reading from socket");

    rle_encode(buffer, rle_str, frequency); // rle function on client input str

    // send client size of rle_str. send client rle_str.
    int sMessage = rle_str.length();
    check(write(sock, &sMessage, sizeof(int)), 0, "SERVER ERROR: writing to socket");
    check(write(sock, rle_str.c_str(), sMessage), 0, "SERVER ERROR: writing to socket");

    // copy values from frequency vector to int array freqArr. send client size of freqArr. send client freqArr
    int sFreq = frequency.size();
    int *freqArr = new int[sFreq];
    for (int i = 0; i < sFreq; i++)
        freqArr[i] = frequency[i];
    check(write(sock, &sFreq, sizeof(int)), 0, "SERVER ERROR: writing to socket");
    check(write(sock, freqArr, sFreq * sizeof(int)), 0, "SERVER ERROR: writing to socket");

    // handle socket and dynamic array
    delete[] buffer;
    delete[] freqArr;
    close(sock);

    _exit(0);
}

void fireman(int)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in cli_addr;
    signal(SIGCHLD, fireman);

    check(argc, 2, "SERVER ERROR: no port provided\n");

    portno = atoi(argv[1]);
    sockfd = connectServSock(portno);
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
            handleRequest(newsockfd);
    }

    close(sockfd);
    return 0;
}
