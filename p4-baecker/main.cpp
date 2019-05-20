#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

/* Options and Defaults */
struct Options {
    // How many breads per interval r
    int b = 1;
    // Max length of queue
    int l = 5;
    // Number of customers
    int m = 4;
    // Number of Employees
    int n = 1;
    // interval of baking bread
    int r = 1;
} options; // name of variable!

bool end = false;
int readyBreads = 0;

struct Baker {

} baker;

void * bake(void* v){
    while(!end){
        readyBreads += options.b;
        sleep(options.r);
    }
}

void throwError(string msg){
    cout << msg << endl << flush;
    exit(EXIT_FAILURE);
}

void setParams(int argc, char *args[])
{
    // i = 0 is programm call
    // all arguments as pairs ( -m 5 -l 9 ...)
    for (int i = 1; i < argc-1; i+=2) {
        char * opt = args[i];
        char * valC = args[i+1];
        int val = atoi(valC);
        if (val == 0) { // not a digit OR just 0. Both fail
            string m = "wrong Input Param - invalid Number: ";
            m += opt;
            m += " ";
            m += valC;
            throwError(m);
        } // else
        if (opt[0] == '-') { // is option
            switch (opt[1]) { // which option?
            case 'b':
                options.b = val; // string to int
                break;
            case 'l':
                options.l = val; // string to int
                break;
            case 'm':
                options.m = val; // string to int
                break;
            case 'n':
                options.n = val; // string to int
                break;
            case 'r':
                options.r = val; // string to int
                break;
            default: // Error wrong input
                string  m = "wrong Input Param - Unknown Option: ";
                m += opt;
                m += " ";
                m += val;
                throwError(m);
                break;
            }
        } else {
            string  m = "wrong Input Param - Not an Option: ";
            m += opt;
            m += " ";
            m += val;
            throwError(m);
        }
    }
}

void printOptions(){
    cout << "b : " << options.b << endl;
    cout << "m : " << options.m << endl;
    cout << "n : " << options.n << endl;
    cout << "l : " << options.l << endl;
    cout << "r : " << options.r << endl;
    cout << flush;
}

void * printBreads(void* v){
    while(!end) {
        cout << "readyBreads : " << readyBreads << endl << flush;
        sleep(1);
    }
}

int main(int argc, char *args[])
{
    setParams(argc, args);
    printOptions();

    pthread_t thBaker;
    pthread_t thPrinter;

    // start baker-thread
    pthread_create(&thBaker, NULL, bake, (void*)NULL);

    // Start Logging Thread
    pthread_create(&thPrinter, NULL, printBreads, (void*)NULL);

    sleep (20);

    return 0;
}

