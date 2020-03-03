#include "usertraps.h"
#include "misc.h"


void main (int argc, char *argv[])
{      // Used to access missile codes in shared memory page
    sem_t s_procs_completed; // Semaphore to signal the original process that we're done
    mbox_t co_send;
    char *inject = "CO";

    //lock_t TheLock;

    if (argc != 3) {
        Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n");
        Exit();
    }

    // Convert the command-line strings into integers for use as handles
    s_procs_completed = dstrtol(argv[1], NULL, 10);
    co_send = dstrtol(argv[2], NULL, 10);



    if(!mbox_open(co_send)){
        Printf("Error Opening sulfur_two mailbox.\n");
        Exit();
    }
    mbox_send(co_send, 3, inject);
    Printf("CO Injected!\n");

    mbox_close(co_send);

    if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
    }
}