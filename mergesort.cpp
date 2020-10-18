#include<bits/stdc++.h>
using namespace std;
#define fri(s,e) for(int i=s;i<e;i++)
  
void merge(int arr[], int s, int m, int e){
	int n1=m+1-s;
	int n2=e-m;
	int l[n1], r[n2];
	fri(0,n1) l[i]=arr[s+i];
	fri(0,n2) r[i]=arr[i+m+1];
	int i=0,j=0,k=s;
	while(i<n1 && j<n2)
	{
		if(l[i]<=r[j])
			arr[k++]=l[i++];
		else
			arr[k++]=r[j++];
	}
	while(i<n1)
		arr[k++]=l[i++];
	while(j<n2)
		arr[k++]=r[j++];
}
	


void mergesort(int arr[],int s,int e){
	if(s<e){
	int m = s + ((e-s)/2);
	mergesort(arr,s,m);
	mergesort(arr,m+1,e);
	merge(arr,s,m,e);
	}
}


int main(){
	int size;
	cin >> size;
	int array[size];
	fri(0,size)	cin>>array[i];
	mergesort(array,0,size-1);
	fri(0,size)	cout<<array[i]<<" ";
}