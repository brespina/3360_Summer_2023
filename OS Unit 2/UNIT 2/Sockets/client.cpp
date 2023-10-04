#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <vector>

/*
want to create a socket function to call pthread_create with.
isntead of rle function. child trhead should not know anything about the rle function here

add own check function. 


*/
int main(int argc, char *argv[])
{
    //     std::vector<std::string> strVect;
    // std::string temp;
    // // read in the input strings from user into vector of strings
    // while(std::cin >> temp)
    //     strVect.push_back(temp);

    int sockfd, portno, n, ret;
    std::string buffer;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc != 3) 
    {
       std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        std::cerr << "ERROR opening socket" << std::endl;
        exit(0);
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    ret = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    //std::cout << ret << std::endl;
    if (ret < 0) 
    {
        std::cerr << "ERROR connecting" << std::endl;
        exit(0);
    }
    std::cout << "Please enter the message: " << std::endl;
    std::getline(std::cin,buffer);
    int msgSize = buffer.size();
    n = write(sockfd,&msgSize,sizeof(int));
    if (n < 0) 
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = write(sockfd,buffer.c_str(),msgSize);
    if (n < 0) 
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = read(sockfd,&msgSize,sizeof(int));
    if (n < 0) 
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    char *tempBuffer = new char[msgSize+1];
    bzero(tempBuffer,msgSize+1);
    n = read(sockfd,tempBuffer,msgSize);
    if (n < 0) 
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    buffer = tempBuffer;
    delete [] tempBuffer;
    std::cout << "Message from server: "<< buffer << std::endl;
    close(sockfd);
    return 0;
}
