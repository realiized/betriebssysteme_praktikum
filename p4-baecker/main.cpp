#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <queue>
#include <mutex>

using namespace std;

/* Options and Defaults */
struct Options {
    // How many breads per interval r
    int b = 10;
    // interval of baking bread
    int r = 1;
    // Max length of queue
    int l = 5;
    // Max number of breads / customer
    int x = 10;
    // Number of customers
    int m = 20;
    // Max time of walking
    int s = 10;
    // Number of Employees
    int n = 3;
} options; // name of variable!

struct Customer {
    int id = 0;
    int wantsBread = 1;
    int gotBread = 0;
    sem_t served;
};

bool bakeryClosed = false;

mutex mutexCout;
void mCout(string toOutput){
    mutexCout.lock();
    cout << toOutput << endl << flush;
    mutexCout.unlock();
}

// init in main
sem_t customersWaiting;

queue<Customer*> customers;
mutex dequeCust;
Customer * dequeCustomer(){
    sem_wait(&customersWaiting);
    dequeCust.lock();
    Customer * c =  customers.front();
    customers.pop();
    dequeCust.unlock();
    return c;
}

void enqueCustomer(Customer * c){
    dequeCust.lock();
    customers.push(c);
    sem_post(&customersWaiting);
    dequeCust.unlock();
    if (c->id>=0) {
        stringstream msg;
        msg << "Customer " << c->id << " enqueued.";
        mCout(msg.str());
    }
    sem_wait(&(c->served));
}

void sendCustomerHome(int eId, Customer * c) {
    if (c->id >= 0) {
        sem_post(&(c->served));
        stringstream msg;
        msg << "Customer " << c->id << " served by Employee " << eId << ". He wanted " << c->wantsBread << " and got " << c->gotBread << " Breads.";
        mCout(msg.str());
    }
}

// init in main
sem_t readyBreads;

// BAKER-THREAD
void * bake (void* v){
    while(!bakeryClosed){
        for (int i = 0; i < options.b; ++i) {
            sem_post(&readyBreads);
        }
        stringstream msg;
        msg << "Baker baked " << options.b << " breads.";
        mCout(msg.str());
        sleep(options.r);
    }

    void * thReturn;
    pthread_exit(thReturn);
}

// EMPLOYEE-THREAD
void * work (void* v){
   int id = (int) v;
   while(!bakeryClosed){
       sem_wait(&readyBreads); // Don't get next customer as long as no bread is there
       // Get next Customer. Waiting if no Customer in Queue
       Customer * c = dequeCustomer();
       c->gotBread = 1; // First is already taken by employee

       for (int i = 1; i < c->wantsBread; ++i) {
           if (sem_trywait(&readyBreads) == 0) {
               c->gotBread += 1;
           } else {
               break;
           }
       }
       sendCustomerHome(id, c);
   }

   void * thReturn;
   pthread_exit(thReturn);
}

int semGetValue(sem_t * sem){
    int val;
    sem_getvalue(sem, &val);
    return val;
}

// CUSTOMER-THREAD
void * doCustomerThings (void* v){
    Customer * c = new Customer;
    c->id = (int) v;
    c->wantsBread = (rand() % options.x) +1;
    sem_init(&(c->served),0,0);
    while(!bakeryClosed && customers.size() >= options.l || semGetValue(&readyBreads) <= 0){
        // No Breads Available. Don't Enque. Go for a Walk.
        int randSec = rand() % options.s;
        stringstream msg;
        msg << "Customer " << c->id  << " goes for a walk for " << randSec << " Seconds.";
        mCout(msg.str());
        sleep(randSec);
    }
    if (!bakeryClosed) {
        enqueCustomer(c);
    }
    void * thReturn;
    pthread_exit(thReturn);
}

void throwError(string msg){
    mCout(msg);
    bakeryClosed = true;
    exit(EXIT_FAILURE);
}

void setParams(int argc, char *args[])
{
    // i = 0 is programm call
    // all arguments as pairs ( -m 5 -l 9 ...)
    for (int i = 1; i < argc-1; i+=2) {
        char * opt = args[i];
        char * valC = args[i+1];
        int val = atoi(valC); //string to int
        if (opt[0] == '-' && val != 0) { // is option
            switch (opt[1]) { // which option?
            case 'b':
                options.b = val;
                break;
            case 'x':
                options.x = val;
                break;
            case 'l':
                options.l = val;
                break;
            case 'm':
                options.m = val;
                break;
            case 'n':
                options.n = val;
                break;
            case 's':
                options.s = val;
                break;
            case 'r':
                options.r = val;
                break;
            default: // Error wrong input
                stringstream m;
                m << "wrong Input Param - Unknown Option: " << opt << " " << val;
                throwError(m.str());
                break;
            }
        } else {
            stringstream  m;
            m << "Invalid Input Param : " <<opt << " " << valC;
            throwError(m.str());
        }
    }
}

void printOptions(){
    stringstream msg;
    msg << "Options:\nb : " << options.b
        <<"\nm : " << options.m
        <<"\nx : " << options.x
        <<"\nn : " << options.n
        <<"\ns : " << options.s
        <<"\nl : " << options.l
        <<"\nr : " << options.r;
    mCout(msg.str());
}

int main(int argc, char *args[])
{
    setParams(argc, args);
    printOptions();

    sem_init(&readyBreads, 0, 0);
    sem_init(&customersWaiting, 0, 0);

    pthread_t thBaker;
    pthread_t thEmployee [options.n];
    pthread_t thCustomer [options.m];


    // start baker-thread
    pthread_create(&thBaker, NULL, bake, (void*)NULL);
    for(int i = 0; i < options.n; i++){
        pthread_create(&thEmployee[i], NULL, work, (void*)i);
    }

    for(int i = 0; i < options.m; i++){
        pthread_create(&thCustomer[i], NULL, doCustomerThings, (void*)i);
    }

    // Tidy up. Close bakery after all Customers are served.
    void * thReturn;
    for(int i = 0; i < options.m; i++){
        pthread_join(thCustomer[i],&thReturn);
    }

    mCout("Joined Customer Threads");

    bakeryClosed = true;

    for(int i = 0; i < options.n; i++){
        // Enque Pseudo Customers to end Employees
        Customer c;
        c.wantsBread = 0;
        c.id = -1;
        enqueCustomer(&c);
        // Bake Pseudo-Breads
        sem_post(&readyBreads);
    }
    for(int i = 0; i < options.n; i++){
        pthread_join(thEmployee[i],&thReturn);
    }

    mCout("Joined Employee Threads");

    pthread_join(thBaker,&thReturn);

    mCout("Joined Baker Thread");

    return 0;
}

