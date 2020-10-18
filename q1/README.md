# Comparing mergesorts

## Overview

The program takes an input 'n' followed by 'n' integer inputs. \
(The necessary functionalities are present in code comments) \
And sorts them by normal mergesort and concurrent mergesort. \
The concurrent mergesort process works by forking, \
we create 2 child processees which sort and the parent waits for them and then merges the 2 partitions. \
We observe the perfomance between them, \
normal mergesort performs around 400-500 times faster than concurrent mergesort as tested in my computer. \
Why??

## Explanation
Commonly in single-core processors, parallel processes work , by switching the processor rapidly between to processes. \
soo if process p1 takes t1 time and process p2 takes t2 time , if they run one by one works ends up in t1+t2 time. \
In case of parallelism it might end up taking more than t1+t2,\ 
(since the process take total t1+t2 and overhead for swithing might end up taking a bit of time)

But here in the case of concurrent mergesort the parent waits till BOTH the processes complete , hence taking more time than t1+t2+overhead. \
Where as in normal merge sort, it completes in t1+t2. \
Hence the performance difference.
