/*
Brandon Espina
06/21/2023
COSC 3360
Dr. Rincon
Exam 1
Programming Question 1
*/


#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>


int main()  {
    int pid;
    std::cout << "I am the parent process" << std::endl;
    
    for(int i = 0; i < 4; i++) {
        if((pid = fork()) == 0) {
            std::cout << "I am the child process " << i << std::endl;
            
            // want to call fork 2^i times
        // math.h was not included. however you can write own power function, or brute force "if else" it
            for(int j = 0; j < pow(2,i); j++) {
                if((pid = fork()) == 0) {
                    std::cout << "I am a grandchild process from child process " << i << std::endl;
                    _exit(0);
                }
            wait(nullptr);  // wait grandchild
            }
            _exit(0);
        }
        wait(nullptr);  // wait child
    }
    return 0;
}