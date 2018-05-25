# Misusing debugfs for In-Memory RCE

This repo contains code that demonstrates a method of code execution using debugfs and netfilter in a loadable kernel module.

## Overview

debugfs is a simple-to-use RAM-based file system specially designed for kernel debugging purposes. It was released with version 2.6.10-rc3 and written by Greg Kroah-Hartman.

Netfilter is a framework in the Linux kernel that allows kernel modules to register callback functions called hooks in the kernel's networking stack.

Both debugfs and Netfilter have very legitimate (non-malware related) uses. debugfs obviously provides debugging purposes while also exposing information from the kernel to userspace. Netfilter is commonly used to implement filtering or modifying rules for packets. Those familiar with `iptables` may find it interesting that it uses customized Netfilter hooks.

## debugfs Chart

Present: Is /sys/kernel/debug/ present on first load?

Mounted: Is /sys/kernel/debug/ mounted on first load?

Possible: Can debugfs be mounted with `sudo mount -t debugfs none /sys/kernel/debug`?

| Operating System | Present | Mounted | Possible |
|:-----------------|:-------:|:-------:|:--------:|
| Ubuntu 6.06      | No      | No      | No       |
| Ubuntu 8.04      | Yes     | No      | Yes      |
| Ubuntu 10.04\*    | Yes     | Yes     | Yes      |
| Ubuntu 12.04     | Yes     | Yes     | Yes      |
| Ubuntu 14.04\*\*   | Yes     | Yes     | Yes      |
| Ubuntu 16.04     | Yes     | Yes     | Yes      |
| Ubuntu 18.04     | Yes     | Yes     | Yes      |
| Centos 6.9       | Yes     | No      | Yes      |
| Centos 7         | Yes     | Yes     | Yes      |

\* debugfs also mounted on the server version as rw,relatime on /var/lib/ureadahead/debugfs

\*\* tracefs also mounted on the server version as rw,relatime on /var/lib/ureadahead/debugfs/tracing

## Full Blog Post

The full blog post can be found [here](https://nbulischeck.io/posts/misusing-debugfs-for-in-memory-rce). This includes significantly more detail on the implementation and reasoning behind this project.
