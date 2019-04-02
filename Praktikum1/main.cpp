#include <iostream>
#include <stdio.h>

using namespace std;


void function(){
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
    // Stack
    int stack0 = 5;
    int stack1 = 4;
    int stack2 = 17568;

    // Heap
    int* heap0 = new int(5);
    int* heap1 = new int(4);
    int* heap2 = new int(17568);

    printf("    Main: Stack = %p; %p; %p \n", &stack0, &stack1, &stack2);
    printf("    Main:  Heap = %p; %p; %p \n", heap0, heap1, heap2);

    function();
    // erneut aufrufen und man sieht, dass der stack die gleichen adressen verwendet (nach letzter Durchführung gelöscht)
    // function();

    return 0;
}
