# CS356 Operating System Projects - Project 1, Spring 2021

## Decription

This is Project 1 for the Course, CS356 Operating System Projects, in Spring 2021, SJTU. In this project, we have mainly two parts. 

First one is to write a new system call for android system to generate 'pstree' like that in linux system. We also use module to accomplish this system call. And then use a program to test my system call, also check robustness. Finally, fork a child process to execute 'testARM' to show the relationship between father process and child process.

Second part is to simulate the procedure in burger shop, which is a good practice and understanding for synchronization and multi-threaded programming. When executing, please set the arguments in command line, which includes the number of cooks, cashiers, and customers and also the rack size.

## Author

KaiXiang Yang , 519030910240, F1903302, SEIEE, SJTU

## Compile and Run

In each *.c file, I declare how to compile and run. And all the makefile and 'Android.mk' files are given under each file folder.

For more details, please refer to my sample execution and sample output in the file 'output.txt'.

## Directory Structure

```
CS356_Prj1
	|—— burger									# Problem 4 burger buddies problem
	|	  |—— jni								
	|	  |	   |—— Android.mk					 # Used to build and cross compile 'BurgerBuddies.c'	
	|	  |	   |—— BurgerBuddies.c				  # Main Source code for Problem 4
	|—— pstree									# Problem 1 android process tree module
	|	  |	   |—— Makefile						 # Used to compile 'pstree.c'
	|	  |    |—— pstree.c						 # Source code for Problem 1, could be made into my module
	|—— pstree_execl							 # Problem 3
	|     |—— jni
	|     |    |—— Android.mk					  # Used to build and cross compile 'ptree_execl.c'
	|     |    |—— ptree_execl.c				  # Source code for Problem 3, generate a child process and run 
	|—— pstree_test								 # Problem 2 Print android process tree
	|     |—— jni
	|     |    |—— Android.mk					  # Used to build and cross compile 'pstree_test.c'
	|     |    |—— pstree_test.c				  # Source code for Problem 2, invoke my system call and print 
	|										    # android pstree
	|—— output.txt								 # Sample execution command and sample output for all the 
	|											# problems including correct inputs and exceptions
	|——Prj1README.md							 # The README file
```

## Sample Output

Refer to 'output.txt'. This file shows the sample execution command and sample outputs of correct inputs as well as exceptions handling of wrong or invalid inputs.