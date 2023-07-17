// #include <iostream>
// #include <pthread.h>
// #include <string>

// struct infoFromMain
// {
//     char ch;
//     pthread_mutex_t *bsem;
//     pthread_cond_t *printTurn;
//     int *turn;
//     int threadID;
// };

// void *reverse(void *void_ptr)
// {
//     infoFromMain ptr = *(infoFromMain *)void_ptr;
//     pthread_mutex_lock(ptr.bsem);
//     while (*ptr.turn != ptr.threadID)
//         pthread_cond_wait(ptr.printTurn, ptr.bsem);
//     pthread_mutex_unlock(ptr.bsem);
//     std::cout << ptr.ch;
//     pthread_mutex_lock(ptr.bsem);
//     (*ptr.turn)--;
//     pthread_cond_broadcast(ptr.printTurn);
//     pthread_mutex_unlock(ptr.bsem);

//     return nullptr;
// }

// int main()
// {
//     pthread_mutex_t bsem;
//     pthread_cond_t printTurn = PTHREAD_COND_INITIALIZER;
//     pthread_mutex_init(&bsem, nullptr);
//     static int turn;
//     std::string input;
//     std::cin >> input;
//     int size = input.size();
//     turn = size - 1;
//     pthread_t *tid = new pthread_t[size];
//     infoFromMain arg;
//     arg.bsem = &bsem;
//     arg.printTurn = &printTurn;
//     arg.turn = &turn;
//     for (int i = 0; i < size; i++)
//     {
//         arg.ch = input[i];
//         arg.threadID = i;
//         pthread_create(&tid[i], nullptr, reverse, &arg);
//     }
//     for (int i = 0; i < size; i++)
//     {
//         pthread_join(tid[i], nullptr);
//     }
//     if (tid != nullptr)
//         delete[] tid;
//     return 0;
// }

/*
Brandon Espina
COSC 3360
06/22/2023
PROF RINCON
PROGRAMMING ASSIGNMENT 1
*/

/*
NOTE: This program is influenced by Dr. Rincon's UNIT 1 thread examples from CANVAS

need to implement pa1 but instead of using separate memory addresses to pass information to the child threads
we will be sharing the same shared memory address amongst the child threads and must use semaphores inorder to 
solve the two synchronization problems. e.g. deal with race condition as well as print in order.
*/

#include <iostream> 
#include <string> 
#include <pthread.h> 
#include <unistd.h> 
#include <vector> 
#include <semaphore.h>
#include <semaphore>

struct threadData { //defining struct of data passed to thread
    std::string inputStr;
    std::string rleStr;
    std::vector<int> rleFreq; 
    // add variable for portno to pass into openSock in void function call.
    pthread_mutex_t *bsem;
    pthread_cond_t *printTurn;
    sem_t s;
};

//creating void function and manipulating threadData 
void * rle_encode(void *ptr) { 
    struct threadData rleData = *(struct threadData * ) ptr;  // create object of threadData = to dereferenced void argument ptr
    int count;
    //signal(ptr.s);
    std::string str = rleData.inputStr; //threadData input string being fed into str 
    std::string rle;
    std::vector<int> freq;
    //sem_init(rleData->s, 0);

    for (int i = 0; i < str.length(); i++) { 
        count = 1;
        while (str[i] == str[i + 1]) {   //compare current char with next. if same inc count and i
            count++, i++;
        }
        
        if (count > 1) {   //if count > 1 then run has occured
            rle += str[i]; 
            rle += str[i];   // add two instances of that char to RLE string, denoting run.
            freq.push_back(count);  // populate vector<int> freq with count of repeated char
        } 
        else {
            rle += str[i]; // if no run occured then add 1 instance of that char
        }
    }
    // feed resulting string into threadData object rleData
    rleData.rleStr = rle;
    for (int i=0; i<freq.size(); i++) 
        rleData.rleFreq.push_back(freq[i]); 

    return NULL;
}


int main() {
    std::vector<std::string> strVect;
    std::string temp;
    // read in the input strings from user into vector of strings
    while(std::cin >> temp)
        strVect.push_back(temp);


//everything following is heavily influenced by Dr. Rincon's thread example programs provided in CANVAS

    //declaring dynamic for input string x. and POSIX threads.
    threadData x;
    pthread_t* tid = new pthread_t[strVect.size()];
    sem_init(x.s, 0);
    for (int i = 0; i < strVect.size(); i++) {
        x.inputStr = strVect[i];

        if (pthread_create(&tid[i], NULL, rle_encode, &x)) {  //calling algorithm but also halting if evaluated as TRUE.
            std::cerr << "Error creating thread" << std::endl;
            return 1;
        }
        sem_wait(x.s);
    //	pthread_join(tid[i], NULL);
    }
	
	// Wait for the other threads to finish.
	for (int i = 0; i < strVect.size(); i++)
        	pthread_join(tid[i], NULL);

	for (int i = 0; i < strVect.size(); i++) {  
        std::cout << "Input string: " << x[i].inputStr << std::endl;
        std::cout << "RLE String: " << x[i].rleStr << std::endl;
        std::cout << "RLE Frequencies: ";
        for(int j = 0; j < x[i].rleFreq.size(); j++)  // iterate thru appropriate threadData rleFreq int vector and print
            std::cout << x[i].rleFreq[j] << " ";

        std::cout << std::endl << std::endl;
    }

    // deallocate memory
    delete[] x;
    delete[] tid;
	return 0;
}