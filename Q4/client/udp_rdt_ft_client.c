#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>

#define MAX 512
#define GETTIME 2.0

typedef struct _Packet{
	int sqno;
	char data[MAX];
	int data_size;
}Packet;

void error_handling(char *message);
void send_binary_file(int sock, char* filename);

int main(int argc, char *argv[])
{
	int sock;
	struct timeval tv;
	tv.tv_sec = GETTIME;
	tv.tv_usec = 0;
	socklen_t adr_sz;
	
	struct sockaddr_in serv_adr, from_adr;
	if(argc!=4){
		printf("Usage : %s <IP> <port> <filename>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_DGRAM, 0);

	//TIMEOUT GETTIME
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));

	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

	send_binary_file(sock, argv[3]);

	close(sock);
	return 0;
}

void send_binary_file(int sock, char* filename){

	struct stat file_info;
	struct timeval send_strt, send_end;
	
	int file_bin, file_size, left_size;
	int name_len = 0, len = 0, s = 0, rd = 0, read_size = 0, nfile = 0, sum_buf = 0;
	int sqno_ = 0;

	long sum_time = 0;

	Packet sndpkt, rcvpkt;
	pid_t rcv_pid[2];	

	file_bin = open(filename, O_RDONLY);
	stat(filename, &file_info);
	
	file_size = (int) file_info.st_size;
	if(!file_bin){
		perror("Error : ");
		exit(1);
	}

	//TODO: send file name

	//		1. make_pkt
	sndpkt.sqno = sqno_;
	strcpy(sndpkt.data, filename);
	
	//		2. udp_send
	while(1){
		s = write(sock, &sndpkt, sizeof(sndpkt));
		rd = read(sock, &rcvpkt, sizeof(MAX));

		printf("%d %d ", rd,rcvpkt.sqno);
		//  3. start_timer & (timeout || stop_timer)
		if((rd > 0) && rcvpkt.sqno == sqno_){
			if(sqno_ == 1){
               	   		sqno_ = 0;
			}else{
           			sqno_ = 1;
			}
			break;
		}
	}
	printf("%s %d\n", sndpkt.data, sqno_);
	//send file size
	 //      1. make_pkt
    sndpkt.sqno = sqno_;
    sprintf(sndpkt.data, "%d",file_size);

    //      2. udp_send
    while(1){
        s = write(sock, &sndpkt, sizeof(sndpkt));
        rd = read(sock, &rcvpkt, sizeof(MAX));
        //  3. start_timer & (timeout || stop_timer)
        if((rd > 0) && rcvpkt.sqno == sqno_){
            if(sqno_ == 1)
				sqno_ = 0;
			else
				sqno_ = 1;
            break;
        }
    }		

	//TODO: send file data MAX byte per cycle
	gettimeofday(&send_strt, NULL);
	while(len < file_size){
		//	1. make_pkt
		read_size = read(file_bin, sndpkt.data, MAX);
		len += read_size;
	
		sndpkt.sqno = sqno_;
		sndpkt.data_size = read_size;
		//	2. udp send
		while(1){
			s = write(sock, &sndpkt, sizeof(sndpkt));
			rd = read(sock, &rcvpkt, sizeof(rcvpkt));
			//  3. start_timer & (timeout || stop_timer)
			if(rd > 0){
            	if(rcvpkt.sqno == sqno_){
                	if(sqno_ == 1)
                        sqno_ = 0;
                	else
                    	sqno_ = 1;
                	break;
            	}else{
                	fprintf(stderr, "Different ACK! retransmission");
            	}
        	}
			else{
            	fprintf(stderr, "timeout && retransmission\n");
        	}		
		}
	}
	gettimeofday(&send_end, NULL);
	close(file_bin);
	sum_time = (send_end.tv_sec - send_strt.tv_sec)*1000000 + (send_end.tv_usec - send_strt.tv_usec);
	fprintf(stderr, "-----------------------------------------\n   elapsed time: %lf sec\n   throughput: %lf bps\n-----------------------------------------\n", (double)sum_time /1000000.0, (double)(file_size * 8)/((double)sum_time/1000000.0));
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

