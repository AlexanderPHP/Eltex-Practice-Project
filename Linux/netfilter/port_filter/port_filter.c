#include "port_filter.h"

static uint16_t f_port;

static int __init
nf_port_init(void)
{
	f_port = htons(7777);
	nf_register_hook(&nf_port_hook);
	return 0;
}

static void __exit
nf_port_exit(void)
{
	nf_unregister_hook(&nf_port);
}

module_init(nf_port_init);
module_exit(nf_port_exit);

uint32_t
block_packet_by_port(void *priv, struct sk_buff * skb, const struct nf_hook_state *state)
{
	struct iphdr *ip_header;
	struct tcphdr *tcp_header;
	struct udphdr *udp_header;

	ip_header = ip_hdr(skb);

	switch(ip_header->protocol) {
		case IPPROTO_TCP:
			tcp_header = tcp_hdr(skb);
			if(tcp_header->source == f_port|| tcp_header->dest == f_port)
			{
				return NF_DROP;
			}
			break;
		case IPPROTO_UDP:
			udp_header = udp_hdr(skb);
			if(udp_header->source == f_port || udp_header->dest == f_port)
			{
				return NF_DROP;
			}
			break;
	}

	return NF_ACCEPT;
}