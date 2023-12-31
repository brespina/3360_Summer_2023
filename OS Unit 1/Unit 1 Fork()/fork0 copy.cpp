#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  int pid;
  std::cout << "I am the parent process " << std::endl;
  for (int i = 0; i < 4; i++) {
    if ((pid = fork()) == 0) {
      std::cout << "I am child process " << i << std::endl;

      if (i == 1 || i == 2) {
        for (int j = 0; j < 2; j++) {
          if ((pid = fork()) == 0) {
            std::cout << "I am a grandchild process from child process " << i << std::endl;
            //sleep(5);
            _exit(0);
          }
          wait(nullptr);
        }
      }

      //sleep(5);
      _exit(0);
    }
    wait(nullptr);
  }
  //  for (int i = 0; i < 4; i++)
  //      wait(nullptr);
  return 0;
}