// BELOW IS THE CODE THAT WAS PROVIDED FOR THE QUESTION
/*
#include <pthread.h>
#include <iostream>
#include <iomanip>

#define NOPER 4

struct operation
{
	int val1;
	int val2;
    int op;
    double result;
};

void *calculator(void *pos_void_ptr)
{
	struct operation *pos_ptr = // cast pos_void_ptr to a struct operation *
    switch(pos_ptr->op)
    {
        case 0: // Addition 
                break;
        case 1: // Subtraction 
                break;
        case 2: //Multiplication 
                break;
        case 3: // Division        
                break;
    }
	return NULL;
}

int main()
{
	static struct operation operations[NOPER];
 	pthread_t tid[NOPER];
        
	for(int i=0;i<NOPER;i++)
	{
		operations[i].op = i;
        std::cin >> operations[i].val1;
        std::cin >> operations[i].val2;
		if(// Call pthread_create ) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}		
	}
	
	// Wait for the other threads to finish.
    // Call pthread_join here
    
    for (int i = 0; i < NOPER; i++)
    {
        switch(operations[i].op)
        {
            case 0: std::cout << operations[i].val1 << " + " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
            case 1: std::cout << operations[i].val1 << " - " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
            case 2: std::cout << operations[i].val1 << " * " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
            case 3: std::cout << operations[i].val1 << " / " << operations[i].val2 << " = " << std::fixed << std::setprecision(2) << operations[i].result << std::endl;
                    break;
        }
    }
    return 0;
}
*/