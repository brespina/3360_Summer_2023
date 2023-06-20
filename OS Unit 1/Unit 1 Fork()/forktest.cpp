#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {

    int pid;
    std::cout << "I am a parent process" << std::endl;
    for(int i = 0; i < 4; i++) {
        if ((pid = fork()) == 0) {
            std::cout << "I am child process " << i << std::endl;

            if (i % 2 == 0) { //even child call four times
                for(int j = 0; j < 4; j++) {
                    if((pid = fork())==0) {
                        std::cout << "I am grandchild process from child process " << i << std::endl;
                        //sleep(5);
                        _exit(0);
                    }
                    wait(nullptr);
                }
            } 
            else {  // odd process call once
                if((pid = fork())==0) {
                    std::cout << "I am grandchild process from child process " << i << std::endl;
                    //sleep(5);
                    _exit(0);
                }
                wait(nullptr);
            }
            //sleep(5);
            _exit(0);
        }
        wait(nullptr);
    }

//    for (int l = 0; l < 4; l++)
//         for ( int k = 0; k < 4; k++)
//             wait(nullptr);


    return 0;
}