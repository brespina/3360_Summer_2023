#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

static pthread_mutex_t bsem;
static pthread_cond_t rincon = PTHREAD_COND_INITIALIZER;
static pthread_cond_t castro = PTHREAD_COND_INITIALIZER;

void *access_one_at_a_time(void *family_void_ptr)
{

  
    return NULL;
}

int main()
{
    int nmembers;
    std::cin >> nmembers;
    pthread_mutex_init(&bsem, NULL); // Initialize access to 1
 	pthread_t *tid= new pthread_t[nmembers];
	char **family=new char*[nmembers];
    for(int i=0;i<nmembers;i++)
        family[i]=new char[20];
	for(int i=0;i<nmembers;i++)
	{
	
	    // Write the code to call pthread_create
	
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < nmembers; i++)
        	pthread_join(tid[i], NULL);
    for(int i=0;i<nmembers;i++)
        delete[] family[i];
    delete [] family;
    delete [] tid;
	return 0;
}