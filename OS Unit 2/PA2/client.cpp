/*
Brandon Espina
06/30/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
client.cpp

WHAT I FIXED:  finally fixed passing int freq array after realizing
               i was only passing enough bytes for 1 int isntead of 
               (size of array * sizeof(int)) 
TODO: Collapse repetitive code into own function e.g. create a "check function."
      Lots of clean up to do.
      
    ..client: add comments. 
    
    ..server: add comments.
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
    //send size of input str
    n = write(threadSockfd, &sizeMess, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    //send input char array
    n = write(threadSockfd, (tSock->inputStr).c_str(), sizeMess);
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    //read size of rle_str 
    int size;
    n = read(threadSockfd, &size, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    // read incoming rle_str into bbuffer
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    n = read(threadSockfd, buffer, size);
    tSock->rleStr = buffer;
    delete [] buffer;
    int sz;
    n = read(threadSockfd, &sz, sizeof(int));
    if(n < 0) 
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    int freq[sz];
    n = read(threadSockfd, freq, sz * sizeof(int));

    for (int i=0; i<sz; i++) 
        tSock->rleFreq.push_back(freq[i]); 

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
        //std::cout << &x[i] << std::endl;

        if (pthread_create(&tid[i], NULL, threadSock, &x[i])) {  //calling algorithm but also halting if evaluated as TRUE.
            std::cerr << "Error creating thread" << std::endl;
            return 1;
        }
    	//pthread_join(tid[i], NULL);
    }
	
	// Wait for the other threads to finish.
	for (int i = 0; i < strVect.size(); i++)
        	pthread_join(tid[i], NULL);

	for (int i = 0; i < strVect.size(); i++) {  
        std::cout << "Input string: " << x[i].inputStr << std::endl;
        std::cout << "RLE String: " << x[i].rleStr << std::endl;
        std::cout << "RLE Frequencies: ";
        for(int j = 0; j < x[i].rleFreq.size(); j++)  // iterate thru appropriate threadData rleFreq int vector and print
            std::cout << x[i].rleFreq[j] << " ";

        std::cout << std::endl << std::endl;
    }
    
    delete[] tid;
    delete[] x;
    return 0;
}
