#include <iostream>
// #include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// I am original process
// I am child process 0
// I am grand child process 0 from child 0
// I am grand child process 1 from child 0
// I am grand child process 2 from child 0
// I am child process 1
// I am child process 2
// I am grandchild process 0 from child 2
// I am grand child process 1 from child 2
// I am child process 3
// I am grand child process 0 from child 3
// I am grand child process 1 from child 3
// I am grand child process 2 from child 3
// I am grand child process 3 from child 3



// I am original
// i am child 1
// i am grandchild to child one
// i am grandhcild to child one
// i am child 2
// i am grandchild to child 2
// i am child 3
int main()
{
  //std::cout << "my name is" << getpid() << std::endl;
  std::cout << "I am the parent process " << std::endl;
  int pid;

  for (int i = 0; i < 4; i++)
  {
    pid = fork();
    if (pid == 0)
      {
        std::cout << "I am the child process " << i << std::endl;
        //std::cout << "my name is " << getpid() << " " << "my parent is " << getppid() << std::endl;
      if (i == 0)
      {
        for (int j = 0; j < 3; j++)
        {
          pid = fork();
          if (pid == 0)
            {
              std::cout << "I am grandchild " << j << "from child " << i << std::endl;
              //std::cout << "my name is " << getpid() << " " << "my parent is " << getppid() << std::endl;
              _exit(0);
            }
           wait(nullptr);
        }
      }
      if (i == 2)
      {
        for (int k = 0; k < 2; k++)
        {
          pid = fork();
          if (pid == 0)
          {
            std::cout << "I am grandchild " << k << "from child " << i << std::endl;
            //std::cout << "my name is " << getpid() << " " << "my parent is " << getppid() << std::endl;
            _exit(0);
          }
          wait(nullptr);
        }
      }
      if (i == 3)
      {
        for (int m = 0; m < 4; m++)
        {
          pid = fork();
          if (pid == 0)
          {
            std::cout << "I am grandchild " << m << "from child " << i << std::endl;
            // std::cout << "my name is " << getpid() << " " << "my parent is " << getppid() << std::endl;
            _exit(0);
          }
          wait(nullptr);
        } 
      }
      _exit(0);
    }
    wait(nullptr);
  }

  return 0;
}