/*
Brandon Espina
COSC 3360
06/20/2023
PROF RINCON
EC EXAM 1

NOTE: must call wait() in the same block as fork() is called to guarantee order of output.
      Ideally, we call wait() in an outside for loop an equal number of times
      that the parent calls fork() but for moodle's auto grading we must guarantee order of output.
*/

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {

    int pid;
    std::cout << "I am the parent process" << std::endl;
    for (int i = 0; i < 4; i++) {
        if((pid = fork()) == 0) {
            std::cout << "I am child process " << i << std::endl;

            if(i % 3 != 0) {  // if i is not 0 or 3
                for(int j = 0; j < 2; j++) {
                    if((pid = fork()) == 0) {
                        std::cout << "I am a grandchild process from process " << i << std::endl;
                        _exit(0);
                    }
                    wait(nullptr);  // wait for grandchildren
                }
            }
            _exit(0);
        }
        wait(nullptr);  // wait for children 
    }

    // for(int i = 0; i < 4; i++)
    //     wait(nullptr);


    return 0;
}