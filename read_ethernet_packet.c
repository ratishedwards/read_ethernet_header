#include<stdio.h>		// printf()
#include<sys/socket.h>		// socket()
#include<netinet/in.h>		// IPPROTO_RAW
#include<unistd.h>		// close()
#include<stdlib.h>		// EXIT_FAILURE
#include<linux/if.h>		// IFNAMSIZ
#include<linux/if_ether.h>	// ETH_P_ALL

#define BUFF_SIZE 1024

struct ETHERNET_HEADER {
	uint8_t destination_mac[6];
	uint8_t source_mac[6];
	uint16_t type;
} __attribute__((packed));

int main(int argc, char *argv[]){

	int sock_fd;
	int opt = 1;
	char *ifName = "wlp0s20f3";
	int bytes = 0;
	uint8_t buffer[BUFF_SIZE] = {0};

	// Pointer to the ethernet header
	struct ETHERNET_HEADER *eth_header = (struct ETHERNET_HEADER *)buffer;

	printf("+++++++++++Program to read Ethernet header and print it++++++++++++++++++\n");

	// To receive all IP packets, use ETH_P_IP with socket
	if ((sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0){
		perror("socket() error");
		exit(EXIT_FAILURE);
	}

	// Allow socket reuse
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
                perror("setsockopt() error");
                exit(EXIT_FAILURE);
        }

	// Bind the socket to particular device
	if (setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) < 0){
		perror("setsockopt() error");
		exit(EXIT_FAILURE);
	}

	printf("Waiting for a packet...\n");

	if ((bytes = recvfrom(sock_fd, buffer, BUFF_SIZE, 0, NULL, NULL)) < 0){
		perror("Receive Error()");
		exit(EXIT_FAILURE);
	}
	
	printf("Received packet...[%d bytes]\n", bytes);
	printf("++++++++++ETHERNET HEADER+++++++++++++++\n");
	printf("+ destination_mac : %02X:%02X:%02X:%02X:%02X:%02X\n",
                eth_header->destination_mac[0],
                eth_header->destination_mac[1],
                eth_header->destination_mac[2],
                eth_header->destination_mac[3],
                eth_header->destination_mac[4],
                eth_header->destination_mac[5]
                );
	printf("+ source_mac      : %02X:%02X:%02X:%02X:%02X:%02X\n",
		eth_header->source_mac[0],
		eth_header->source_mac[1],
		eth_header->source_mac[2],
		eth_header->source_mac[3],
		eth_header->source_mac[4],
		eth_header->source_mac[5]
		);
	printf("+ type            : %x\n",
		ntohs(eth_header->type));
	printf("++++++++++++++++++++++++++++++++++++++++\n\n");

	close(sock_fd);
	return 0;
}
