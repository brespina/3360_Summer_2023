/*
Brandon Espina
07/02/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
client.cpp

NOTE: The code in openSock, threadSock, and produceThread, was copied (then modified)
      from the code Dr. Rincon provided on CANVAS and TEAMS.
      Namely the code from Socket Practice from client.cpp and server.cpp files

      The printParent function is copied from my PA1, but created its own function.
*/

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

struct threadData
{ // defining struct of data passed to thread
    std::string inputStr;
    std::string rleStr;
    std::vector<int> rleFreq;
    int portNum;      // add variable for portno to pass into openSock in void function call.
    char *servIPAddr; // need to also pass this in void thread function
};

void check(int x, int cmp, std::string errMsg)
{
    if (x < cmp)
    {
        std::cerr << errMsg << std::endl;
        exit(1);
    }
}

void checkHost(hostent *h)
{
    if (h == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
}

// open socket with server with arg values from main call. matching server main call.
int openSock(int portno, char *serverIp)
{

    int clientSock;
    struct sockaddr_in serv_addr;
    struct hostent *host;

    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    check(clientSock, 0, "ERROR opening socket");

    host = gethostbyname(serverIp);
    checkHost(host);

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)host->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          host->h_length);
    serv_addr.sin_port = htons(portno);

    check(connect(clientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), 0, "CLIENT ERROR: connecting");
    return clientSock;
}

void *threadSock(void *ptr)
{
    struct threadData *tSock = (struct threadData *)ptr; // added int portNum
    int threadSockfd, sizeMess;
    threadSockfd = openSock(tSock->portNum, tSock->servIPAddr);
    sizeMess = (tSock->inputStr).length();

    check(write(threadSockfd, &sizeMess, sizeof(int)), 0, "CLIENT ERROR: writing to socket");                             // send size of input str
    check(write(threadSockfd, (tSock->inputStr).c_str(), sizeMess * sizeof(char)), 0, "CLIENT ERROR: writing to socket"); // send input input str as char array

    // read size of rle_str and prepare a buffer to hold incoming rle_str
    int size;
    check(read(threadSockfd, &size, sizeof(int)), 0, "CLIENT ERROR: reading from socket");
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    check(read(threadSockfd, buffer, size * sizeof(char)), 0, "CLIENT ERROR: reading from socket");

    tSock->rleStr = buffer;
    delete[] buffer;

    // read size of rle_freq and prepare a temp dynamic int array to hold value of incoming rle_freq
    int sz;
    check(read(threadSockfd, &sz, sizeof(int)), 0, "CLIENT ERROR: reading from socket");
    int *freq = new int[sz];
    check(read(threadSockfd, freq, sz * sizeof(int)), 0, "CLEINT ERROR: reading from socket");

    for (int i = 0; i < sz; i++)
        tSock->rleFreq.push_back(freq[i]); // populate threadData object with temp array.

    delete[] freq;

    close(threadSockfd);
    return NULL;
}

void produceThreads(std::vector<std::string> sVec, pthread_t *tid, struct threadData *x, char *servIP, char *port)
{
    for (int i = 0; i < sVec.size(); i++)
    {
        x[i].inputStr = sVec[i];
        x[i].servIPAddr = servIP;
        x[i].portNum = atoi(port);

        if (pthread_create(&tid[i], NULL, threadSock, &x[i]))
        { // calling algorithm but also halting if evaluated as TRUE.
            std::cerr << "Error creating thread" << std::endl;
            // return 1;
            exit(1);
        }
    }
    // Wait for the other threads to finish.
    for (int i = 0; i < sVec.size(); i++)
        pthread_join(tid[i], NULL);
}

void printParent(std::vector<std::string> sVec, struct threadData *tData)
{
    for (int i = 0; i < sVec.size(); i++)
    {
        std::cout << "Input string: " << tData[i].inputStr << std::endl;
        std::cout << "RLE String: " << tData[i].rleStr << std::endl;
        std::cout << "RLE Frequencies: ";
        for (int j = 0; j < tData[i].rleFreq.size(); j++) // iterate thru appropriate threadData rleFreq int vector and print
            std::cout << tData[i].rleFreq[j] << " ";

        std::cout << std::endl
                  << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::string> strVect;
    std::string temp;
    char *serv_IP = argv[1];
    char *port = argv[2];

    while (std::cin >> temp)
        strVect.push_back(temp);

    check(argc, 3, "CLIENT ERROR: missing arguments");

    threadData *x = new threadData[strVect.size()];
    pthread_t *tid = new pthread_t[strVect.size()];

    produceThreads(strVect, tid, x, serv_IP, port);
    printParent(strVect, x);

    delete[] tid;
    delete[] x;
    return 0;
}
