#include <iostream>
extern "C"
{
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
}

#include <wsh_command.h>
#include <wsh_environment.h>

void sigint_handler(int signo)
{
    exit(0);
}

int main(int argc, char** argv, char** envp)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGINT, sigint_handler);

    std::cout << ">> ";

    char input = 0;

    wsh::Command command;
    wsh::Environment env(envp);

    while( input != EOF )
    {
        input = getchar();

        if ( input == '\n' )
        {
            //@TODO: implement command history here
            command.execute(env);
            command.clear();
            std::cout << ">> ";
        }
        else
        {
            command.append(input);
        }
    }

    return 0;
}
