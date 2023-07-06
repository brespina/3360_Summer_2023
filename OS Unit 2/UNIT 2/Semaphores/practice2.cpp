#include <iostream>
#include <pthread.h>
#include <string>

struct infoFromMain
{
    char ch;
    pthread_mutex_t *bsem;
    pthread_cond_t *printTurn;
    int *turn;
    int threadID;
};

void *reverse(void *void_ptr)
{
    infoFromMain *ptr = (infoFromMain *)void_ptr;
    pthread_mutex_lock(ptr->bsem);
    while (*ptr->turn != ptr->threadID)
        pthread_cond_wait(ptr->printTurn, ptr->bsem);
    pthread_mutex_unlock(ptr->bsem);
    std::cout << ptr->ch;
    pthread_mutex_lock(ptr->bsem);
    (*ptr->turn)--;
    pthread_cond_broadcast(ptr->printTurn);
    pthread_mutex_unlock(ptr->bsem);

    return nullptr;
}

int main()
{
    pthread_mutex_t bsem;
    pthread_cond_t printTurn = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&bsem, nullptr);
    static int turn;
    std::string input;
    std::cin >> input;
    int size = input.size();
    turn = size - 1;
    pthread_t *tid = new pthread_t[size];
    infoFromMain *arg = new infoFromMain[size];
    for (int i = 0; i < size; i++)
    {
        arg[i].ch = input[i];
        arg[i].bsem = &bsem;
        arg[i].printTurn = &printTurn;
        arg[i].turn = &turn;
        arg[i].threadID = i;
        pthread_create(&tid[i], nullptr, reverse, &arg[i]);
    }
    for (int i = 0; i < size; i++)
    {
        pthread_join(tid[i], nullptr);
    }
    if (tid != nullptr)
        delete[] tid;
    if (arg != nullptr)
        delete[] arg;
    return 0;
}