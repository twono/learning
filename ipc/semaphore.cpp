#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <iostream>
using namespace std;

int init_semaphore(key_t key, int how_many)
{
    // TODO
    return 2;
}

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
    if (semaphore_id = init_semaphore(key, 1); semaphore_id == -1)
    {
        perror("init_semaphore");
        return 1;
    }

    cout << "Press <Enter> to lock: ";
    char c;
    cin >> c;
    cout << "Trying to lock...\n";

    sembuf buf{ .sem_num = 1, .sem_op = -1, .sem_flg = SEM_UNDO };
    if (semop(semaphore_id, &buf, 1) == -1)
    {
        perror("semop(lock)");
        return 1;
    }

    cout << "Locked. Press <Enter> to unlock: ";
    cin >> c;

    buf.sem_op = 1;
    if (semop(semaphore_id, &buf, 1) == -1)
    {
        perror("semop(unlock)");
        return 1;
    }

    cout << "Unlocked\n";

    return 0;
}