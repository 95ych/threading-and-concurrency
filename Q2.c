#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define fri(s,e) for(int i=s;i<e;i++)
#define min(a,b) ((a) < (b) ? (a) : (b))
#define ll long long int
#define BUF 1024