#include <iostream>
#include <stdlib.h> // rand


using namespace std;

int main()
{
    int * list = new int(1);
    for (int j = 0; j < 4000000000; ++j) {
        list = new int(*list * rand());
    }
}

/*
 * Use cmd top to see resource usage.
 */
