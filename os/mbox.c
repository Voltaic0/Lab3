#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

static mbox mboxes[MBOX_NUM_MBOXES];
static mbox_message messages[MBOX_NUM_BUFFERS];

//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
    int i;

    for(i=0;i < MBOX_NUM_MBOXES; i++){
        mboxes[i].inuse = 0;
    }
    for(i=0;i < MBOX_NUM_BUFFERS; i++){
        messages[i].inuse = 0;
    }

}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
    int i;
    mbox_t handle;
    uint32 intrs;

    intrs = DisableIntrs();
    for(i=0; i<MBOX_NUM_MBOXES; i++){
        if(!mboxes[i].inuse){
            handle = i;
            mboxes[i].inuse = 1;
            break;
        }
    }
    RestoreIntrs(intrs);

    if(i == MBOX_NUM_MBOXES){return MBOX_FAIL;}

    if (AQueueInit(&mboxes[handle].msgQ) != QUEUE_SUCCESS) {
        printf("FATAL ERROR: could not initialize mbox msgQ in MBOXCreate!\n");
        exitsim();
    }

    for( i =0; i<32;i++){
        mboxes[handle].procs[i] = 0;
    }
    mboxes[handle].waitFull = SemCreate(0);
    mboxes[handle].waitEmpty = SemCreate(0);
    mboxes[handle].lock = LockCreate();

    return handle;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {

    if(handle < 0 || handle > MBOX_NUM_MBOXES || !mboxes[handle].inuse){return MBOX_FAIL;}

    mboxes[handle].procs[GetCurrentPid()] = 1;


  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
    int i;
    Link *l;
    mbox_message *mess;
    if(!mboxes[handle].inuse){
        return MBOX_FAIL;
    }
    mboxes[handle].procs[GetCurrentPid()] = 0;

    for(i = 0 ; i< 32; i++){
        if(mboxes[handle].procs[i]){
            return MBOX_SUCCESS;
        }
    }
    LockHandleAcquire(mboxes[handle].lock);
    while(!AQueueEmpty(&mboxes[handle].msgQ)){
        l = AQueueFirst(&mboxes[handle].msgQ);
        mess = (mbox_message *)AQueueObject(l);
        mess->inuse = 0;
        AQueueRemove(&l);
    }
    mboxes[handle].inuse = 0;
    LockHandleRelease(mboxes[handle].lock);

    return MBOX_SUCCESS;

}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
    Link *l;
    int i;
    for(i=0;i<MBOX_NUM_BUFFERS;i++){
        if(!messages[i].inuse){
            bcopy(message, messages[i].message, length);
            messages[i].inuse =1;
            messages[i].length = length;
            break;
        }
    }
    if(i == MBOX_NUM_BUFFERS){return MBOX_FAIL;}
    if(length > MBOX_MAX_MESSAGE_LENGTH){return MBOX_FAIL;}

    if(!mboxes[handle].procs[GetCurrentPid()]){return MBOX_FAIL;}


    if(AQueueLength(&mboxes[handle].msgQ)== 10){
        SemHandleWait(mboxes[handle].waitFull);
        if ((l = AQueueAllocLink((void *)&messages[i])) == NULL) {
            printf("FATAL ERROR: could not allocate link for MBOXSEND!\n");
            return MBOX_FAIL;
        }

        if (AQueueInsertLast (&mboxes[handle].msgQ, l) != QUEUE_SUCCESS) {
            printf("FATAL ERROR: could not insert new link into lock waiting queue in MBOXLENGTH!\n");
            return MBOX_FAIL;
        }
    }else{
        if ((l = AQueueAllocLink((void *)&messages[i])) == NULL) {
            printf("FATAL ERROR: could not allocate link for MBOXSEND!\n");
            return MBOX_FAIL;
        }

        if (AQueueInsertLast (&mboxes[handle].msgQ, l) != QUEUE_SUCCESS) {
            printf("FATAL ERROR: could not insert new link into lock waiting queue in MBOXLENGTH!\n");
            return MBOX_FAIL;
        }
        SemHandleSignal(mboxes[handle].waitEmpty);
    }

    return MBOX_SUCCESS;


}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
    Link *l;
    mbox_message *mess;
    if(!mboxes[handle].procs[GetCurrentPid()]){return MBOX_FAIL;}

    if(AQueueEmpty(&mboxes[handle].msgQ)){
        SemHandleWait(mboxes[handle].waitEmpty);
        LockHandleAcquire(mboxes[handle].lock);
        l = AQueueFirst(&mboxes[handle].msgQ);
        mess = (mbox_message *)AQueueObject(l);
        AQueueRemove(&l);
        mess->inuse = 0;
        LockHandleRelease(mboxes[handle].lock);
        if(mess->length > maxlength) {
            return MBOX_FAIL;
        }
        bcopy(mess->message, message, mess->length);

    }
    else{
        l = AQueueFirst(&mboxes[handle].msgQ);
        mess = (mbox_message *)AQueueObject(l);
        if(mess->length > maxlength) {
            return MBOX_FAIL;
        }
        bcopy(mess->message, message, mess->length);
        SemHandleSignal(mboxes[handle].waitFull);
    }
    return mess->length;
}


//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
    int i;
    for(i = 0; i <MBOX_NUM_MBOXES; i++){
        if(mboxes[i].inuse){
            mboxes[i].procs[pid] = 0;
        }
    }

  return MBOX_FAIL;
}
