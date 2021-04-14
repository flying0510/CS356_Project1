/****************************************************************************
*		File Name: BurgerBuddies.c									    	*
*		Used: For Course CS356 OS Project1 @SJTU 2021 Spring		    	*
*		Copyright (c) 2021 SJTU Sophomore KaiXiang Yang				    	*
*		Author: KaiXiang Yang										    	*
*		Student ID : 519030910240											*
*		Creation Date: 2021-04-05											*
*		Description: Simulate the procedure in a burger shop				*
*					 Accomplish synchronization requirements.				*
*		Compile: ubuntu_20.10(64 bit) ndk-build under folder 'jni'			*
*				 with 'Android.mk' and run BBCARM in AVD					*
*		Run: four arguments:cook_num,cashier_num,customer_num,rack_size		*
*			 execute by ./BBCARM a1 a2 a3 a4								*
===========================================================================*/


#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//define cooking time (by microsecond)
#define COOKING 100000
//define basetime of customers' random arrival time
#define BASETIME 100000

//define boundaries for random coefficient on basetime
#define LOW_LINE 1
#define HIGH_LINE 10

int cook_num, cashier_num, customer_num, rack_size;
pthread_t *cook_thread, *cashier_thread, *customer_thread;

//record the number of customers in the queue of each cashiers
int *waiting_customer;

//represents the threads' index
int *cook_ind, *cashier_ind, *customer_ind;

//actually a mutex array for (int *)waiting_customer
sem_t *available_cashier;

/*semaphores to record currently in the rack
* how many positions are empty (init(rack_size))
* and how many positions have burger (init(0)) */ 
sem_t rack_empty;
sem_t rack_full;

//mutex for output operation to print ture execution sequence
sem_t mutex_printf;


//Three kinds of thread's running function.
//The argument used to assign its index.
void *cook(void *i);
void *cashier(void *i);
void *customer(void *i);


//Cook
/*while(1){
	wait(rack_empty);
	//cooking......
	signal(rack_full);
}*/
void *cook(void *i)
{
	int index = *(int*)i;
	while(1){
		sem_wait(&rack_empty);
		usleep(COOKING);

		sem_wait(&mutex_printf);
		printf("Cook [%d] makes a burger.\n",index);
		sem_post(&mutex_printf);

		sem_post(&rack_full);
	}
}


//Cashier
/*while(1){
	//Accept an order......
	wait(rack_full);
	signal(rack_empty);
	if(--waiting_customer[index] == 0)
		exit(0);
}*/
void *cashier(void *i)
{
	int index = *(int*)i;
	while(1){
		sem_wait(&mutex_printf);
		printf("Cashier [%d] accepts an order.\n",index);
		sem_post(&mutex_printf);

		sem_wait(&rack_full);
		sem_post(&rack_empty);

		sem_wait(&mutex_printf);
		printf("Cashier [%d] takes a burger to a customer.\n",index);
		sem_post(&mutex_printf);

		sem_wait(available_cashier+index);
		if((--waiting_customer[index])==0){
			sem_wait(&mutex_printf);
			printf("Cashier [%d] goes to sleep.\n",index);
			sem_post(&mutex_printf);

			sem_post(available_cashier+index);
			pthread_exit(0);
		}
		else sem_post(available_cashier+index);
	}
}


//Customer
/*while(1){
	random_sleep();
	random_select_cashier();
	if(++waiting_customer[cashier_index]==1)
		create(cashier(cashier_index));
}*/
void *customer(void *i)
{
	int ra = rand()%(HIGH_LINE - LOW_LINE + 1) + LOW_LINE;
	usleep(ra*BASETIME);

	int index = *(int*)i;

	sem_wait(&mutex_printf);
	printf("Customer [%d] comes.\n",index);
	sem_post(&mutex_printf);

	int cashier_index = rand() % cashier_num;

	sem_wait(available_cashier+cashier_index);
	if((++waiting_customer[cashier_index])==1){
		pthread_create(cashier_thread+cashier_index, NULL, cashier, cashier_ind + cashier_index);		
		sem_wait(&mutex_printf);
		printf("Cashier [%d] wakes up.\n",cashier_index);
		sem_post(&mutex_printf);
	}
	sem_post(available_cashier+cashier_index);
}


