/****************************************************************************
*		File Name: sysc_ptree.c									    	    *
*		Used: For Course CS356 OS Project1 @SJTU 2021 Spring		    	*
*		Copyright (c) 2021 SJTU Sophomore KaiXiang Yang				    	*
*		Author: KaiXiang Yang										    	*
*		Student ID : 519030910240											*
*		Creation Date: 2021-03-25											*
*		Description: A module for my system call: sys_pstreecall			*
*					 used to generate pstree like linux in android OS		*
*                    We use DFS to implementation this system call          *
*                    and the system call number is 356.                     *
*		Compile and Run: ubuntu_20.10(64 bit) make with 'Makefile' and     	*
*                push sysc_ptree.ko to avd and then install this module     *
===========================================================================*/



#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
MODULE_LICENSE("Dual BSD/GPL");

// This system call's number
#define __NR_pstreecall 356

// This is the maximum number of processes
#define MAXN 32768

// This function pointer used to record the origin system call with number 356
static int (*oldcall)(void);

// To store the information of process
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

// Use DFS to traverse all the processes, and record the depth for each process.
static void dfs(struct task_struct *task, struct prinfo *buffer, int *number, int depth)
{
    struct list_head *tmp;
    int n = *number;
    
    //printk("At level %d\n",depth);  
    //memset(buffer, 0, sizeof(struct prinfo));
   
    // Record current process in the buffer
    buffer[n].parent_pid = (task -> real_parent) -> pid;
    buffer[n].pid = task -> pid;

    // If current process has no child, set first_child_pid by 0
    if(!list_empty(&task->children))
    	buffer[n].first_child_pid = list_entry((task->children).next, struct task_struct, sibling)->pid;
    else buffer[n].first_child_pid = 0;

    // If current process has no sibling, set next_sibling_pid by 0
    if(!list_empty(&task->sibling))
    	buffer[n].next_sibling_pid = list_entry((task->sibling).next, struct task_struct, sibling)->pid;
    else buffer[n].next_sibling_pid = 0;

    buffer[n].state = task->state;
    buffer[n].uid = task -> real_cred -> uid;
    strcpy(buffer[n].comm, task->comm);
    buffer[n].depth = depth;
    
    //struct prinfo p = buffer[n];
	//printk("Task %d: %s, %d, %ld, %d, %d, %d, %ld.\n", n , p.comm, p.pid, p.state, p.parent_pid, 
			//p.first_child_pid, p.next_sibling_pid, p.uid);

    // Prepare to record next process.
    (*number)++;
    
    // According to DFS, then traverse all the children in next level.
    list_for_each(tmp, &task->children){
    	dfs(list_entry(tmp, struct task_struct, sibling), buffer, number, depth + 1);
    }
}

/* system call for pstree
 * First judge if the argument is valid, or exit with error
 * hold tasklist_lock and traverse by DFS
 * if the number of processes larger than that prepared by user, exit with error
 * copy the result from kernel to user */
static int sys_pstreecall(struct prinfo *buf, int *nr)
{
    printk(KERN_INFO "ptree() called.\n");
	printk("buf: %p, nr: %p\n", buf, nr);
	
	// Check buf
	if (buf == NULL) {
		printk("Buffer pointer is null!\n");
		return 1;
	}
	
	// Check nr
	if (nr == NULL) {
		printk("Buffer size pointer is null!\n");
		return 2;
	}
	
    // Store the size of buf given by user
    int* tmp;
    tmp = kmalloc(sizeof(int),GFP_KERNEL);
    tmp = (*nr);

	*nr = 0;

	struct prinfo *buffer;
	buffer = kmalloc(MAXN*sizeof(struct prinfo),GFP_KERNEL);
	
    // Traverse by DFS
	read_lock(&tasklist_lock);	
	printk("Start traversing task tree.\n");
	dfs(&init_task, buffer, nr, 0);	
	read_unlock(&tasklist_lock);

    // If user prepare not enough space to record, exit with error
    if((*nr) > tmp) {
    	printk("Buffer size is not enough!\n");
    	return 3;
    }

    // Copy from kernel to user, also can use function 'copy_to_user()'
	int i;
	for(i=0;i<(*nr);++i)
	{
		buf[i] = buffer[i];
	}
	//copy_to_user(buf, buffer, 1000*sizeof(struct prinfo));
	
	kfree(buffer);
	return 0;
}

//When load this module, record the old one first
static int addsyscall_init(void)
{
    long *syscall = (long*)0xc000d8c4;
    oldcall = (int(*)(void))(syscall[__NR_pstreecall]);
    syscall[__NR_pstreecall] = (unsigned long)sys_pstreecall;
    printk(KERN_INFO "pstree module load!\n");
    return 0;
}

//exit this module
static void addsyscall_exit(void)
{
    long *syscall = (long*)0xc000d8c4;
    syscall[__NR_pstreecall] = (unsigned long)oldcall;
    printk(KERN_INFO "pstree module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
