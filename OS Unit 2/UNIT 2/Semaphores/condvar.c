#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define NTHREADS 5
#define FAMILYNAME "CASTRO"

struct argFromMain
{
	char *famName;
	pthread_mutex_t * bsemPTR;
	pthread_cond_t *condVar;
	int *memb;
};

static pthread_mutex_t bsem;
static int members;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

void *access_house(void *family_void_ptr)
{
	char fam[20];
	strcpy(fam,(char *) family_void_ptr);
	printf("%s member arrives to the house\n", fam);
	pthread_mutex_lock(&bsem);
	if (strcmp(fam,FAMILYNAME)!=0)
		pthread_cond_wait(&empty, &bsem);
	members++;	
	pthread_mutex_unlock(&bsem);
	printf("%s member inside the house\n", fam);
	sleep(5);
	printf("%s member leaving the house\n", fam);
 	pthread_mutex_lock(&bsem);
	members--;
	if (strcmp(fam,FAMILYNAME) == 0 && members == 0)
		pthread_cond_signal(&empty);
 	pthread_mutex_unlock(&bsem);
	if (strcmp(fam,"RINCON") == 0) 
		pthread_cond_signal(&empty);
	return NULL;
}

int main()
{

 	pthread_t tid[NTHREADS];
	static int members=0;
	char family[NTHREADS][20];
   	pthread_mutex_init(&bsem, NULL); // Initialize access to 1

	for(int i=0;i<NTHREADS;i++)
	{
		if(i%2 == 0)
			strcpy((char *) &family[i],"RINCON");
		else
			strcpy((char *)&family[i],"CASTRO");
		if(pthread_create(&tid[i], NULL, access_house,(void *)&family[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < NTHREADS; i++)
        	pthread_join(tid[i], NULL);
	return 0;
}
