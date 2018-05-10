#include "init.h"

static int __init init_mod(void){
	printk("Inserting module\n");
	return 0;
}

static void __exit exit_mod(void){
	printk("Removing module\n");
}

MODULE_LICENSE("GPL");

module_init(init_mod);
module_exit(exit_mod);
