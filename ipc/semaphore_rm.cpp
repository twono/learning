#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

int main()
{
    int key_seed{34};
    key_t key{-1};
    if (key = ftok("semaphore.cpp", key_seed); key == -1)
    {
        perror("ftok");
        return 1;
    }

    int semaphore_id;
    if (semaphore_id = semget(key, 1, 0); semaphore_id == -1)
    {
        perror("semget");
        return 1;
    }

    if (semctl(semaphore_id, 0, IPC_RMID, 0) == -1)
    {
        perror("semctl(IPC_RMID)");
        return 1;
    }

    return 0;
}