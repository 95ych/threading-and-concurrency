#define _POSIX_C_SOURCE 199309L
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>

#define fri(s,e) for(int i=s;i<e;i++)

int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}

void selectionsort(int* array, int s, int e){
	
	int j=s, minind;
	while(j<e){
		int min =array[j];
		minind =j;
		for(int i=j+1;i<=e;i++){
			if(array[i]<min){
				min=array[i];
				minind=i;
			}
		}
		array[minind]=array[j];
		array[j]=min;
		j++;
	}
	return;
}

void merge(int* arr, int s, int m, int e){
	int n1=m+1-s;
	int n2=e-m;
	int l[n1], r[n2];
	fri(0,n1) l[i]=arr[s+i];
	fri(0,n2) r[i]=arr[i+m+1];
	int i=0,j=0,k=s;
	while(i<n1 && j<n2){
		if(l[i]<=r[j]){
			arr[k++]=l[i++];
			
		}
		else{
			arr[k++]=r[j++];			
		}
	}
	while(i<n1){
		arr[k++]=l[i++];
	}
	while(j<n2){
		arr[k++]=r[j++];
	}
}

void norm_mergesort(int* arr,int s,int e){
	if(e-s+1<5){
		selectionsort(arr,s,e);
		return;
	}
	if(s<e){
		int m = s + ((e-s)/2);
		norm_mergesort(arr,s,m);
		norm_mergesort(arr,m+1,e);
		merge(arr,s,m,e);
	}
	return;
}

void conc_mergesort(int* arr,int s,int e){
	if(e-s+1<5){
		selectionsort(arr,s,e);
		return;
	}
	if(s<e){
		int m = s + ((e-s)/2);
		int pid1 = fork();
		if(!pid1){
			conc_mergesort(arr,s,m);
			exit(1);
		}
		else{
			int pid2=fork();
			if(!pid2){
				conc_mergesort(arr,m+1,e);
				exit(1);
			}
			else{
				int status;
                waitpid(pid1, &status, 0);
                waitpid(pid2, &status, 0);
				merge(arr,s,m,e);
			}
		}
	}
	return;
}

int main(){
	struct timespec ts;
	int inp_size;
	scanf("%d",&inp_size);
	int* arr = calloc(inp_size,sizeof(int));
	fri(0,inp_size)
		scanf("%d",&arr[i]);
	int *brr = shareMem(sizeof(int)*(inp_size));
	fri(0,inp_size)
		brr[i]=arr[i];
	printf("Running normal mergesort...\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
	norm_mergesort(arr,0,inp_size-1);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
	fri(0,inp_size)
		printf("%d ",arr[i]);
	printf("\n");
	printf("Time taken: %Lf\n",en-st);
	long double t1=en-st;

	printf("\nRunning concurrent mergesort...\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
	conc_mergesort(brr,0,inp_size-1);
	fri(0,inp_size){
		printf("%d ",brr[i]);	
	}
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("Time taken: %Lf\n",en-st);
    long double t2=en-st;
    printf("\nNormal mergesort is %Lf times faster than concurrent mergesort",t2/t1);
}