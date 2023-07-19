/*
Brandon Espina
UHID: 2041553
07/06/2023
COSC 3360
Dr. Rincon
EXAM 2- Interprocess communication
PQ - semaphores
*/

#include <pthread.h>
#include <iostream>
#include <string>

struct infoFromMain
{
    int threadID;
    int digit;
};

static pthread_mutex_t bsem;                               // Mutex semaphore
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER; // Condition variable to control the turn
static int turn = 0;                                       // value to control which child thread enters the critical section

void *printDigit(void *void_ptr)
{
    struct infoFromMain *info = (struct infoFromMain *)void_ptr;

    if (info->digit % 2 == 0)
        (info->digit)++;

    pthread_mutex_lock(&bsem);
    while (turn != info->threadID)
        pthread_cond_wait(&waitTurn, &bsem);

    pthread_mutex_unlock(&bsem);

    std::cout << info->digit;

    pthread_mutex_lock(&bsem);
    turn++;
    pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);

    return nullptr;
}

int main()
{
    std::string input;
    std::cin >> input;

    int nThreads = input.size();
    pthread_t *tid = new pthread_t[nThreads];
    infoFromMain *arg = new infoFromMain[nThreads];

    pthread_mutex_init(&bsem, nullptr); // Initializing bsem

    for (int i = 0; i < nThreads; i++)
    {
        arg[i].threadID = i;           // initialize the threadID based on the question's specifications
        arg[i].digit = input[i] - '0'; // assign the digit for each child thread based on the question's specifications
        if (pthread_create(&tid[i], nullptr, printDigit, &arg[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    for (int i = 0; i < nThreads; i++)
        pthread_join(tid[i], nullptr);

    if (arg != nullptr)
        delete[] arg;
    if (tid != nullptr)
        delete[] tid;
    return 0;
}