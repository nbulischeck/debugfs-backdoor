#ifndef NFHOOK_H
#define NFHOOK_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/esp.h>

#define DATA_LEN 255
#define TARGET_SPI 0x48dcd78c
#define TARGET_SEQ 0xae574ada

struct nf_hook_ops;

int nfhook_init(void);
void nfhook_exit(void);

unsigned int nfhook(unsigned int, struct sk_buff *, const struct net_device *, const struct net_device *, int (*okfn)(struct sk_buff *));

#endif /* NFHOOK_H */