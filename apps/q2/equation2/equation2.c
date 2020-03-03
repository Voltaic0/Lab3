#include "usertraps.h"
#include "misc.h"


void main (int argc, char *argv[])
{      // Used to access missile codes in shared memory page
    sem_t s_procs_completed; // Semaphore to signal the original process that we're done
    mbox_t o2_send;
    mbox_t co_rec;
    int i;
    char *recieve = "CO";
    char *send = "2 O2";

    //lock_t TheLock;

    if (argc != 4) {
        Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n");
        Exit();
    }
    // Convert the command-line strings into integers for use as handles
    s_procs_completed = dstrtol(argv[1], NULL, 10);
    co_rec = dstrtol(argv[2], NULL, 10);
    o2_send = dstrtol(argv[3], NULL, 10);



    if(!mbox_open(co_rec)){
        Printf("Error Opening sulfur_two mailbox.\n");
        Exit();
    }
    mbox_recv(co_rec, 3, recieve);
    Printf("%s Recieved!\n", recieve);

    mbox_recv(co_rec, 3, recieve);
    Printf("%s Recieved!\n", recieve);

    mbox_recv(co_rec, 3, recieve);
    Printf("%s Recieved!\n", recieve);

    mbox_recv(co_rec, 3, recieve);
    Printf("%s Recieved!\n", recieve);

    mbox_close(co_rec);
    mbox_open(o2_send);
    Printf("2 O2 + 2C2 Created\n");


    mbox_send(o2_send, 4, send);
    mbox_close(o2_send);

    if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
    }
}