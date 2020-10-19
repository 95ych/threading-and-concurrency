#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define fri(s,e) for(int i=s;i<e;i++)
#define min(a,b) ((a) < (b) ? (a) : (b))
#define BUF 1024

int n,m,o;//pharmas , zones and students respectively.
int exitt;
int success_per[BUF], zonepharma[BUF];// success percentage, zonepharma->the pharma id dist. to given zone 
int vaccinesinzone[BUF], slots[BUF];  // vaccines in each zone and slots in each zone
int vac_start[BUF]; // vaccination success or fail for student i;
int student_wait;  // denotes no. of students waiting for slots
pthread_t pharma_th[BUF], zone_th[BUF], student_th[BUF]; // threads for pharma, zone, student
pthread_mutex_t dispatch_to_zone_lock[BUF]; // lock for dispatching vaccines to zone;
pthread_mutex_t student_zone_lock[BUF];   // zone's  number of slots lock 
pthread_mutex_t student_wait_lock =PTHREAD_MUTEX_INITIALIZER; //initialise mutex lock for no. of students waiting counter


void *pharma(void *index){
	int in = *((int*)index);
	while(!exitt){
		int w = rand()%4+2;
        int r = rand()%6+1;
        int p = rand()%21+10;
		sleep(w);             // preparing time w seconds;
		printf("Pharmaceutical company[%d] has prepared %d batches of  %d vaccines each, with success percentage %d%%.\n ", in,r,p,success_per[in]);
		printf("Waiting for all the vaccines to resume production.\n");
		int zn=0;

		while(r){
			int dispatched = 0;
			for(int j=0;j<m;j++){
				if(pthread_mutex_trylock(&dispatch_to_zone_lock[j])==0){  //if found vacancy,lock zone dipatch process (critical proc) else wait
					printf("Pharmaceutical company[%d] is delivering a batch of %d vaccines to vaccination zone[%d] which has success rate %d%%\n", in,p, j, success_per[in]);
					dispatched=1;
					zn=j;
					vaccinesinzone[j] = p;
					sleep(1);
					break;
				}
			}
			r-=dispatched;
		}
		while(vaccinesinzone[zn]!=0); //wait till previous batch gets over;
	}
	return NULL;
}


void *zone_maintain(void *index){
	int in = *((int*)index);
	pthread_mutex_trylock(&student_zone_lock[in]); //lock the slots, i.e. dont start allocation.
	while(!exitt){

        while(vaccinesinzone[in] == 0);           //wait for vaccines to arrive
        vac_start[in]=0;                          //vaccination phase not started (its a flag)  
		slots[in] = min(vaccinesinzone[in],(rand()%8)+1);     //generate slots    
		printf("%d slots are available in zone[%d]\n",slots[in], in);
		pthread_mutex_unlock(&student_zone_lock[in]);		// unlock for allocation for students
		int s = slots[in];                                 //s is total no. of slots allocated
	    
	    while((slots[in]!=0 && student_wait)|| slots[in]==s){ 				       //wait till all slots get occupied or till no students are waiting with atleast one slot occupied
	    	if(!slots[in]) break;
	    	sleep(6);												//if slots remain
	    	if(student_wait==0)                                    //waits for students till 5secs, if no one turns up proceed;
	    		break;
	    }
	    
	    pthread_mutex_trylock(&student_zone_lock[in]);   // stop allocation of slots and enter vaccination phase;
	    printf("Zone[%d] is entering vaccination phase\n", in);
	    vac_start[in]=1;
	    vaccinesinzone[in] -= s; 		    	  // vaccines got reduced in vaccination phase;
		if(vaccinesinzone[in] == 0)				  
			pthread_mutex_unlock(&dispatch_to_zone_lock[in]);  //when vaccines are over , unlock and ask from pharma;
		sleep(1);
	}

	return NULL;
}


