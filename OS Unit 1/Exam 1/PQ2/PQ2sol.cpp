/*
Brandon Espina
06/21/2023
COSC 3360
Dr. Rincon
Exam 1
Programming Question 2

This is Dr. Rincon's solution provided on moodle
*/
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>

struct infoFromMain
{
  std::string message;
  int freq[256] = {0};
  int alphabetSize = 0;
};

void *freqCalculator(void *void_ptr)
{
  struct infoFromMain *ptr = (struct infoFromMain *) void_ptr;


  for(int count = 0; count < ptr->message.length(); count++) {
    ptr->freq[(int)ptr->message[count]]++;
  }
  
  for (int i = 0; i < 256; i++) {
      if(ptr->freq[i] != 0)
        ptr->alphabetSize++;
  }

  return nullptr;
}

int main()
{
  std::vector<std::string> input;
  std::string temp;
  
  while (std::getline(std::cin,temp))
    input.push_back(temp);
    
  int inputSize = input.size();
  
  infoFromMain *arg = new infoFromMain[inputSize];
  pthread_t *tid = new pthread_t[inputSize];
  
  for (int i = 0; i < inputSize; i++)
  {
    arg[i].message = input[i]; // initialize the message based on the input
    if (pthread_create(&tid[i], NULL, freqCalculator, &arg[i]))
    {
      fprintf(stderr, "Error creating thread\n");
      return 1;
    }
  }
  
  for (int i = 0; i < inputSize; i++)
    pthread_join(tid[i], NULL);
    
  for (int i = 0; i < inputSize; i++)
  {
    std::cout << "Message: " << input[i] << std::endl;
    std::cout << "Alphabet size: " << arg[i].alphabetSize << std::endl;
    std::cout << "Frequencies: ";
    int count = 0;
    if (arg[i].alphabetSize > 0)
    {
      for (int j = 0; j < 256; j++)
      {
        if (arg[i].freq[j] != 0)
        {
          std::cout << (char)j << " " << arg[i].freq[j];
          if (count < arg[i].alphabetSize-1)
          {
            std::cout << ", ";
            count++;
          }
        }
      }
      std::cout << std::endl;
    }
  }
  if (tid!=nullptr)
    delete [] tid;
  if (arg!=nullptr)
    delete [] arg;
  return 0;
}