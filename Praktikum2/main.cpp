#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <exception>

using namespace std;

vector<string> getWords(string str)
{
    // Used to split string around spaces.
    istringstream ss(str);
    vector<string> words;
    // Traverse through all words
    do {
        // Read a word
        string word;
        ss >> word;

        // Print the read word
        words.push_back(word);

        // While there is more to read
    } while (ss);

    return words;
}


int main()
{
    bool end = false;
    string input = "";
    vector<string>words;
    int child_status;

    cout << "Welcome to the shell" << endl;

    while(!end){
        cout << "MyShell > ";
        getline(cin, input);

        words = getWords(input);

        char* args[words.size()];
        char word[100];
        for(int i = 0; i < words.size(); i++){
            strcpy(word, words.front().c_str());
            args[i] = word;
        }



        pid_t pid = fork();
        if(pid==0){
            execvp(args[0],
                   args);
        }
        else{
            waitpid(pid, NULL, NULL);
        }

    }
    return 0;
}