void *student_allocation(void *index)
{
	sleep(rand()%4+1);            //IIIT students coming at random time
	int id = *((int*)index);
	printf("Student[%d] arrived to Campus\n",id);
	int vac_success=0;
	int vaccinated = 0;
	int flag=1;
	while(vaccinated < 3){                   // to get vaccinated atmost 3 times till success
		printf("Student[%d] is waiting to be allocated for his/her round %d \n", id, vaccinated+1);
		pthread_mutex_trylock(&student_wait_lock);  // lock for no. of students waiting
		student_wait++;                     
		pthread_mutex_unlock(&student_wait_lock);
		flag=1;         // flag denotes whether student is allocated previously or not (0 if yes)
		while(flag){
			fri(0,m){
				if(!pthread_mutex_trylock(&student_zone_lock[i])){  //searching zones
					if(slots[i] == 0){								//no slots in zone, ;leave		
						pthread_mutex_unlock(&student_zone_lock[i]);
						continue;
					}
					printf("Student[%d] is  assigned at zone[%d]: %d slots remain available\n", id, i, --slots[i]);
					pthread_mutex_unlock(&student_zone_lock[i]);
					pthread_mutex_lock(&student_wait_lock);
					student_wait--;
					pthread_mutex_unlock(&student_wait_lock);
					while(!vac_start[i]); 	                       // wait for start of vaccination phase
					//printf(" vac stsrt %d\n",vac_start[i]);
					int success = rand()%101;                       // generate random number to determine success or failure
					vaccinated++;
					sleep(0.2);
					if (success<=success_per[zonepharma[i]]){       //comparing random number with probability whether success or not
						printf("Vaccination of student[%d] is successfull :), he/she can attend college :)\n",id);	
						vac_success=1;
					}
					else printf("Vaccination of student[%d] is unsuccessfull :(, %d trials left\n",id, 3-vaccinated);
					flag=0;
					break;
				}
			}	
		}
		if(vac_success)	//if vaccination is succesful go to campus;
			break;			// exit loop and end thread;
	}
	return NULL;
}

void destroy_mutex(){
    int i=0;
    while(i<m){
        pthread_mutex_destroy(&student_zone_lock[i]);
        pthread_mutex_destroy(&dispatch_to_zone_lock[i]);
        i++;
    } 
}

int main()
{
	srand(time(NULL));    //seed for rand
	printf("Number of pharma companies: ");
	scanf("%d", &n);
	printf("Number of Vaccination zones: ");
	scanf("%d", &m);
	printf("Number of students registered: ");
	scanf("%d", &o);
	if(m<=0 || n<=0 || o<=0 ){
		printf("Simulation not possible\n");
		return 0;
	}
	fri(0,n){
		printf("Success percentage for pharma[%d]: ",i);
		scanf("%d",&success_per[i]);
		while(success_per[i]<=0 || success_per[i]> 100)
		{
			printf("Pls enter valid Success percentage for pharma[%d]: ",i);
			scanf("%d",&success_per[i]);
		}
	}
	int pharmaid[n],zoneid[m],studentid[o];  //storing ids of pharmas ,zones and students for unique pointers
	   fri(0,n) pharmaid[i]=i;
 	   fri(0,m) zoneid[i]=i;
       fri(0,o) studentid[i]=i; 
    fri(0,n){
		pthread_create(&(pharma_th[i]), NULL, &pharma, &pharmaid[i]); //start threading for pharmas
		sleep(0.0001);
	}

	fri(0,m){
		int j=i;		
        pthread_mutex_init(&dispatch_to_zone_lock[i], NULL); // initializing the mutex locks		
        pthread_mutex_init(&student_zone_lock[i], NULL);  
		
        pthread_create(&(zone_th[i]), NULL, &zone_maintain, &zoneid[i]);  //start threading for zones
		sleep(0.0001);
	}

	fri(0,o){
		int j=i;	
		pthread_create(&(student_th[i]), NULL, &student_allocation, &studentid[i]);  //start threading for students
		sleep(0.0001);
	}

	fri(0,o)
		pthread_join(student_th[i], NULL);  //wait for the student threads to terminate
											//i.e wait for the simulation to complete
    exitt=1;								//terminating the threads	
    destroy_mutex();                        //terminating mutexes;
    
    printf("x----------------Simulation Over----------------x\n");
	return 0;
}