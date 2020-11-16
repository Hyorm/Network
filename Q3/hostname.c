#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char** argv){

	struct hostent *host;
	struct in_addr host_add;

	long int *add;
	long int *ali;
	int idx = 0;

	host = gethostbyname(argv[1]);
	
	if(host == NULL){
		
		perror("ERROR:");
		exit(0);
	}
	
	printf("Official name: %s\n", host->h_name);

	//Aliases
	while(host->h_aliases[idx] != NULL){
		printf("Aliases %d: %s\n", idx, host->h_aliases[idx++]);
	}			

	//Address type
	printf("Address type: %s\n", (host->h_addrtype==AF_INET)? "AF_INET":"AF_INET6");

	//IP addr
	idx = 0;	
	while(*host->h_addr_list != NULL){
		add = (long int *)*host->h_addr_list;
		host_add.s_addr = *add;
		printf("IP addr %d: %s\n", ++idx, inet_ntoa(host_add));
		host->h_addr_list++;
	}
}
