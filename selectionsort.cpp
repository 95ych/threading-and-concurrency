//we search for min every time and keep it at beginning

#include<bits/stdc++.h>

using namespace std;

int main(){
	int inp_size;
	cin>>inp_size;
	int array[inp_size];
	for(int i=0; i<inp_size;i++)
	{
		cin>>array[i];
	}
	
	int j=0,minind;
	while(j!=inp_size-1){
		int min =array[j];
		minind=j;
		for(int i=j+1;i<inp_size;i++){
			if(array[i]<min){
				min=array[i];
				minind=i;
			}
		}
		array[minind]=array[j];
		array[j]=min;
		j++;

	}
	for(int i=0;i<inp_size;i++){
		cout<<array[i]<<" ";
	}
	return 0;
}


