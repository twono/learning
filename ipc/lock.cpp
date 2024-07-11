#include <iostream>
#include <string>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[])
{
    flock fl{.l_type = F_WRLCK, .l_whence = SEEK_SET, .l_start = 0, .l_len = 0};
    if (argc > 1) fl.l_type = F_RDLCK;
    
    const int descriptor{open("lock.cpp", O_RDWR)};
    if (descriptor == -1)
    {
        perror("open()");
        return 1;
    }

    cout << "Press <Enter> to try to get lock";
    getchar();
    cout << "Trying to get lock...\n";

    if (fcntl(descriptor, F_SETLKW, &fl) == -1)     // blocking
    {
        perror("fcntl(lock)");
        return 1;
    }

    cout << "Got lock, press <Enter> to release";
    getchar();

    fl.l_type = F_UNLCK;

    if (fcntl(descriptor, F_SETLK, &fl) == -1)      // non-blocking
    {
        perror("fcntl(unlock)");
        return 1;
    }

    cout << "Unlocked\n";
    close(descriptor);

    return 0;
}