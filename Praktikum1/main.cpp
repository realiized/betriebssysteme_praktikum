#include <iostream>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>

using namespace std;

int global1 = 1;
int global2 = 2;
int global3 = 3;

void getStackLimits()
{
    struct rlimit rlim;

    getrlimit(RLIMIT_STACK, &rlim);

    int soft;                /* Soft limit */
    int hard;                /* Hard limit */
    soft = rlim.rlim_cur;
    hard = rlim.rlim_max;
    printf("Hard limit of Stack = 0x%x \nSoft limit of Stack = 0x%x \n", hard, soft);
}

void getHeapLimits()
{
    struct rlimit rlim;

    getrlimit(RLIMIT_DATA, &rlim);

    int soft;                /* Soft limit */
    int hard;                /* Hard limit */
    soft = rlim.rlim_cur;
    hard = rlim.rlim_max;
    printf("Hard limit of Heap = 0x%x \nSoft limit of Heap = 0x%x \n", hard, soft);
}

void function()
{
    // Stack
    int stack0 = 5;
    int stack1 = 4;
    int stack2 = 17568;

    // Heap
    int* heap0 = new int(5);
    int* heap1 = new int(4);
    int* heap2 = new int(17568);

    printf("Function: Stack = %p; %p; %p \n", &stack0, &stack1, &stack2);
    printf("Function:  Heap = %p; %p; %p \n", heap0, heap1, heap2);

}



int main()
{
    // Limits
    getStackLimits();
    getHeapLimits();

    //Globals
    printf("Globals = %p; %p; %p \n", &global1, &global2, &global3);

    //Functions
    printf("Functions = Main: %p; Function: %p \n", &main, &function);

    // Stack
    int stack0 = 5;
    int stack1 = 4;
    int stack2 = 17568;    

    // Heap
    int* heap0 = new int(5);
    int* heap1 = new int(4);
    int* heap2 = new int(17568);

    printf("Main: Stack = %p; %p; %p \n", &stack0, &stack1, &stack2);
    printf("Main:  Heap = %p; %p; %p \n", heap0, heap1, heap2);

    printf("\n");
    function();
    // erneut aufrufen und man sieht, dass der stack die gleichen adressen verwendet (nach letzter Durchführung gelöscht)
    printf("\n");
    function();

    //printf("%i",sizeof(int)); Size von allen locals zusammenzählen um zu wissen wie viel speicher allokiert wird

    return 0;
}

/*
Erste Ausgabe
Hard limit of Stack = 0xffffffff
Soft limit of Stack = 0x800000
Hard limit of Heap = 0xffffffff
Soft limit of Heap = 0xffffffff
Globals = 0x804a030; 0x804a034; 0x804a038
Functions = Main: 0x8048775; Function: 0x80486c3
Main: Stack = 0xbff8cd28; 0xbff8cd2c; 0xbff8cd30
Main:  Heap = 0x815a008; 0x815a018; 0x815a028
Function: Stack = 0xbff8cce8; 0xbff8ccec; 0xbff8ccf0
Function:  Heap = 0x815a038; 0x815a048; 0x815a058

Zweite Ausgabe
Hard limit of Stack = 0xffffffff
Soft limit of Stack = 0x800000
Hard limit of Heap = 0xffffffff
Soft limit of Heap = 0xffffffff
Globals = 0x804a030; 0x804a034; 0x804a038
Functions = Main: 0x8048775; Function: 0x80486c3
Main: Stack = 0xbfa8cbf8; 0xbfa8cbfc; 0xbfa8cc00
Main:  Heap = 0x8da3008; 0x8da3018; 0x8da3028
Function: Stack = 0xbfa8cbb8; 0xbfa8cbbc; 0xbfa8cbc0
Function:  Heap = 0x8da3038; 0x8da3048; 0x8da3058

Dritte Ausgabe
Hard limit of Stack = 0xffffffff
Soft limit of Stack = 0x800000
Hard limit of Heap = 0xffffffff
Soft limit of Heap = 0xffffffff
Globals = 0x804a030; 0x804a034; 0x804a038
Functions = Main: 0x8048775; Function: 0x80486c3
Main: Stack = 0xbfc928d8; 0xbfc928dc; 0xbfc928e0
Main:  Heap = 0x9180008; 0x9180018; 0x9180028
Function: Stack = 0xbfc92898; 0xbfc9289c; 0xbfc928a0
Function:  Heap = 0x9180038; 0x9180048; 0x9180058

*/

