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
bool end = false;
bool flagLogout = false;
//-VARIABLES

//SIG-Handler
void handle_SIGINT(int signum){
    printf("SIGINT %d", fgProcess);
    if(fgProcess > 0){
        kill(fgProcess, SIGINT);
        waitpid(fgProcess, &status, WUNTRACED);
        fgProcess = 0;
    }
    else{
        flagLogout = true;
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
    pid_t tmpPID = waitpid(-1, &status, WNOHANG);
    for (int i = 0; i < hgProcesses.size(); i++) {
        if (hgProcesses.at(i) == tmpPID) {
            hgProcesses.erase(hgProcesses.begin() + i);
            break;
        }
    }
}

void initSIG(){
    //LOAD
    signal(SIGINT, handle_SIGINT);
    printf("Signal Handler for CTRL+C loaded!\n");
    signal(SIGTSTP, handle_SIGTSTP);
    printf("Signal Handler for CTRL+Z loaded!\n");
    signal(SIGCHLD, handle_SIGCHLD);
    printf("Signal Handler for SIGCHLD loaded!\n");
    //-LOAD
}
// -SIG-Handler

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

string inputString;
char * input;
int cinInput(char **args){
    getline(cin, inputString);
    if (inputString.size() == 0) {
        return 0;
    }
    input = new char[inputString.size()];
    strcpy(input, inputString.c_str());
    int count = parseCommand(input, args);
    return count;
}

void logout(){
    flagLogout = false;
    char  *args[64];
    cout << "Wollen sie wirklich beenden? Y/N" << endl;
    int cmdCount = cinInput(args);
    if(cmdCount != 0 && strcmp(args[0], "Y") == 0){
        if(stoppedProcesses.size() > 0 || hgProcesses.size() > 0){
            printf("There are still running processes in the background!\n");
        }
        else{
            end = true;
        }
    }
}

void fg(){
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

void bg(){
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

pid_t forkExec(char ** args){
    pid_t pid;
    if ((pid = fork()) < 0) { // Error
         printf("Fork  failed!\n");
    }
    else if (pid == 0) { // Child
         if (execvp(*args, args) < 0) {
            printf("Execution failed!\n");
         }
         end = true;
    } else {
        setpgid(pid, pid);
    }
    return pid;
}

void executeWait(char **args)
{
     pid_t pid;

     pid = forkExec(args);
     if (pid > 0) { // success & Vaterprozess    */
          fgProcess = pid;
          waitpid(pid, &status, WUNTRACED);       /* warte auf beendigung */
     }
}

int execute(char **args)
{
     pid_t pid;

     pid = forkExec(args);
     if(pid > 0){
         hgProcesses.push_back(pid);
     }
     return pid;
}

int main()
{
    char  *args[64];

    initSIG();

    cout << "Welcome to the shell V3" << endl;

    while(!end){
        /*
         * If SIGINT (CTRL+C) set the logout-Flag to true, the process gets started.
         * Implemented like this to not stay in SIG-Handler to long.
         */
        if(flagLogout){
            logout();
            continue;
        }

        cout << "MyShell > ";
        int cmdCount = cinInput(args);

        if(cmdCount == 0){ // empty input
            continue;
        } // else

        string last(args[cmdCount-1]);

        if (strcmp(args[0], "logout") == 0){
            logout();
        }
        else if(strcmp(args[0], "fg") == 0){
            fg();
        }
        else if(strcmp(args[0], "bg") == 0){
            bg();
        }
        else if(last == "&"){
            args[cmdCount-1] = '\0'; // remove &
            pid_t newP = execute(args);
            cout << "New Process started with PID " << newP << endl;
        }
        else{ // sync(fg) exec. normal.
            executeWait(args);
        }
    }

    return 0;
}
