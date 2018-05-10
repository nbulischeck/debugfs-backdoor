#include "net.h"
#include "debugfs.h"

#define BUFFSIZE 1024

unsigned char *buffer;
unsigned long buffer_length;
struct task_struct *task;

/* Parse HTTP Response and store in buffer */
void parse_response(char *response){
	char *body, *ptr, *temp;

	if ((ptr = strstr(response, "Content-Length: "))){
		temp = kstrdup(ptr, GFP_KERNEL);
		ptr = strsep(&temp, " \r\n");
		ptr = strsep(&temp, " \r\n");
		kstrtol(ptr, 10, &buffer_length);
		if ((body = strstr(response, "\r\n\r\n"))){
			buffer = (unsigned char *)(body+4);
			return;
		}
	}

	buffer = NULL;
	buffer_length = 0;
}

/* Perform an HTTP GET Request */
int get(char *uri, struct connection *cn){
	ssize_t total = 0, read = 0;
	char *response, *ptr, *temp;
	char *header, *req_fmt, *addr;

	req_fmt = "GET %s HTTP/1.1\r\nAccept: */*\r\nHost: %s\r\n\r\n";

	addr = kmalloc(INET6_ADDRSTRLEN, GFP_KERNEL);
	if (addr == NULL){
		return -1;
	}

	addr = inet_ntop(addr, cn);
	if (addr == NULL){
		return -1;
	}

	header = kmalloc(strlen(req_fmt)+strlen(addr)+1, GFP_KERNEL);
	if (header == NULL){
		return -1;
	}

	response = kmalloc(BUFFSIZE, GFP_KERNEL);
	if (response == NULL){
		return -1;
	}

	temp = kmalloc(BUFFSIZE, GFP_KERNEL);
	if (temp == NULL){
		return -1;
	}

	/* Craft the GET request header */
	snprintf(header, strlen(req_fmt)+strlen(uri)+strlen(addr), req_fmt, uri, addr);

	/* Send the GET request to the server */
	send(cn->sk, header, strlen(header), 0);

	/* Dynamically read in the server's response */
	while((read = recv(cn->sk, temp, BUFFSIZE, 0))){
		total += read;
		ptr = krealloc(response, total+1, GFP_KERNEL);
		if (!ptr){
			return -1;
		}
		response = ptr;
		memcpy(response+(total-read), temp, read);
	}
	response[total] = 0;

	parse_response(response);

	kfree(header); header = NULL;
	kfree(temp); temp = NULL;
	kfree(addr); addr = NULL;
	kfree(response); response = NULL;

	return 0;
}

void connect(char *address, int port, struct connection *cn){
	int ret = 0, addr_len;

	/* Zero out connection info */
	memset(cn, 0, sizeof(struct connection));

	/* Fill connection address info */
	memset(&cn->addr_srv, 0, sizeof(cn->addr_srv));
	cn->addr_srv.sin_family = AF_INET;
	cn->addr_srv.sin_port = htons(port);
	cn->addr_srv.sin_addr.s_addr = inet_addr(address);
	addr_len = sizeof(struct sockaddr_in);

	/* Create connection socket */
	ret = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &cn->sk);
	if (!cn->sk){
		return;
	}

	/* Connect to the remote host */
	ret = cn->sk->ops->connect(cn->sk, (struct sockaddr *)&cn->addr_srv, addr_len, 0);
	if (ret < 0){
		sock_release(cn->sk);
		cn->sk = NULL;
	}
}
