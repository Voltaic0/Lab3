#include "usertraps.h"
#include "misc.h"


void main (int argc, char *argv[])
{      // Used to access missile codes in shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mbox_t s2_send;
  char *inject = "S2";
  Printf("Entering s2inject\n");

  //lock_t TheLock;

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf("\n");Printf(argv[1]);Printf("\n");Printf(argv[2]);Printf("\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  s2_send = dstrtol(argv[2], NULL, 10);


    if(!mbox_open(s2_send)){
        Printf("Error Opening sulfur_two mailbox.\n");
        Exit();
    }
    Printf("About to inject\n");
    mbox_send(s2_send, 3, inject);
    Printf("S2 Injected!\n");

    mbox_close(s2_send);
  
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}