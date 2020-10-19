# 2 Back to College
## Usage
Compilation and running:
```bash
$ gcc q2.c -pthread -o q2 && ./q2
```
After running prompts will appear for input for no. of pharmas, zones and students and probability percentage for each pharma.

## Overview
We make 3 thread arrays, for pharma, zone, and student. \
We make mutex locks for each zone's slot counter, no. of vaccines in each zone and for student wait counter.
First we start the threads for each pharma , zone and student.
Initially the pharma produces batches,
then tries to lock the mutex for no. of vaccines for each zone, once locked ,the batch is dispatched is no. of vaccines value is given.
meanwhile we ' ve kept 
```c
while(vaccinesinzone[in] == 0);           //wait for vaccines to arrive
```
so the process in zones continue once they recieve vaccines. \
The pharma continues like this looking for unlocked locks (free zones) for dispatching its batches till its stock is complete. \
Once the stock is complete, it prepares and starts over again. \
Initially the zone thread starts with locked mutex of no. of slots is locked \
i.e we havent started allocation of slots yet. \
once we recieve vaccines, the lock for slots is unlocked and allocation begins. \
```c
 while((slots[in]!=0 && student_wait)|| slots[in]==s){ 				       //wait till all slots get occupied or till no students are waiting with atleast one slot occupied
	    	if(!slots[in]) break;
	    	sleep(6);												//if slots remain
	    	if(student_wait==0)                                    //waits for students till 5secs, if no one turns up proceed;
	    		break;
	    }
```
the allocation goes on till this while loop breaks, which happens when either:
* all slots are filled
* no students are waiting outside for 6 seconds (i.e it waits for 6 secs for anyone to come before going into vaccination phase)
* exception: it only goes into vaccination phase if atleast one slot is filled

After wich the mutex for slot counter is locked (i.e allocation stops ) and flag for vaccination vac_start[i] is set true \
theres a while(!vac_start[i]); in student thread which waits till the flag is true after which vaccination starts for student in ith zone.
we increment no. of vaccinations taken by student \
and see whether it was successful or not.
```c
int success = rand()%101;                       // generate random number to determine success or failure
vaccinated++;
sleep(0.2);
if (success<=success_per[zonepharma[i]]){       //comparing random number with probability whether success or not
  printf("Vaccination of student[%d] is successfull :), he/she can attend college :)\n",id);	
  vac_success=1;
}
else printf("Vaccination of student[%d] is unsuccessfull :(, %d trials left\n",id, 3-vaccinated);
```
The probabilty implementation is done like this:
suppose if success rate is 80% (say) \
this pick random no. between 0 to 100. Then:
* The no. to be in 0 - 80 has 80% chance --> so let them give success
* The no. to be in 80 - 100 has 20% chance --> so let them give failure

If success break, from loop and thread dies, \
if unsuccessful continue in loop while(no_of_vaccinations < 3) (i.e till he get succesful vaccinations or fails 3 times) \
After which the simulation ends.


`P.S :All the necessary explanation for varibles and functions have also been made in comments. `
