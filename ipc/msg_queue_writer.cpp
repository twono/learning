#include <iostream>
#include <string>
#include <cstring>
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
    const int key_seed{13};

    const key_t key{ftok("msg_queue_writer.cpp", key_seed)};
    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    const int msg_queue_id{msgget(key, IPC_CREAT | 0644)};
    if (msg_queue_id == -1)
    {
        perror("msgget");
        return 1;
    }

    cout << "Enter text with new lines, Ctrl+D to stop:\n";

    msg_t msg{.type = 1};   // doesn't matter, as long as > 0
    while (fgets(msg.text, sizeof(msg.text), stdin) != nullptr)
    {
        int length = strlen(msg.text);
        if (msg.text[length-1] == '\n') msg.text[length-1] = '\0';

        if (msgsnd(msg_queue_id, &msg, length, 0) == -1)
        {
            perror("msgsnd");
        }
    }

    if (msgctl(msg_queue_id, IPC_RMID, nullptr) == -1)
    {
        perror("msgctl(IPC_RMID)");
        return 1;
    }

    return 0;
}