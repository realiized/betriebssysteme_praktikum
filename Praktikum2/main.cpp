#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <exception>

using namespace std;

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
          }
     }
     else {                                  /* Vaterprozess    */
          while (wait(&status) != pid);       /* warte auf beendigung */

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
     }
     else {                                  /* Vaterprozess    */
          while (wait(&status) != pid);       /* warte auf beendigung */

     }
     return pid;
}


int main()
{
    bool end = false;
    char  input[1024];
    char  *args[64];

    cout << "Welcome to the shell" << endl;

    while(!end){
        cout << "MyShell > ";
        gets(input);
        int cmdCount = parseCommand(input, args);
        if (strcmp(args[0], "logout") == 0){
            cout << "Wollen sie wirklich beenden? Y/N" << endl;
            gets(input);
            parseCommand(input, args);
            if(strcmp(args[0], "Y") == 0){
                return 0;
            }
        }

        string last(args[cmdCount-1]);

        if(last == "&"){
            args[cmdCount-1] = "";
            cout << "New Process started with PID " << execute(args, cmdCount) << endl;
        }
        executeWait(args);
    }
    return 0;
}
