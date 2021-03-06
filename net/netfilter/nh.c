#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h> /* ip_hdr */

MODULE_LICENSE("GPL");
static struct nf_hook_ops nfho;
/* little endian:
 * 0:127
 * 1:0
 * 2:0
 * 3:1 */
static unsigned char drop_ip[] = "\x7f\x00\x00\x01";

unsigned int nfhook(unsigned hooknum, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out,
			int (*okfn)(struct sk_buff *))
{
	struct sk_buff *sk = skb;	
	struct iphdr *ip;
	if (sk == NULL || (ip = ip_hdr(sk)) == NULL)
		return NF_ACCEPT;
	ip = ip_hdr(sk);
	if (ip->protocol == IPPROTO_TCP) {
		printk(KERN_ALERT "drop tcp packet\n");
		return NF_DROP;
	}
	else if (ip->protocol == IPPROTO_ICMP) {
		printk(KERN_ALERT "drop icmp packet\n");
		return NF_DROP;
	}

	return NF_ACCEPT;
}

#define nf_reg(nfhops, hn, nf, pfnum)		\
	nfhops.hook = nf;			\
	nfhops.hooknum = hn;/*position*/	\
	nfhops.pf = pfnum; /*family*/		\
	nfhops.priority = NF_IP_PRI_FIRST;	\
	nf_register_hook(&nfhops)
	



static int __init nh_init(void)
{
	nf_reg(nfho, NF_INET_PRE_ROUTING, nfhook, PF_INET);
	printk(KERN_ALERT "init ok\n");
	return 0;
}

static void __exit nh_exit(void)
{
	nf_unregister_hook(&nfho);

	printk(KERN_ALERT "exit\n");
}

module_init(nh_init);
module_exit(nh_exit);
