#include <cstring>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
using namespace std;

constexpr int MEMORY_SIZE{1024};

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        cerr << "<shared_memory> to read, <shared_memory> <text> to write text\n";
        return 1;
    }

    const int key_seed{17};
    const key_t key{ftok("shared_memory.cpp", key_seed)};
    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    const int memory_segment_id{shmget(key, MEMORY_SIZE, 0644 | IPC_CREAT)};
    if (memory_segment_id == -1)
    {
        perror("shmget");
        return 1;
    }

    char* data{static_cast<char*>(shmat(memory_segment_id, nullptr, 0))};
    if (data == (void*) -1)
    {
        perror("shmat");
        return 1;
    }

    if (argc == 1)
    {
        cout << "Data from segment: [" << data << "]\n";
    }
    else 
    {
        cout << "Writing to segment: [" << argv[1] << "]\n";
        strncpy(data, argv[1], MEMORY_SIZE);
        data[MEMORY_SIZE-1] = '\0';
    }

    if (shmdt(data) == -1)
    {
        perror("shmdt");
        return 1;
    }

    return 0;
}