#include <iostream>
#include <string>
#include <string_view>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
using namespace std;

struct msg_t
{
    long type;
    char text[200];
};

int main()
{
    const int ANY_MSG_TYPE{0};
    const int key_seed{13};

    const key_t key{ftok("msg_queue_writer.cpp", key_seed)};
    if (key == -1)    // same as writer
    {
        perror("ftok");
        return 1;
    }

    const int msg_queue_id{msgget(key, 0644)};
    if (msg_queue_id == -1)       // no IPC_CREAT
    {
        perror("msgget");
        return 1;
    }

    cout << "Ready to receive message\n";

    while (true)
    {
        msg_t msg;
        if (msgrcv(msg_queue_id, &msg, sizeof(msg.text), ANY_MSG_TYPE, 0) == -1)
        {
            perror("msgrcv");
            return 1;
        }

        cout << "Received: [" << msg.text << "]\n";
    }

    return 0;
}