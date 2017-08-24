#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/string.h>
#include <asm/uaccess.h>

#include <linux/proc_fs.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Alexander Lopatin");
MODULE_DESCRIPTION("A module for blocking UDP/TCP traffic by port.");

#define NF_PORT_PROC_NAME "nf_port"
#define MAX_BUFF_SIZE 6

static int __init nf_port_init(void);
static void __exit nf_port_exit(void);

static uint32_t block_packet_by_port(void *priv, struct sk_buff * skb, const struct nf_hook_state *state);

static struct nf_hook_ops nf_port_hook = {
	.hook = block_packet_by_port,
	.pf = PF_INET,
	.hooknum = NF_INET_LOCAL_IN,
	.priority = NF_IP_PRI_FIRST
};
