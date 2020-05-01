# Lab3
To test the mailboxes and q2 begin by modifying the makefile in apps/q2/ to specify the number of S2 and CO molecules, currently set at 2 and 8 that need to be injected. After run make in the OS folder and then make in apps/q2 then make run to test the code

Q3 can be checked by modifying the The makeprocs.c file in apps/q2/makeprocs and changing the second zero in the lines calling 
process_create to 1. This was done for the S2 inject process_create in Q1 to show the functionality of it.

Q4 and Q5 can be tested by running make in apps/prio_test and then make run.
  Currently the first three outputs match the expected output, however Each A labeled process finishes too fast and then the B and C 
  labeled processes complete as expected in the expected output provided.
  
Each time make clean can be run if you wish to clean up the object files created in each test case.
