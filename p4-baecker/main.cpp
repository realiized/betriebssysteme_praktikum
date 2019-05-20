#include <iostream>
#include <stdlib.h>

using namespace std;

/* Options and Defaults */
struct Options {
    // How many baker-threads
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
            case 't':
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

/* For Testing Purposes */
void printParams(){
    cout << "b : " << options.b << endl;
    cout << "m : " << options.m << endl;
    cout << "n : " << options.n << endl;
    cout << "l : " << options.l << endl;
    cout << "r : " << options.r << endl;
    cout << flush;
}

int main(int argc, char *args[])
{   
    setParams(argc, args);
    printParams();
    return 0;
}

