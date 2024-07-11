#include <iostream>
#include <string>
#include <string_view>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
using namespace std;

int main()
{
    const string fifo_name{"greatly_pointy"};
    if (mknod(fifo_name.c_str(), S_IFIFO | 0666, 0) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mknod");
            return 1;
        }
        
        cout << "FIFO " << fifo_name << " has been created already, fine\n";
    }

    cout << "waiting for someone to start writing from " << fifo_name << '\n';

    const int fifo_fd{open(fifo_name.c_str(), O_RDONLY)};
    if (fifo_fd == -1)
    {
        perror("open()");
        return 1;
    }

    cout << "writer has arrived, start reading...\n";
    string buf(100, '\0');
    for (int bytes = 1; bytes > 0;)
    {
        if (bytes = read(fifo_fd, buf.data(), buf.size());
            bytes == -1)
        {
            perror("read()");
        }
        else if (bytes > 0)
        {
            cout << "read " << bytes << " bytes: [" << string_view(buf.c_str(), bytes) << "]\n";
        }
    }

    if (close(fifo_fd) == -1)
    {
        perror("close()");
    }

    return 0;
}