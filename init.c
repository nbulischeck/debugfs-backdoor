#include "init.h"
#include "nfhook.h"

static int __init init_mod(void){
	int ret = 0;
	printk("Inserting module\n");
	ret = nfhook_init();
	return ret;
}

static void __exit exit_mod(void){
	printk("Removing module\n");
	nfhook_exit();
}

MODULE_LICENSE("GPL");

module_init(init_mod);
module_exit(exit_mod);
