#include <iostream>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <exception>

using namespace std;

//Nur wenn im Vordergrund ein  Prozess läuft soll CTRL C und Z auf diesen wirken

//VARIABLES
pid_t vgProcess;
vector<pid_t> allProcesses;
//-VARIABLES

void handle_SIGINT(int signum){
    printf("SIGINT");
}

void handle_SIGTSTP(int signum){
    printf("SIGSTP");
}

int parseCommand(char *input, char **args)
{
    int cmdCount = 0;
    while (*input != '\0') {       /* wenn nicht ende des inputs */
          while (*input == ' ' || *input == '\t' || *input == '\n')
               *input++ = '\0';     /* space mit 0 füllen    */
          *args++ = input;          /* speichern der argumentlocation    */
          while (*input != '\0' && *input != ' ' &&
                 *input != '\t' && *input != '\n')
               input++;             /* überspringe args bis..*/

          cmdCount++;
     }
     *args = '\0';                 /*Ende der args Liste*/
    return cmdCount;
}

void  executeWait(char **args)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {
          printf("Fork  failed!\n");
     }
     else if (pid == 0) {
          if (execvp(*args, args) < 0) {
               printf("Execution failed!\n");
               exit(0);
          }
     }
     else {                                  /* Vaterprozess    */
          waitpid(pid, &status, WUNTRACED);       /* warte auf beendigung */

     }
}

int  execute(char **args, int cmdCount)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {
          printf("Fork  failed!\n");
     }
     else if (pid == 0) {
          if (execvp(*args, args) < 0) {
               printf("Execution failed!\n");
          }
          exit(0);
     }
     return pid;
}


int main()
{
    bool end = false;
    string inputString;
    char  *args[64];

    //LOAD
    signal(SIGINT, handle_SIGINT);
    printf("Signal Handler for CTRL+C loaded!\n");
    signal(SIGTSTP, handle_SIGTSTP);
    printf("Signal Handler for CTRL+Z loaded!\n");
    //-LOAD

    cout << "Welcome to the shell" << endl;

    while(!end){
        cout << "MyShell > ";
        getline(cin, inputString);
        char input[inputString.size()];
        strcpy(input, inputString.c_str());
        cout << input << endl;
        int cmdCount = parseCommand(input, args);
        if (strcmp(args[0], "logout") == 0){
            cout << "Wollen sie wirklich beenden? Y/N" << endl;
            getline(cin, inputString);
            strcpy(input, inputString.c_str());
            parseCommand(input, args);
            if(strcmp(args[0], "Y") == 0){
                return 0;
            }
        }

        string last(args[cmdCount-1]);

        if(last == "&"){
            args[cmdCount-1] = '\0';
            allProcesses.push_back(execute(args, cmdCount));
            cout << "New Process started with PID " << allProcesses[allProcesses.size() - 1] << endl;
        }
        else{
            executeWait(args);
        }
    }
    return 0;
}