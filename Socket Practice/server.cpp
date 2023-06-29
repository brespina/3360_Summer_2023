/*
Brandon Espina
06/27/2023
COSC 3360
Dr. Rincon
Programming Assignment 2
server.cpp


NOTE: this is prof rincons example code from microsoft teams
      I will be using this a frame of reference.
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


 
/* 
 need socket connection function + process socket in fork call
*/
void rle_encode(std::string str, std::string &rle_str, std::string &rle_fre)
{
    int count;
 
    for (int i = 0; str[i]; i++)
    {
        //cout <<"for loop: "<< i << endl;
        count = 1;
        while (str[i] == str[i + 1]) {
            //cout << "while loop: " << i << endl;
            count++, i++;
        }

        if (count > 1) {
            rle_str += str[i];
            rle_str += str[i];
            rle_fre += std::to_string(count) + " ";
        } 
        else {
            rle_str += str[i];
        }
    }
}

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    std::string rle_str = "";
    std::string freq = "";

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

            rle_encode(buffer, rle_str, freq);
            std::cout << "Size: " << size << std:: endl;
            std::cout << "Name of the student that knows how to use sockets: " << buffer << std::endl;
            char message[] = "Hello from Server";
            int sMessage = strlen(message);
            n = write(newsockfd, &sMessage, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, message, sMessage);
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            close(newsockfd);
            delete[] buffer;
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}
