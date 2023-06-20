/*
Brandon Espina
06/20/2023
COSC 3360
Dr. Rincon
PracticePQ1sol
*/


#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {

    int pid;
    std::cout << "I am the parent process" << std::endl;

    for(int i = 0; i < 3; i++) {
        if((pid = fork()) == 0) {
            std::cout << "I am child process " << i << std::endl;

            if(i == 1) {
                if((pid = fork()) == 0) {
                    std::cout << "I am grandchild process from child process " << i << std::endl;
                    _exit(0);
                }
                wait(nullptr);  // wait for grandchildren
            }
            _exit(0);
        }
        wait(nullptr);  // wait for children
    }

    // for(int i = 0; i < 3; i++)
    //     wait(nullptr);
    
    return 0;
}