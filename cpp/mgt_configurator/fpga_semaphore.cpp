#include <unistd.h>
#include <inttypes.h>
#include "sp12_macros.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <map>

int semid; /* semid of semaphore set */
key_t sem_key ; /* key to pass to semget() */
int semflg = IPC_CREAT | 0666; /* semflg to pass to semget() */
int nsems = 1; /* nsems to pass to semget() */
int nsops; /* number of operations to do */
struct sembuf sops[2];
std::map<int,int> fd_semid; // file_descriptor to semaphore ID map


void lock_board(int fd);
void unlock_board(int fd);
/*
void mwrite(uint64_t a, void* b, size_t c, off_t d)
{
    lock_board((int)a);
    if (c == 4)
    {
        c = 8;
        uint64_t b8 = *((uint32_t*)b);
        if (pwrite ((int)a,&b8,c,d) != (ssize_t)c)
            log_printf ("pwrite error: w: %d a: %08x file: %s line: %d\n",
                        c, d, __FILE__, __LINE__);
    }
    else
    {
        if (pwrite ((int)a,b,c,d) != (ssize_t)c)
            log_printf ("pwrite error: w: %d a: %08x file: %s line: %d\n",
                        c, d, __FILE__, __LINE__);
    }
    unlock_board((int)a);
}

void mread(uint64_t a, void* b, size_t c, off_t d)
{
    int fd = (int)( a >> 32);
    lock_board((int)a);
    if (c == 4)
    {
         c = 8;
         uint64_t b8;
         if (pread (fd,&b8,c,d) != (ssize_t)c)
             log_printf ("pread error: w: %d a: %08x file: %s line: %d\n",
                         c, d, __FILE__, __LINE__);
         *((uint32_t*)b) = (uint32_t) b8;
    }
    else
    {
        if (pread (fd,b,c,d) != (ssize_t)c)
            log_printf ("pread error: w: %d a: %08x file: %s line: %d\n",
                        c, d, __FILE__, __LINE__);
    }
    unlock_board((int)a);
}
*/
void create_semaphore (int fd, int dev_ind)
{
    /* generate key */
    if ((sem_key = ftok("semaphore_file.txt", (char)dev_ind)) == -1)
        log_printf("ftok");

    /* set up semaphore */

    log_printf("\nsemget: Setting up semaphore: semget(%#lx, %\
           %#o)\n",sem_key, nsems, semflg);

    if ((semid = semget(sem_key, nsems, semflg)) == -1)
            log_printf("semget: semget failed\n");

    fd_semid[fd] = semid; // store semaphore ID in map
}

void lock_board(int fd)
{
    return;
    int sem_id = fd_semid[fd];

    nsops = 2;

    /* wait for semaphore to reach zero */

    sops[0].sem_num = 0; /* We only use one track */
    sops[0].sem_op = 0; /* wait for semaphore flag to become zero */
    sops[0].sem_flg = SEM_UNDO; /* take off semaphore asynchronous  */


    sops[1].sem_num = 0;
    sops[1].sem_op = 1; /* increment semaphore -- take control of track */
    sops[1].sem_flg = SEM_UNDO | IPC_NOWAIT; /* take off semaphore */

    if ((semop(sem_id, sops, nsops)) == -1)
    {
        log_printf("lock semop: semop failed\n");
    }
}

void unlock_board(int fd)
{
    return;
    int sem_id = fd_semid[fd];
    nsops = 1;

    /* wait for semaphore to reach zero */
    sops[0].sem_num = 0;
    sops[0].sem_op = -1; /* Give UP COntrol of track */
    sops[0].sem_flg = SEM_UNDO | IPC_NOWAIT; /* take off semaphore, asynchronous  */


    if ((semop(sem_id, sops, nsops)) == -1)
    {
        log_printf("unlock semop: semop failed\n");
    }
}
