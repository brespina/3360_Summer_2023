/*
Brandon Espina
COSC 3360
07/14/2023
PROF RINCON
PROGRAMMING ASSIGNMENT 3
*/

/*
NOTE: This program is influenced by Dr. Rincon's UNIT 1 thread examples from CANVAS
      and PA1

CURRENT GOAL: Establishing baseline to make code match output using semaphores combined with PA1
              Not using same memory address. each child thread will be getting "own car keys" for now.

CURRENT PROBLEM: two synchronization issues. 1) passing info into child thread before another child accesses same memory
                 and attempts to modify information. 2) printing

TODO: Only pass info using one instance of struct. i.e. we are currently using a dynamic array of
      structs. change this to only one struct. Each child must share one set of "car keys"

WE ARE DISREGARDING THE REQUIREMENT THAT STATES CHILD THREAD INFO MUST BE PASSED BY
THE SAME MEMORY ADDRESS FOR NOW. 07/14
*/

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <vector>

struct threadData
{ // defining struct of data passed to thread
    std::string inputStr;
    std::string rleStr;
    std::vector<int> rleFreq;
    pthread_mutex_t *bsem;
    pthread_cond_t *printTurn;
    int *turn;
    int threadID;
    // add variable for portno to pass into openSock in void function call.
};

// creating void function and manipulating threadData
void *rle_encode(void *ptr)
{
    struct threadData *rleData = (struct threadData *)ptr; // create object of threadData = to dereferenced void xument ptr
    int count;
    std::string str = rleData->inputStr; // threadData input string being fed into str
    std::string rle;
    std::vector<int> freq;

    for (int i = 0; i < str.length(); i++)
    {
        count = 1;
        while (str[i] == str[i + 1])
        { // compare current char with next. if same inc count and i
            count++, i++;
        }

        if (count > 1)
        { // if count > 1 then run has occured
            rle += str[i];
            rle += str[i];         // add two instances of that char to RLE string, denoting run.
            freq.push_back(count); // populate vector<int> freq with count of repeated char
        }
        else
        {
            rle += str[i]; // if no run occured then add 1 instance of that char
        }
    }
    // feed resulting string into threadData object rleData
    rleData->rleStr = rle;
    for (int i = 0; i < freq.size(); i++)
        rleData->rleFreq.push_back(freq[i]);

    return NULL;
}

int main()
{
    pthread_mutex_t bsem;
    pthread_cond_t printTurn = PTHREAD_COND_INITIALIZER;
    std::vector<std::string> strVect;
    std::string temp;
    // read in the input strings from user into vector of strings
    while (std::cin >> temp)
        strVect.push_back(temp);

    pthread_mutex_init(&bsem, nullptr);
    static int turn;
    // std::string input;
    // std::cin >> input;
    // int size = input.size();
    //turn = size - 1;
    pthread_t *tid = new pthread_t[strVect.size()];
    threadData *x = new threadData[strVect.size()];
    for (int i = 0; i < strVect.size(); i++)
    {
        x[i].inputStr = strVect[i];
        x[i].bsem = &bsem;
        x[i].printTurn = &printTurn;
        //x[i].turn = &turn;
        x[i].threadID = i;
        pthread_create(&tid[i], nullptr, rle_encode, &x[i]);
    }
    for (int i = 0; i < strVect.size(); i++)
    {
        pthread_join(tid[i], nullptr);
    }

    for (int i = 0; i < strVect.size(); i++)
    {
        std::cout << "Input string: " << x[i].inputStr << std::endl;
        std::cout << "RLE String: " << x[i].rleStr << std::endl;
        std::cout << "RLE Frequencies: ";
        for (int j = 0; j < x[i].rleFreq.size(); j++) // iterate thru appropriate threadData rleFreq int vector and print
            std::cout << x[i].rleFreq[j] << " ";

        std::cout << std::endl
                  << std::endl;
    }
    if (tid != nullptr)
        delete[] tid;
    if (x != nullptr)
        delete[] x;
    return 0;
}