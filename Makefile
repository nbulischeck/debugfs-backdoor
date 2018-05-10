BACKDOOR := backdoor

obj-m := $(BACKDOOR).o
$(BACKDOOR)-y += init.o
$(BACKDOOR)-y += request.o
$(BACKDOOR)-y += net.o
$(BACKDOOR)-y += debugfs.o
$(BACKDOOR)-y += nfhooks.o

default: all

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
