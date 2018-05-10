#ifndef REQUEST_H
#define REQUEST_H

#include "net.h"

#define BUFFSIZE 1024

void parse_response(char *response);
int get(char *uri, struct connection *cn);
void connect(char *address, int port, struct connection *cn);

#endif /* REQUEST_H */
