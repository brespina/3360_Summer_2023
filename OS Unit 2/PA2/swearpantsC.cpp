#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <pthread.h>

struct ThreadData
{
    int port;
    char *serverIP;
    std::string input;
    std::string rleString;
    std::vector<int> frequencies;
};

void *sendRequest(void *arg)
{
    struct ThreadData *data = (struct ThreadData *)arg;
    // std::string buffer;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd;
    //  std::string buffer = data->buffer;

    server = gethostbyname(data->serverIP);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(0);
    }
    // Prepares serv_addr structure with necessary info to establish network conn.
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(data->port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket" << std::endl;
        exit(0);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting" << std::endl;
        exit(0);
    }

    int n;
    int msgSize = (data->input).size();
    n = write(sockfd, &msgSize, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = write(sockfd, (data->input).c_str(), msgSize);
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }

    // Read the RLE string size
    int rleSize;
    n = read(sockfd, &rleSize, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }

    // Read the RLE string
    char *rleBuffer = new char[rleSize + 1];
    bzero(rleBuffer, rleSize + 1);
    n = read(sockfd, rleBuffer, rleSize);
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    data->rleString = rleBuffer;
    delete[] rleBuffer;

    // Read the frequency array size
    int freqSize;
    n = read(sockfd, &freqSize, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }

    // Read the frequency array
    int *intA = new int[freqSize];

    n = read(sockfd, intA, freqSize * sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }

    for (int i = 0; i < freqSize; i++)
        data->frequencies[i] = intA[i];
    // Store the response data in the ThreadData object

    delete[] intA;

    close(sockfd);
}

int main(int argc, char *argv[])
{
    std::vector<std::string> str;
    std::string temp;
    int sockfd, portno, n;
    if (argc != 3)
    {
        std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
        exit(0);
    }

    std::vector<pthread_t> threads;
    std::vector<ThreadData *> threadDataList;

    // Read and send multiple lines of input to the server
    while (std::getline(std::cin, temp))
        str.push_back(temp);

    ThreadData *data = new ThreadData[str.size()];
    pthread_t *thread = new pthread_t[str.size()];
    // data->sockfd = sockfd;
    // data->buffer = buffer;

    for (int i = 0; i < str.size(); i++)
    {
        // populate data so that we can give child thread information
        data[i].input = str[i];
        data[i].port = atoi(argv[2]);
        data[i].serverIP = argv[1];

        if (pthread_create(&thread[i], NULL, sendRequest, &data[i]) != 0)
        {
            std::cerr << "ERROR creating thread" << std::endl;
            exit(0);
        }
    }

    for (int i = 0; i < str.size(); i++)
        pthread_join(thread[i], NULL);

    for (int i = 0; i < str.size(); i++)
    {
        // Output the original message, RLE string, and frequency array
        std::cout << "Input String: " << data[i].input << std::endl;
        std::cout << "RLE String: " << data[i].rleString << std::endl;
        std::cout << "RLE Frequencies: ";
        for (int j = 0; j < data[i].frequencies.size(); j++)
        {
            std::cout << data[i].frequencies[j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}