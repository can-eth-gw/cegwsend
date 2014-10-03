/**
* @file cegwsend.c
* @brief Control Area Network - Ethernet - Gateway - Send (Utility)
* @author Jakob Pfeiffer (jakob.pfeiffer@in.tum.de)
* @details A Ethernet package is created here and sent to the Interface
* @date July, 2013
* @copyright GNU Public License v3 or higher
*/

/***************************************************************************
* (C) Copyright 2013 Fabian Raab, Stefan Smarzly, Jakob Pfeiffer
*
* This file is part of CAN-Eth-GW.
*
* CAN-Eth-GW is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* CAN-Eth-GW is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with CAN-Eth-GQ. If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#include <linux/version.h>
#include <linux/if_ether.h>
#include <getopt.h>
#include <stdlib.h>
#include "can.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <assert.h>

char *mac;

/**
* @fn void convert_to_mac(const char *string1)
* @brief converts a string from the console to a MAC address
* @param string1 the string that should be converted to a MAC
* @return MAC address with the length of 6 Bytes
*/
void convert_to_mac(const char *string1)
{
	/* i is used in the for loop */
	int i;
	char* string2;

	/*
	* A MAC address has the length of 6 Bytes.
	* We need to allocate enough memory to store it.
	*/
	mac = (char *) malloc(6);
	mac[0] = strtoul(string1, &string2, 16);
	for(i = 1; i <= 5; i++){
		mac[i] = strtoul(string2 + 1, &string2, 16);
	}
}

/**
* @fn static void die(const char *msg)
* @brief It prints an error message in case of a failure
* and exits the program
* @param msg The error message which should be written
* on the console
* @return EXIT_FAILURE
*/
static void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static unsigned char gethex(const char *s, char **end){
	assert(s);
	while(isspace(*s)) s++;
	assert(*s);
	return strtoul(s, end, 16);
}

unsigned char *convert_to_hex(const char *s, int *length){
	unsigned char *hex = malloc((strlen(s) +1) / 3);
	unsigned char *p;
	for(p = hex; *s; p++)
		*p = gethex(s, (char **) &s);
	*length = p - hex;
	return hex;
}

