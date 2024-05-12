#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main()
{
    int descriptors[2];
    if (pipe(descriptors) == -1)
    {
        perror("pipe()");
        return 1;
    }

    if (fork() == 0)
    {
        cout << "CHILD: writing to pipe, using descriptor " << descriptors[1];
        write(descriptors[1], "alamakota", 10);
        cout << "\nCHILD: finished\n";
        exit(0);
    }
    else
    {
        cout << "PARENT: reading from pipe, using descriptor " << descriptors[0];
        string msg(10, '\0');
        read(descriptors[0], msg.data(), 10);
        cout << "\nPARENT: read [" << msg << "], finished\n";
        wait(nullptr);
    }

    return 0;
}