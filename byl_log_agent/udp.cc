#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "udp.h"

#include "log.h"

extern Log log;

int bind_udp(string host, int port)
{
	int fd;
	struct sockaddr_in server_addr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		log.error("File[%s] Line[%d]: socket failed: %s\n",
						__FILE__, __LINE__, strerror(errno));
		return -1;
	}
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(host.c_str());
	server_addr.sin_port = htons(port);
	if (bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		log.error("File[%s] Line[%d]: bind failed: %s\n",
						__FILE__, __LINE__, strerror(errno));
		return -1;
	}
	
	log.info("The server is bind on %s:%d\n", host.c_str(), port);
	
	return fd;
}
