/*
Brandon Espina
COSC 3360
06/22/2023
PROF RINCON
PROGRAMMING ASSIGNMENT 1
*/

/*
NOTE: This program is influenced by Dr. Rincon's UNIT 1 thread examples from CANVAS
*/

#include <iostream> 
#include <string> 
#include <pthread.h> 
#include <unistd.h> 
#include <vector> 

using namespace std;

struct threadData { //defining struct of data passed to thread
    string inputStr;
    string rleStr;
    vector<int> rleFreq; 
};

//creating void function and manipulating threadData 
void * rle_encode(void *ptr) { 
    struct threadData * rleData = (struct threadData * ) ptr;  // create object of threadData = to dereferenced void argument ptr
    int count;
    string str = rleData->inputStr; //threadData input string being fed into str 
    string rle;
    vector<int> freq;

    //for loop that avoids calling .length
    for (int i = 0; i < str.length(); i++) { 
        count = 1;
        while (str[i] == str[i + 1]) {   //compare current char with next. if same inc count and i
            count++, i++;
        }
        
        if (count > 1) {   //if count > 1 then run has occured
            rle += str[i]; 
            rle += str[i];   // add two instances of that char to RLE string, denoting run.
            freq.push_back(count);  // populate vector<int> freq with count of repeated char
        } 
        else {
            rle += str[i]; // if no run occured then add 1 instance of that char
        }
    }
    // feed resulting string into threadData object rleData
    rleData->rleStr = rle;
    for (int i=0; i<freq.size(); i++) 
        rleData->rleFreq.push_back(freq[i]); 

    return NULL;
}


int main() {
    vector<string> strVect;
    string temp;
    // read in the input strings from user into vector of strings
    while(cin >> temp) {
        strVect.push_back(temp);
    };

//everything following is influenced by Dr. Rincon's thread example programs provided in CANVAS

    //declaring dynamic for input string x. and POSIX threads.

    static threadData* x = new threadData[strVect.size()]; 
    pthread_t* tid = new pthread_t[strVect.size()];
    // vector<threadData> x;
    // vector<pthread_t> tid;
    // static struct threadData x[100];
    // pthread_t tid[100];


    	for (int i = 0; i < strVect.size(); i++) {
            x[i].inputStr = strVect[i];
            cout << x[i].inputStr << endl;
             //populate threadData array x with hardcode strArray
            if (pthread_create(&tid[i], NULL, rle_encode, &x[i])) {  //calling algorithm but also halting if evaluated as TRUE.
                cerr << "Error creating thread" << endl;
                return 1;
            }
	//	pthread_join(tid[i], NULL);
	}
	
	// Wait for the other threads to finish. Edited
	for (int i = 0; i < strVect.size(); i++)
        	pthread_join(tid[i], NULL);
	for (int i = 0; i < strVect.size(); i++) {  // the NTHREADS - 1 is only because in moodle i need it. 
        cout << "Input string: " << x[i].inputStr << endl;
        cout << "RLE String: " << x[i].rleStr << endl;
        cout << "RLE Frequencies: ";
        for(int j = 0; j < x[i].rleFreq.size(); j++)  // iterate thru appropriate threadData rleFreq int vector and print
            cout << x[i].rleFreq[j] << " "; 
        cout << endl << endl;
    }

    // deallocate memory
    delete[] x;
    delete[] tid;
	return 0;
}