/*
Brandon Espina
07/02/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
client.cpp

TODO: add concise comments to openSock function. add printing function.
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

 void check(int x, int cmp, std::string errMsg) {
    if(x < cmp) {
        std::cerr << errMsg << std::endl;
        exit(1);
    }
 }

 void checkHost(hostent *h) {
    if (h == NULL) {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
 }


int openSock(int portno, char* serverIp) {

    int clientSock, n;
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

void * threadSock(void * ptr) {
    struct threadData * tSock = (struct threadData*) ptr; // added int portNum
    int threadSockfd, sizeMess;
    threadSockfd = openSock(tSock->portNum, tSock->servIPAddr);
    sizeMess = (tSock->inputStr).length();

    check(write(threadSockfd, &sizeMess, sizeof(int)),0,"CLIENT ERROR: writing to socket");  //send size of input str
    check(write(threadSockfd, (tSock->inputStr).c_str(), sizeMess * sizeof(char)),0,"CLIENT ERROR: writing to socket");  //send input input str as char array
   
    //read size of rle_str and prepare a buffer to hold incoming rle_str
    int size;
    check(read(threadSockfd, &size, sizeof(int)), 0, "CLIENT ERROR: reading from socket");
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    check(read(threadSockfd, buffer, size * sizeof(char)), 0, "CLIENT ERROR: reading from socket");

    tSock->rleStr = buffer;
    delete[] buffer;

    //read size of rle_freq and prepare a temp dynamic int array to hold value of incoming rle_freq
    int sz;
    check(read(threadSockfd, &sz, sizeof(int)), 0, "CLIENT ERROR: reading from socket");
    int *freq= new int[sz];
    check(read(threadSockfd, freq, sz * sizeof(int)), 0, "CLEINT ERROR: reading from socket");

    for (int i=0; i<sz; i++) 
        tSock->rleFreq.push_back(freq[i]);   //populate threadData object with temp array.

    delete[] freq;

    close(threadSockfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> strVect;
    std::string temp;
    // read in the input strings from user into vector of strings
    while(std::cin >> temp)
        strVect.push_back(temp);

    check(argc, 3, "CLIENT ERROR: missing arguments");

    threadData* x = new threadData[strVect.size()]; 
    pthread_t* tid = new pthread_t[strVect.size()];

    for (int i = 0; i < strVect.size(); i++) {
        x[i].inputStr = strVect[i];
        x[i].portNum = atoi(argv[2]);
        x[i].servIPAddr = argv[1];

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