int main(int argc, char ** argv)
{
	//If the number of execution arguments is not 5, exit(-1)
	if(argc!=5){
		printf("Wrong Input!\n");
		printf("Please Input Valid Variables (Positive Integer) (4)!\n");
		printf("1: the number of Cooks; 2: the number of cashiers; 3: the number of customers; 4: the size of Rack\n");
		exit(-1);
	}
	int i;
	srand(time(NULL));

	//get four parameters
	cook_num = atoi(argv[1]);
	cashier_num = atoi(argv[2]);
	customer_num = atoi(argv[3]);
	rack_size = atoi(argv[4]);
	
	// Ensure correct input
	if(cook_num <= 0 || cashier_num <= 0 || customer_num <= 0 || rack_size <= 0) {
		printf("Wrong Input!\n");
		printf("Please Input Valid Variables (Positive Integer) (4)!\n");
		printf("1: the number of Cooks; 2: the number of cashiers; 3: the number of customers; 4: the size of Rack\n");
		exit(-1);
	}
	
	printf("Cooks: %d, Cashiers: %d, Customers: %d, RackSize: %d\n",cook_num, cashier_num, customer_num, rack_size);
	printf("BEGIN:\n");
	
	//initialize each waiting queue of cashiers
	waiting_customer = (int *)malloc(sizeof(int)*customer_num);
	for(i = 0; i < customer_num; ++i) waiting_customer[i] = 0;
	
	//the mutex array for waiting_customer
	available_cashier = (sem_t *)malloc(sizeof(sem_t)*cashier_num);
	
	//initialize the index for cooks
	cook_ind = (int *)malloc(sizeof(int)*cook_num);
	for(i = 0; i < cook_num; ++i) cook_ind[i] = i;
	
	//initialize the index for cashiers
	cashier_ind = (int *)malloc(sizeof(int)*cashier_num);
	for(i = 0; i < cashier_num; ++i) cashier_ind[i] = i;
	
	//initialize the index for customers
	customer_ind = (int *)malloc(sizeof(int)*customer_num);
	for(i = 0; i < customer_num; ++i) customer_ind[i] = i;
	
	//initialize all the semaphores
	for(i = 0; i < cashier_num; ++i) sem_init(available_cashier+i,0,1);
	sem_init(&rack_empty,0,rack_size); //at first the rack is empty
	sem_init(&rack_full,0,0);
	sem_init(&mutex_printf,0,1);
	
	//generate these threads
	cook_thread = malloc(sizeof(pthread_t)*cook_num);
	cashier_thread = malloc(sizeof(pthread_t)*cashier_num);
	customer_thread = malloc(sizeof(pthread_t)*customer_num);
	
	//Create all the cook_thread
	for(i = 0; i < cook_num; ++i){
		pthread_create(cook_thread+i,NULL,cook,cook_ind+i);
	}

	//Create all the customer_thread
	for(i = 0; i < customer_num; ++i){
		pthread_create(customer_thread+i,NULL,customer,customer_ind+i);
	}

	//Wait for the termination of these threads
	for(i = 0; i < customer_num; ++i)
		pthread_join(customer_thread[i],NULL);
	for(i = 0; i < cashier_num; ++i)
		pthread_join(cashier_thread[i],NULL);
	//for(i = 0; i < cook_num; ++i)
		//pthread_cancel(cook_thread[i]);

	//Free all the resources applied before
	free(waiting_customer);
	free(available_cashier);
	free(cook_ind);
	free(cashier_ind);
	free(customer_ind);
	free(cook_thread);
	free(cashier_thread);
	free(customer_thread);

	sem_wait(&mutex_printf);
	printf("END!\n");
	printf("All customers have been served!\n");
	sem_post(&mutex_printf);

	//Using exit(0) to exit safely, it will terminate all the child thread, and then terminate current thread.
	exit(0);
}
