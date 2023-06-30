/*
Brandon Espina
06/30/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
server.cpp

WHAT I FIXED:  finally fixed passing int freq array after realizing
               i was only passing enough bytes for 1 int isntead of 
               (size of array * sizeof(int)) 
TODO: Collapse repetitive code into own function e.g. create a "check function."
      Lots of clean up to do.
      
    ..client: add comments. 
    
    ..server: add comments.
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


 

void *rle_encode(char * charray, std::string &rleOut, std::vector<int> &freqy) { 
    int count;
    std::string str = charray; //threadData input string being fed into str 
    //std::string rle;
    //std::vector<int> freq;

    for (int i = 0; i < str.length(); i++) { 
        count = 1;
        while (str[i] == str[i + 1]) {   //compare current char with next. if same inc count and i
            count++, i++;
        }
        
        if (count > 1) {   //if count > 1 then run has occured
            rleOut += str[i]; 
            rleOut += str[i];   // add two instances of that char to RLE string, denoting run.
            freqy.push_back(count);  // populate vector<int> freq with count of repeated char
        } 
        else {
            rleOut += str[i]; // if no run occured then add 1 instance of that char
        }
    }

    return NULL;
}

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman); 
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding";
        exit(1);
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true)
    {   
        std::vector<int> frequency;
        frequency.clear();
        //int freqBuffer[1024];
        std::string rle_str = "";
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            if (newsockfd < 0)
            {
                std::cerr << "ERROR on accept";
                exit(1);
            }
            int size;
            n = read(newsockfd, &size, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            char *buffer = new char[size+1];
            bzero(buffer, size+1);
            n = read(newsockfd, buffer, size);
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }

            rle_encode(buffer, rle_str, frequency);
           // std::cout << "Size: " << size << std:: endl;
            //std::cout << "RLE output string: " << rle_str << std::endl;
            
            int sMessage = rle_str.length();
            n = write(newsockfd, &sMessage, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, rle_str.c_str(), sMessage);
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            int sFreq = frequency.size();
            int *fArr =new int[sFreq];
            for(int i = 0; i < sFreq; i++)
                fArr[i] = frequency[i];
            n = write(newsockfd, &sFreq, sizeof(int));
            if (n < 0 ) 
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            //n = send(newsockfd, &fArr, sFreq * sizeof(int), 0);
            n = write(newsockfd, fArr ,sFreq * sizeof(int));
            if (n<0) {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            //handle socket and dynamic array
            close(newsockfd);
            delete[] buffer;
            delete[] fArr;
            
            _exit(0);
        }
        //wait(nullptr);
    }
    close(sockfd);
    return 0;
}
