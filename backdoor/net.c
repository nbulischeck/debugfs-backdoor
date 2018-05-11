#include "net.h"

char *inet_ntop(char *addr, struct connection *cn){
	switch (cn->addr_srv.sin_family){
	case AF_INET:
			snprintf(addr, INET_ADDRSTRLEN, "%pI4", &cn->addr_srv.sin_addr);
			break;
	case AF_INET6:
			snprintf(addr, INET6_ADDRSTRLEN, "%pI6", &cn->addr_srv.sin_addr);
			break;
	default:
			return NULL;
	}

	return addr;
}

unsigned int inet_addr(char *str){
	int a, b, c, d;
	char arr[4];
	sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
	arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;
	return *(unsigned int *)arr;
}

ssize_t recv(struct socket *socket, void *buffer, size_t length, int flags){
	struct socket *sk;
	struct msghdr msg;
	struct iovec iov;
	int ret;

	mm_segment_t old_fs;

	memset(&msg, 0, sizeof(msg));
	sk = (struct socket *)socket;

	iov.iov_base = (void *)buffer;
	iov.iov_len = (__kernel_size_t)length;

	msg.msg_iter.type = READ;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	msg.msg_iter.iov = &iov;
	msg.msg_iter.iov_offset = 0;
	msg.msg_iter.count = iov.iov_len;
	msg.msg_iter.nr_segs = 1;

	msg.msg_control = NULL;
	msg.msg_controllen = 0;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	ret = sock_recvmsg(sk, &msg, flags);

	set_fs(old_fs);

	return ret;
}

ssize_t send(struct socket *socket, const void *buffer, size_t length, int flags){
	struct socket *sk;
	struct msghdr msg;
	struct iovec iov;
	int len;

	mm_segment_t old_fs;

	sk = (struct socket *)socket;

	iov.iov_base = (void *)buffer;
	iov.iov_len = (__kernel_size_t)length;

	msg.msg_iter.type = READ;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	msg.msg_iter.iov = &iov;
	msg.msg_iter.iov_offset = 0;
	msg.msg_iter.count = iov.iov_len;
	msg.msg_iter.nr_segs = 1;

	msg.msg_control = NULL;
	msg.msg_controllen = 0;

	msg.msg_flags = flags;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	len = sock_sendmsg(sk, &msg);

	set_fs(old_fs);

	return len;
}
