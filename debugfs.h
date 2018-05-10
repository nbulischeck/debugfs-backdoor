#ifndef DEBUGFS_H
#define DEBUGFS_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/kthread.h>

int create_file(void);
void execute_file(void);
int destroy_file(void);
void debugfs_check(void);

#endif /* DEBUGFS_H */
