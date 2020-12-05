#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#define MAX 512

void recv_binary_file(int serv_sock, struct sockaddr_in clnt_adr, socklen_t clnt_adr_sz);
void error_handling(char *message);

typedef struct _Packet{
	int sqno;
	char data[MAX];
	int data_size;
}Packet;

int main(int argc, char *argv[])
{
	int serv_sock;
	socklen_t clnt_adr_sz;
	
	struct sockaddr_in serv_adr, clnt_adr;
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock==-1)
		error_handling("UDP socket creation error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	recv_binary_file(serv_sock, clnt_adr, clnt_adr_sz);
	
	close(serv_sock);
	return 0;
}

void recv_binary_file(int serv_sock, struct sockaddr_in clnt_adr, socklen_t clnt_adr_sz){

	int sqno_ = 0;
	int len = 0,rd = 0, file_len = 0,s = 0;

	Packet rcvpkt, sndpkt;
	FILE* fp;

	//TODO: recv file name
	//	1: rdt_rcv && check sqno_
	while(1){
		rd = recvfrom(serv_sock, &rcvpkt, sizeof(rcvpkt), 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		//  3: make packet(ack, !sqno_, checksum)
		sndpkt.sqno = sqno_;
		strcpy(sndpkt.data, "0");
	
		//  4: udp send
		s = sendto(serv_sock, &sndpkt, sizeof(sndpkt), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
		if((rd > 0) && (rcvpkt.sqno == sqno_)){
			if(sqno_ == 1)
				sqno_ = 0;
			else
				sqno_ = 1;
			rd = 0;
			break;
		}
	}
	//	2: extract data
	rcvpkt.data[strlen(rcvpkt.data)] = '\0';
	fp = fopen(rcvpkt.data, "wb");	

	//recv file size
	while(1){
		rd = recvfrom(serv_sock, &rcvpkt, sizeof(rcvpkt), 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        //  3: make packet(ack, !sqno_, checksum)
        sndpkt.sqno = sqno_;
        strcpy(sndpkt.data, "0");

        //  4: udp send
        s = sendto(serv_sock, &sndpkt, sizeof(sndpkt), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
        if((rd > 0) && (rcvpkt.sqno == sqno_)){
            if(sqno_ == 1)
                sqno_ = 0;                                                                                 else
				sqno_ = 1;
            rd = 0;
            break;
        }
    }
	//	2: extract data
	file_len = atoi(rcvpkt.data);
	
	//TODO: recv file data
	while(file_len > len){
		//  1: rdt_rcv && check sqno_
		while(1){
			rd = recvfrom(serv_sock, &rcvpkt, sizeof(rcvpkt), 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
			//  3: make packet(ack, !sqno_, checksum)
			sndpkt.sqno = sqno_;
			strcpy(sndpkt.data, "0");

			//  4: udp send
			s = sendto(serv_sock, &sndpkt, sizeof(sndpkt), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
			if((rd > 0) && (rcvpkt.sqno == sqno_)){
				if(sqno_ == 1)
					sqno_ = 0;
				else
					sqno_ = 1;
				break;
			}
			printf("hoho");
		}
		//	2: extract data
		//printf("rcv file size %d, %d\n", rd,  strlen(rcvpkt.data));
		len += rcvpkt.data_size;
		fwrite(rcvpkt.data, rcvpkt.data_size, 1, fp);
	}
	fclose(fp);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
