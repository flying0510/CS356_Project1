/****************************************************************************
*		File Name: pstree_execl.c									    	*
*		Used: For Course CS356 OS Project1 @SJTU 2021 Spring		    	*
*		Copyright (c) 2021 SJTU Sophomore KaiXiang Yang				    	*
*		Author: KaiXiang Yang										    	*
*		Student ID : 519030910240											*
*		Creation Date: 2021-03-26											*
*		Description: Fork a child process and execute testARM 				*
*					 in child process										*
*		Compile: ubuntu_20.10(64 bit) ndk-build under folder 'jni'			*
*				 with 'Android.mk' and run execlARM in AVD					*
===========================================================================*/



#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[]){

	// Print parent process pid
	printf("519030910240Parent, PID: %d\n",getpid());
	pid_t pid = fork();
	if (pid < 0){
		fprintf(stderr, "Fork error!\n");
		return -1;
	}
	// Child Process
	else if(pid == 0){
		printf("519030910240Child, PID: %d\n", getpid());
		execl("./testARM", "testARM", NULL);
	}
	// Parent Process
	else{
		// Wait for child process finishing
		wait(NULL);
		printf("Child Completed!\n");
	}
	
	return 0;
}

