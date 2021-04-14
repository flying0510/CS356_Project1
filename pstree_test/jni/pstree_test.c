/****************************************************************************
*		File Name: pstree_test.c									    	*
*		Used: For Course CS356 OS Project1 @SJTU 2021 Spring		    	*
*		Copyright (c) 2021 SJTU Sophomore KaiXiang Yang				    	*
*		Author: KaiXiang Yang										    	*
*		Student ID : 519030910240											*
*		Creation Date: 2021-03-25											*
*		Description: Used to test my system call							*
*					 Change the buf size or set the pointers to NULL		*
*					 to test these cases.									*
*		Compile: ubuntu_20.10(64 bit) ndk-build under folder 'jni'			*
*				 with 'Android.mk' and run testARM in AVD					*
===========================================================================*/



#include <stdio.h>

// This struct is the same with that in my system call
struct prinfo{
    pid_t parent_pid;               /*process id of parent*/
    pid_t pid;                      /*process id*/
    pid_t first_child_pid;          /*pid of youngest child*/  
    pid_t next_sibling_pid;	    	/*pid of older sibling*/
    long state;                     /*current state of process*/
    long uid;                       /*user id of process owner*/
    char comm[64];                  /*name of program executed*/
    int depth;     		    		/*process depth in the pstree*/
};

// Prepared to record the results
struct prinfo buf[1000];

// Represents the size of buf
int nr = 1000;

/*other exception test*/
// struct prinfo buf[10];
// int nr = 10; 


int main(int argc, char *argv[]) 
{
	int value;
	value = syscall(356, buf, &nr);

	// If my system call haven't executed correctly, return -1
	if(value!=0) return -1;

	int ip;

	// Print pstree by '\t'
	for (ip = 0; ip < nr; ip++) {
		struct prinfo p = buf[ip];
		int id;
		for (id = 0; id < p.depth; id++)
			printf("\t");
		printf("%s, %d, %ld, %d, %d, %d, %d\n", p.comm, p.pid, p.state, p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
	}
	return 0;
}
