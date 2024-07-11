#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

int main()
{
    const int key_seed{34};
    const key_t key{ftok("semaphore.cpp", key_seed)};
    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    const int semaphore_id{semget(key, 1, 0)};
    if (semaphore_id == -1)
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