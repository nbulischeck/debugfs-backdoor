#ifndef NET_H
#define NET_H

#include <net/sock.h>

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46

struct connection {
	struct socket *sk;
	struct sockaddr_in addr_srv;
};

/* Net Util */
char *inet_ntop(char *addr, struct connection *cn);
unsigned int inet_addr(char *str);

/* Net */
ssize_t recv(struct socket *socket, void *buffer, size_t length, int flags);
ssize_t send(struct socket *socket, const void *buffer, size_t length, int flags);

#endif /* NET_H */
