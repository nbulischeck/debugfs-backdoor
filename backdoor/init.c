#include "init.h"
#include "nfhook.h"

static int __init init_mod(void){
	printk("Inserting module\n");
	return nfhook_init();
}

static void __exit exit_mod(void){
	printk("Removing module\n");
	nfhook_exit();
}

MODULE_LICENSE("GPL");

module_init(init_mod);
module_exit(exit_mod);
