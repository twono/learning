#include <iostream>
#include <string>
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

    cout << "waiting for someone to start reading from " << fifo_name << '\n';

    const int fifo_fd{open(fifo_name.c_str(), O_WRONLY)};
    if (fifo_fd == -1)
    {
        perror("open()");
        return 1;
    }

    cout << "reader has arrived, start writing...\n";
    string buf(100, '\0');
    int bytes_read;
    while (true)
    {
        bytes_read = read(STDIN_FILENO, buf.data(), buf.size());
        if (bytes_read == -1)
        {
            perror("read()");
        }
        if (bytes_read <= 0)
        {
            break;
        }

        if (int bytes_written = write(fifo_fd, buf.c_str(), bytes_read);
            bytes_written == -1)
        {
            perror("write()");
        }
        else
        {
            cout << "writer wrote " << bytes_written << " bytes\n";
        }
    }

    if (close(fifo_fd) == -1)
    {
        perror("close()");
    }

    return 0;
}