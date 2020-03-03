#include "usertraps.h"
#include "misc.h"


void main (int argc, char *argv[])
{      // Used to access missile codes in shared memory page
    sem_t s_procs_completed; // Semaphore to signal the original process that we're done
    mbox_t s_send;
    mbox_t s2_rec;
    int i;
    char recieve[2];
    char send[2];

    //lock_t TheLock;

    if (argc != 4) {
        Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n");
        Exit();
    }
    send[0] = "S";
    // Convert the command-line strings into integers for use as handles
    s_procs_completed = dstrtol(argv[1], NULL, 10);
    s2_rec = dstrtol(argv[2], NULL, 10);
    s_send = dstrtol(argv[3], NULL, 10);



    if(!mbox_open(s2_rec)){
        Printf("Error Opening sulfur_two mailbox.\n");
        Exit();
    }
    mbox_recv(s2_rec, 3, recieve);
    Printf("%s Recieved!\n", recieve);
    mbox_close(s2_rec);
    mbox_open(s_send);
    Printf("S + S Created\n");


    mbox_send(s_send, 1, send);
    mbox_send(s_send, 1, send);
    mbox_close(s_send);

    if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
    }
}
