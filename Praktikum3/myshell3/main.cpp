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
pid_t fgProcess;
vector<pid_t> hgProcesses;
vector<pid_t> stoppedProcesses;
int status;
//-VARIABLES

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

void handle_SIGINT(int signum){
    printf("SIGINT %d", fgProcess);
    if(fgProcess > 0){
        kill(fgProcess, SIGINT);
        waitpid(fgProcess, &status, WUNTRACED);
        fgProcess = 0;
    }
    else{
        string inputString;
        printf("Wirklich beenden? \n");
        cin >> inputString;
        if(inputString == "Y"){
            if(stoppedProcesses.size() > 0 || hgProcesses.size() > 0){
                printf("There are still running processes in the background!\n");
            }
            else{
                exit(0);
            }
        }
    }
}

void handle_SIGTSTP(int signum){
    printf("SIGSTP");
    if(fgProcess > 0){
        stoppedProcesses.push_back(fgProcess);
        kill(fgProcess, SIGTSTP);
        printf("The size of the stopped Processes: %d \n",stoppedProcesses.size());
        fgProcess = 0;
    }
    else{
        printf("No process to stop active! \n");
    }
}

void handle_SIGCHLD(int signum){
    waitpid(-1, &status, WNOHANG);
}

void executeWait(char **args)
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
          fgProcess = pid;
          setpgid(pid, pid);
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
     setpgid(pid, pid);
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
    signal(SIGCHLD, handle_SIGCHLD);
    printf("Signal Handler for SIGCHLD loaded!\n");
    //-LOAD

    cout << "Welcome to the shell V2" << endl;

    while(!end){
        cout << "MyShell > ";
        getline(cin, inputString);
        char input[inputString.size()];
        strcpy(input, inputString.c_str());
        cout << input << endl;
        int cmdCount = parseCommand(input, args);

        string last(args[cmdCount-1]);


        if (strcmp(args[0], "logout") == 0){
            cout << "Wollen sie wirklich beenden? Y/N\n" << endl;
            getline(cin, inputString);
            strcpy(input, inputString.c_str());
            parseCommand(input, args);
            if(strcmp(args[0], "Y") == 0){
                if(stoppedProcesses.size() > 0 || hgProcesses.size() > 0){
                    printf("There are still running processes in the background!\n");
                }
                else{
                    return 0;
                }

            }
        }

        else if(strcmp(args[0], "fg") == 0){
            if(stoppedProcesses.size() > 0){
                kill(stoppedProcesses.at(0), SIGCONT);
                fgProcess = stoppedProcesses.at(0);
                stoppedProcesses.erase(stoppedProcesses.begin());
                waitpid(fgProcess, &status, WUNTRACED);
            }
            else if(hgProcesses.size() > 0){
                fgProcess = hgProcesses.at(0);
                hgProcesses.erase(hgProcesses.begin());
                waitpid(fgProcess, &status, WUNTRACED);
            }
            else{
                printf("There are no processes to continue in the foreground.\n");
            }
        }

        else if(strcmp(args[0], "bg") == 0){
            if(stoppedProcesses.size() > 0){
                pid_t stoppedProcess = stoppedProcesses.at(0);
                kill(stoppedProcess, SIGCONT);
                hgProcesses.push_back(stoppedProcess);
                stoppedProcesses.erase(stoppedProcesses.begin());
            }
            else{
                printf("There are no processes to continue in the background.\n");
            }
        }
        else if(last == "&"){
            args[cmdCount-1] = '\0';
            pid_t newP = execute(args, cmdCount);
            hgProcesses.push_back(newP);
            cout << "New Process started with PID " << newP << endl;
        }
        else{
            executeWait(args);
        }
    }
    return 0;
}
