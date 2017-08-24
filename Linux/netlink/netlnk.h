#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#define IFLIST_REPLY_BUFFER 8192

typedef struct nl_req_s nl_req_t;  

struct nl_req_s {
	struct nlmsghdr hdr;
	struct rtgenmsg gen;
};
