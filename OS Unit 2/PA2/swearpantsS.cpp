#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

void handleClient(int newsockfd)
{
    int n, msgSize = 0;
    n = read(newsockfd, &msgSize, sizeof(int));
    if (n < 0)
    {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }

    char *tempBuffer = new char[msgSize + 1];
    bzero(tempBuffer, msgSize + 1);
    n = read(newsockfd, tempBuffer, msgSize);
    if (n < 0)
    {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }
    std::string buffer = tempBuffer;
    delete[] tempBuffer;

    // Generate the RLE string and frequencies based on the received input string
    std::string rleString;
    std::vector<int> frequencies;
    char prevChar = '\0';
    int count = 0;

    // Step 1: Generate the RLE string
    for (char c : buffer)
    {
        if (c == prevChar)
        {
            count++;
        }
        else
        {
            if (count > 0)
            {
                rleString += prevChar;
                if (count > 1)
                {
                    rleString += prevChar;
                    frequencies.push_back(count);
                }
            }
            prevChar = c;
            count = 1;
        }
    }

    // Add the last run to the RLE string and frequencies
    if (count > 0)
    {
        rleString += prevChar;
        if (count > 1)
        {
            rleString += prevChar;
            frequencies.push_back(count);
        }
    }

    // Send the RLE string size
    int rleSize = rleString.size();
    n = write(newsockfd, &rleSize, sizeof(int));
    if (n < 0)
    {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    }

    // Send the RLE string
    n = write(newsockfd, rleString.c_str(), rleSize);
    if (n < 0)
    {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    }

    // Send the frequency array size
    int freqSize = frequencies.size();
    n = write(newsockfd, &freqSize, sizeof(int));
    if (n < 0)
    {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    }
    int *freqA = new int[freqSize];
    for (int i = 0; i < freqSize; i++)
        freqA[i] = frequencies[i];
    // Send the frequency array
    n = write(newsockfd, freqA, freqSize * sizeof(int));
    if (n < 0)
    {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    }

    close(newsockfd);
    exit(0);
}

void fireman(int signum)
{
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // Handle terminated child processes
        // std::cout << "Child process terminated: " << pid << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Check the commandline arguments
    if (argc != 2)
    {
        std::cerr << "Port not provided" << std::endl;
        exit(0);
    }

    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        exit(0);
    }

    // Populate the sockaddr_in structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket with the sockaddr_in structure
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error binding" << std::endl;
        exit(0);
    }

    // Set the max number of concurrent connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    signal(SIGCHLD, fireman);

    while (true)
    {
        // Accept a new connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (newsockfd < 0)
        {
            std::cerr << "Error accepting new connections" << std::endl;
            exit(0);
        }

        // Fork a new process to handle the client
        pid_t pid = fork();

        if (pid < 0)
        {
            std::cerr << "Error forking new process" << std::endl;
            exit(0);
        }
        else if (pid == 0)
        {
            // Child process
            // close(sockfd);  // Close the listening socket in the child process
            handleClient(newsockfd);
        }
        else
        {
            // Parent process
            close(newsockfd); // Close the new socket in the parent process
        }
    }

    close(sockfd);
    return 0;
}