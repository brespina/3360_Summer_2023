#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int pid;
  fork();
  //sleep(5);
  fork();
  //sleep(5);
  fork();
  //sleep(5);
  std::cout << "I am a process" << std::endl;
  return 0;
}