#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main()
{
    auto pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        cout << "CHILD: pid[" << getpid() << "] parent[" << getppid() 
             << "]\nGive me a number to return: ";
        int value;
        cin >> value;
        exit(value);
    }
    else
    {
        cout << "PARENT: pid[" << getpid() << "] child[" << pid << "]\n";
        int value;
        if (wait(&value) == -1)
        {
            cout << "wait() failed\n";
        }
        else
        {
            cout << "child exit status: " << WEXITSTATUS(value) << '\n';
        }
    }

    return 0;
}