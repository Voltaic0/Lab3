#include "usertraps.h"
#include "misc.h"
//#include "stdio.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
    int i;
    int numprocs = 0;
    char sulfur_two_str[10];
    char carbon_oxy_str[10];
    char sulfur_str[10];
    char oxy_two_str[10];
    char carbon_two_str[10];
    char s_procs_completed_str[10];

    mbox_t sulfur_two = mbox_create();
    mbox_t carbon_oxy = mbox_create();
    mbox_t sulfur = mbox_create();
    mbox_t oxy_two = mbox_create();
    mbox_t carbon_two = mbox_create();
    sem_t s_procs_completed;

    int sulfur2 = 0;
    int carbon_o = 0;
    int equation1;
    int equation2;
    int equation3;

    if (argc != 3) {
        Printf("Usage: "); Printf(argv[0]); Printf(" Need to specify number of times to inject the Molecules.\n");
        Exit();
    }

    sulfur2 = dstrtol(argv[1], NULL, 10); // the "10" means base 10
    carbon_o = dstrtol(argv[2], NULL, 10);

    equation1 = sulfur2;
    equation2 = carbon_o /4;
    if(equation2>(equation1*2)){
        equation3 = equation1 *2;
    }
    else{
        equation3 = equation2;
    }
    numprocs = equation1 + equation2 + equation3 + sulfur2 + carbon_o;
    //OPEN ALL MAILBOXES
    if(!mbox_open(sulfur_two)){
        Printf("Error Opening sulfur_two mailbox.\n");
        Exit();
    }
    if(!mbox_open(carbon_oxy)){
        Printf("Error Opening carbon_oxy mailbox.\n");
        Exit();
    }
    if(!mbox_open(sulfur)){
        Printf("Error Opening sulfur mailbox.\n");
        Exit();
    }
    if(!mbox_open(oxy_two)){
        Printf("Error Opening oxy_two mailbox.\n");
        Exit();
    }
    if(!mbox_open(carbon_two)){
        Printf("Error Opening carbon_two mailbox.\n");
        Exit();
    }

    if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
        Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
        Exit();
    }

    //TRANSLATE MAILBOX HANDLES INTO STRINGS TO PASS TO FUNCTIONS
    ditoa(s_procs_completed, s_procs_completed_str);
    ditoa(sulfur_two, sulfur_two_str);
    ditoa(carbon_oxy, carbon_oxy_str);
    ditoa(sulfur, sulfur_str);
    ditoa(oxy_two, oxy_two_str);
    ditoa(carbon_two, carbon_two_str);


    for(i=0; i< 15; i++){
        if(i< sulfur2){
            process_create(FILENAME_TO_RUN, 0, 1, s_procs_completed_str, sulfur_two_str, NULL); // pro
        }
        if(i<carbon_o){
            process_create(FILENAME_TO_RUN2, 0, 0, s_procs_completed_str, carbon_oxy_str, NULL); // pro
        }
        if(i<equation1){
            process_create(FILENAME_TO_RUN3, 0, 0, s_procs_completed_str, sulfur_two_str, sulfur_str, NULL);
        }
        if(i<equation2){
            process_create(FILENAME_TO_RUN4, 0, 0, s_procs_completed_str, carbon_oxy_str, oxy_two_str, NULL);
        }
        if(i<equation3){
            process_create(FILENAME_TO_RUN5, 0, 0, s_procs_completed_str, sulfur_str, oxy_two_str, NULL);
        }

    }

    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
        Exit();
    }
    Printf("%d S leftover, %d O2 Leftover, %d C2 leftover, %d SO4 Created.\n", 2*equation1-equation3, 2*(equation2-equation3),2*equation2, equation3);
    if(!mbox_close(sulfur_two)){
        Printf("Error Opening sulfur_two mailbox.\n");
        Exit();
    }
    if(!mbox_close(carbon_oxy)){
        Printf("Error Opening carbon_oxy mailbox.\n");
        Exit();
    }
    if(!mbox_close(sulfur)){
        Printf("Error Opening sulfur mailbox.\n");
        Exit();
    }
    if(!mbox_close(oxy_two)){
        Printf("Error Opening oxy_two mailbox.\n");
        Exit();
    }
    if(!mbox_close(carbon_two)){
        Printf("Error Opening carbon_two mailbox.\n");
        Exit();
    }

}