/**
* @fn int main(int argc, char *argv[])
* @brief creates an Ethernet package and sends it to a device.
* @param argc counts the arguments from the console
* param argv each word from the console is written down in this
* array
* @return EXIT_SUCCESS if the program was succesful,
* EXIT_FAILURE if an error occured
*/
int main(int argc, char *argv[])
{
	/* c is used for calling getopt_long() */
	int c;
	/*
	* src points at the beginning of the memory space in which the MAC address
	* of the message's receiver will be written
	*/
	char *src;
	src = (char *) malloc(6);
	/*
	* dest points at the beginning of the memory space in which the MAC
	* address of the message's sender will be written
	*/
	char *dest;
	dest = (char *) malloc(6);
	/* type is the identifier of the EtherType */
	ushort type;
	/*
	* raw_index points at the beginning of the raw data in argv[].
	* If raw_index == -1, then no data exists
	*/
	int raw_index = -1;
	/*
	* name_index is the index of argv in which the name of the ethernet
	* device begins. If name_index == -1, then there is no name
	*/
	int name_index = -1;

	/*
	* getopt_long is used here for reading the arguments from the command line
	*/
	while (1){
		static struct option long_options[] = {
			/* source MAC address */
			{"src", required_argument, 0, 's'},
			/* destination MAC address */
			{"dst", required_argument, 0, 'd'},
			/* packet type ID field */
			{"type", required_argument, 0, 't'},
			/* raw data */
			{"raw", required_argument, 0, 'r'},
			/* name of the network interface */
			{"interface", required_argument, 0, 'i'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "s:d:t:r:i:", long_options, &option_index);

		/* Detect the end of the options. */
		if(c == -1)
			break;

		switch (c){
			case 0:
				/* If this option set a flag, do nothing else now. */
				if(long_options[option_index].flag != 0)
					break;
				printf("option %s", long_options[option_index].name);
				if(optarg)
					printf(" with arg %s", optarg);
				printf("\n");
				break;

			case 's':
				printf("option -s with value `%s'\n", optarg);
				/*
				* convert the argument to a valid MAC address
				*/
				convert_to_mac(optarg);
				/*
				* convert_to_mac writes the address down in mac.
				* It must be stored in src now.
				*/
				memcpy(src, mac, 6);
				break;

			case 'd':
				printf("option -d with value `%s'\n", optarg);
				/*
				* convert the argument to al valid MAC address
				*/
				convert_to_mac(optarg);
				/*
				* convert_to_mac writes the address down in mac.
				* It must be stored in dest now.
				*/
				memcpy(dest, mac, 6);
				break;

			case 't':
				printf("option -t with value `%s'\n", optarg);
				/*
				* The EtherType is defined by a number with the length of
				* 2 Bytes (e. g. 0x8000 for the Internet Protocol). The
				* argument from the console must be converted to a number.
				* This happens here.
				*/
				if(strcmp(optarg, "ipv4") == 0){
					/*
					* The EtherType of Internet Protocol, Version 4 is
					* 0x0800
					*/
					type = 0x0800;
				} else if(strcmp(optarg, "ipv6") == 0){
					/*
					* The EtherType of Internet Protocol, Version 6 is
					* 0x86DD
					*/
					type = 0x86DD;
				} else if(strcmp(optarg, "can") == 0){
					/*
					* The EtherType of Controller Area Network is 0x000C
					*/
					type = 0x000C;
				} else if(strcmp(optarg, "canfd") == 0){
					/*
					* The EtherType of CAN flexible data rate is 0x000D
					*/
					type = 0x000D;
					/*
					* 0x88B5 and 0x88B6 have been
					* reserved for experimental use
					*/
				} else if(strcmp(optarg, "none") == 0){
					type = 0x88B5;
				/*
				* If the identifier was typed in like "0x1234" we cut off
				* the 0x and convert the number. If instead it was written
				* down like "1234" we simply convert the argument from the
				* console to a number.
				*/
				} else if(strncmp(optarg, "0x", 2) == 0){
					type = strtoul(optarg + 2, &optarg, 16);
				} else {
					type = strtoul(optarg, &optarg, 16);
				}
				break;

			case 'r':
				printf("option -r with value `%s'\n", optarg);
				raw_index = optind - 1;
				break;

			case 'i':
				printf("option -i with value `%s'\n", optarg);
				name_index = optind - 1;
				break;

			case '?':
				/* getopt_long already printed error message- */
				break;

			default:
				abort();
		}
	}
	/* mac is not needed anymore */
	free(mac);

	printf("source: %hhX", *src);
	printf(":%hhX", *(src + 1));
	printf(":%hhX", *(src + 2));
	printf(":%hhX", *(src + 3));
	printf(":%hhX", *(src + 4));
	printf(":%hhX\n", *(src + 5));
	printf("destination: %hhX", *dest);
	printf(":%hhX", *(dest + 1));
	printf(":%hhX", *(dest + 2));
	printf(":%hhX", *(dest + 3));
	printf(":%hhX", *(dest + 4));
	printf(":%hhX\n", *(dest + 5));
	printf("type: %hX \n", type);

	/*
	* Create the AF_PACKET socket
	*/
	int fd = socket(AF_PACKET, SOCK_DGRAM, htons(type));
	/*
	* In case of a failure the allocated memory must be freed and die is called
	*/
	if (fd == -1){
		free(dest);
		free(src);
		die(strerror(errno));
	}

	/*
	* Name/index number of the interface
	* If there is no name, we have a problem and the program must end
	*/
	if(name_index == -1){
		free(dest);
		free(src);
		die("name/index number of the network interface required");
	}
	struct ifreq ifr;
	size_t if_name_len = strlen(argv[name_index]);
	if(if_name_len < sizeof(ifr.ifr_name)){
		memcpy(ifr.ifr_name, argv[name_index], if_name_len);
		ifr.ifr_name[if_name_len] = 0;
	} else {
		printf("sizeof(ifr.ifr_name): %d", sizeof(ifr.ifr_name));
		printf(" sollte groesser sein als if_name_len: %d", if_name_len);
		free(src);
		free(dest);
		die("interface name is too long");
	}
	if(ioctl(fd, SIOCGIFINDEX, &ifr) == -1){
		free(src);
		free(dest);
		die(strerror(errno));
	}
	int ifindex = ifr.ifr_ifindex;

	/*
	* Construct the destination address
	*/
	struct sockaddr_ll addr = {0};
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = ifindex;
	addr.sll_halen = ETHER_ADDR_LEN;
	addr.sll_protocol = htons(type);
	memcpy(addr.sll_addr, dest, ETHER_ADDR_LEN);
	/* dest is not needed anymore */
	free(dest);

	/*
	* Prepare the data to be sent. If it exists, of course
	*/
	if(raw_index == -1){
		free(src);
		die("No data to be sent");
	}
	/*
	* Here, the raw data is converted from a string to a number
	*/
	unsigned char *data = NULL;
	int length = strlen(argv[raw_index]);
	data = convert_to_hex(argv[raw_index], &length);
	int data_length = ((strlen(argv[raw_index]) + 1) / 3);
	/*
	* If the package is a can package, a can frame must be
	* written into the payload
	*/
	if(type == 0x000C){
		/*
		* The maximum data which fits in a can frame
		* is 8 Bytes. If the data is too long, the
		* program terminates.
		*/
		if(data_length > 8){
			free(src);
			free(data);
			die("The raw data is too long for a can frame");
		}
		struct can_frame frame;
		memset(frame.data, 0, 8);
		frame.can_id = 0;
		frame.can_dlc = data_length;
		memcpy(frame.data, data, data_length);
		/*
		* A Ethernet frame which contains the can frame in
		* its payload is sent
		*/
		if(sendto(fd, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1){
			free(data);
			free(src);
			die(strerror(errno));
		}
	} else if(type == 0x000D){
		printf("Ja, wir haben einen canfd\n");
		/*
		* The maximum data which fits in a canfd frame
		* is 64 Bytes. If the data is too long, the
		* program terminates.
		*/
		if(data_length > CANFD_MAX_DLEN){
			free(data);
			free(src);
			die("The raw data is too long for a canfd frame");
		}
		static struct canfd_frame frame;
		memset(frame.data, 0, CANFD_MAX_DLEN);
		frame.can_id = 0;
		frame.flags = 0;
		frame.len = data_length;
		memcpy(frame.data, data, data_length);
		printf("Alles kopiert und so. Wird gleich gesendet.\n");
		/*
		* A Ethernet frame which contains the canfd frame
		* in its payload is sent
		*/
		if(sendto(fd, &frame, sizeof(struct canfd_frame), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1){
			free(data);
			free(src);
			die(strerror(errno));
		}
		printf("Jap, gesendet.\n");
	} else {
		/* Finally, let's send the package */
		if(sendto(fd, data, data_length, 0, (struct sockaddr*)&addr, sizeof(addr)) == -1){
			free(data);
			free(src);
			die(strerror(errno));
		}
	}
		/*
		* Nerver forget to free the allocated memory
		*/
		free(data);
		free(src);
		return EXIT_SUCCESS;
}
