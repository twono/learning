#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
using namespace std;
using namespace std::chrono_literals;

union semun
{
    int val;
    semid_ds* buf;
    ushort* array;
    seminfo* __buf;
};

int init_semaphore(key_t key, int how_many)
{
    int semaphore_id = semget(key, how_many, IPC_CREAT | IPC_EXCL | 0666);
    if (semaphore_id >= 0)  // we got the semaphore
    {
        sembuf buf{.sem_num = 0, .sem_op = 1, .sem_flg = 0};
        for (buf.sem_num = 0; buf.sem_num < how_many; ++buf.sem_num)
        {
            // semop to "free" the semaphores, this sets sem_otime
            if (semop(semaphore_id, &buf, 1) == -1)
            {
                const int old_errno{errno};
                semctl(semaphore_id, 0, IPC_RMID);
                errno = old_errno;
                return -1;
            }
        }
    } 
    else if (errno == EEXIST) // someone else got the semaphore
    {
        // get the id of the existing semaphore
        if (semaphore_id = semget(key, how_many, 0); semaphore_id < 0)
        {
            return semaphore_id;
        }

        bool ready{false};
        semid_ds buf;
        semun data{.buf = &buf};

        // wait for the semaphore creator to initialise
        for (int i = 0; i < 10 && !ready; ++i)
        {
            semctl(semaphore_id, how_many - 1, IPC_STAT, data);

            if (data.buf->sem_otime != 0) ready = true;
            else this_thread::sleep_for(1s);
        }

        if (!ready)
        {
            errno = ETIME;
            return -1;
        }
    }
    
    return semaphore_id;
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

    cout << "Type a string to lock: ";
    string s;
    cin >> s;
    cout << "Trying to lock...\n";

    sembuf buf{ .sem_num = 0, .sem_op = -1, .sem_flg = SEM_UNDO };
    if (semop(semaphore_id, &buf, 1) == -1)
    {
        perror("semop(lock)");
        cout << "errno = " << errno << '\n';
        return 1;
    }

    cout << "Locked. Type a string to unlock: ";
    cin >> s;

    buf.sem_op = 1;
    if (semop(semaphore_id, &buf, 1) == -1)
    {
        perror("semop(unlock)");
        return 1;
    }

    cout << "Unlocked\n";

    return 0;
}