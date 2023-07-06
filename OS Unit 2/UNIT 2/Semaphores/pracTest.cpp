/*
RINCON PAST EXAM 2 PQ:
EXPECTED OUTPUT FOR nThreads = 5;

I am Thread 3
I am Thread 1
I am Thread 0
I am Thread 2
I am Thread 4
*/

#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

static pthread_mutex_t bsem;                               // mutex semaphore
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER; // condition variable to control the turn
static int turn;                                           // Index to control access to the turn array
static int nThreads;                                       // Number of threads from input

void *print_oddDecreasing_evenIncreasing(void *void_ptr_argv)
{

    int *threadVal = (int *)void_ptr_argv;
    pthread_mutex_lock(&bsem);
    while (turn != *threadVal)
        pthread_cond_wait(&waitTurn, &bsem);
    pthread_mutex_unlock(&bsem);
    std::cout << "I am Thread " << *threadVal << std::endl;
    pthread_mutex_lock(&bsem);
    if (*threadVal % 2 == 1)
        turn -= 2;
    else
        turn += 2;
    if (turn < 0)
        turn = 0;
    pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);
    return nullptr;
}

int main()
{
    std::cin >> nThreads;
    pthread_mutex_init(&bsem, nullptr); // this initializes bsem to 1
    pthread_t *tid = new pthread_t[nThreads];
    int *threadNumber = new int[nThreads];

    if(nThreads % 2 == 0)
        turn = nThreads - 1;
    else
        turn = nThreads -2; // initialize you turn here (you might need to use an if statement to assign value of the turn variable)
    for (int i = 0; i < nThreads; i++)
    {
        threadNumber[i] = i;                                                                    // initialize the thread number here (remember to follow the rules from the specifications of the assignment)
        pthread_create(&tid[i], nullptr, print_oddDecreasing_evenIncreasing, &threadNumber[i]); // call pthread_create
    }

    for (int i = 0; i < nThreads; i++)
        pthread_join(tid[i], nullptr);

    delete[] threadNumber;
    delete[] tid;

    return 0;
}