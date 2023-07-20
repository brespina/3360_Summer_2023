/*
Brandon Espina
COSC 3360
07/20/2023
PROF RINCON
PROGRAMMING ASSIGNMENT 3
*/

/*
NOTE: This program is influenced by Dr. Rincon's UNIT 1 thread examples from CANVAS, my own PA1,
      Dr. Rincon's UNIT 2 Semaphores Folder of example code, namely, mutexsem.cpp,
      Dr. Rincon's Programming Question on the Summer 2023 Exam 2,
      Dr. Rincon's "reverse a string" code provided on TEAMS on 07/05/2023,
      the print and rle_func functions are slightly modified functions from my PA2
*/

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <vector>

struct threadData // defining struct of data to be passed into child from main. modified from PA1
{
    std::string inputStr;
    int threadID;
    int *turn;
    pthread_mutex_t *bsem;
    pthread_mutex_t *copy;
    pthread_cond_t *wait;
};

// each child thread will execute this concurrently as we only use variables local to the child thread as arguments
void *rle_func(std::string input, std::string &rleOut, std::vector<int> &freqy)
{
    int count;
    std::string str = input; // input string received from child thread defined in CS1

    for (int i = 0; i < str.length(); i++)
    {
        count = 1;
        while (str[i] == str[i + 1]) // compare current char with next. if same inc count and i
            count++, i++;

        if (count > 1) // if count > 1 then run has occured
        {
            rleOut += str[i];
            rleOut += str[i];       // add two instances of that char to RLE string, denoting run.
            freqy.push_back(count); // populate vector<int> freq with count of repeated char
        }
        else
            rleOut += str[i]; // if no run occured then add 1 instance of that char
    }
    return nullptr;
}

// all variables being printed are local to the respective child thread. Call outside critical section
void printChild(std::string inputStr, std::string rleStr, std::vector<int> frequency)
{

    std::cout << "Input string: " << inputStr << std::endl;
    std::cout << "RLE String: " << rleStr << std::endl;
    std::cout << "RLE Frequencies: ";
    for (int j = 0; j < frequency.size(); j++)
        std::cout << frequency[j] << " ";

    std::cout << std::endl
              << std::endl;
}

// creating void function and manipulating threadData
void *rle_encode(void *ptr)
{
    struct threadData rleData = *(struct threadData *)ptr;

    // atomically copying instance of shared struct into local variables
    std::string str = rleData.inputStr;
    int threadNum = rleData.threadID;

    pthread_mutex_unlock(rleData.copy); // child unlocks the lock from parent
/*---------------------------END of CS1---------------------------*/

    std::string rle;
    std::vector<int> freq;

    rle_func(str, rle, freq); // rle_function is concurrently being executed across each child

/*--------------------------START of CS2--------------------------*/
    // all children wait turn to print
    pthread_mutex_lock(rleData.bsem);
    while (*rleData.turn != threadNum)
        pthread_cond_wait(rleData.wait, rleData.bsem);
    pthread_mutex_unlock(rleData.bsem);
/*---------------------------END of CS2---------------------------*/

    printChild(str, rle, freq);

/*--------------------------START of CS3--------------------------*/
    // after print increment turn
    pthread_mutex_lock(rleData.bsem);
    (*rleData.turn)++;
    pthread_cond_broadcast(rleData.wait);
    pthread_mutex_unlock(rleData.bsem);
/*---------------------------END of CS3---------------------------*/
    return nullptr;
}

int main()
{
    pthread_mutex_t bsem;
    pthread_mutex_t copy;
    pthread_cond_t wait = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&bsem, nullptr);
    pthread_mutex_init(&copy, nullptr);
    static int turn;
    turn = 0;

    std::vector<std::string> strVect;
    std::string temp;

    //  read in the input strings from moodle into vector of strings
    while (std::cin >> temp)
        strVect.push_back(temp);

    pthread_t *tid = new pthread_t[strVect.size()];
    threadData x; // requirements restrict our data container to one memory address that must be shared
    x.turn = &turn;
    x.bsem = &bsem;
    x.wait = &wait;
    x.copy = &copy;
    
    for (int i = 0; i < strVect.size(); i++)
    {
/*-------------------------START of CS1-------------------------*/
        pthread_mutex_lock(&copy);
        x.inputStr = strVect[i];
        x.threadID = i;

        pthread_create(&tid[i], nullptr, rle_encode, &x); // CS1 will end in the child thread after data is copied into local variables
    }

    for (int i = 0; i < strVect.size(); i++)
    {
        pthread_join(tid[i], nullptr);
    }

    if (tid != nullptr)
        delete[] tid;
    return 0;
}