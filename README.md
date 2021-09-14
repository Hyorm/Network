# Network(Study)
study network algorithm with C and x86

## Summary
### Q1    
Implement the TCP-based file transfer program as shown below.
- C language
- Client sends the file name and contents of the file to the server.
- Server receives file name and store the received data from the client.
- Server is iterative type.(Missing Version)
- Server and client program should support both ASCII-based file and binary file.
- Do not use sleep function in the programs.
- After uploading the file, the client program should display the elapsed time and throughput.    
#### Usage
Server: 
<code> # ./tcp_ft_server <port></code><br>
Client:
<code> # ./tcp_ft_client <IP> <port> <filename></code>

### Q2
Implement UDP-based file transfer program. Convert the programs in Q1 to the UDP-based file transfer programs.
- C language    
#### Usage
Server: 
<code> # ./udp_ft_server <port></code><br>
Client:
<code> # ./udp_ft_client <IP> <port> <filename></code>

### Q3
Implement the program using “gethostbyname()” as shown below.
- You should implement the program using gethostbyname().
- Source code filename: hostname.c
- Your program should display all of aliases, address type, and IP addresses.
#### Usage
<code> # ./hostname <URL> </code>
```
(example)
# ./hostname www.naver.com
Official name: e60630.a.akamiedge.net
Aliases 1: www.naver.com
Aliases 2: www.naver.com.nheos.com
Aliases 3: www.naver.com.edgekey.net
Address type: AF_INET
IP addr 1: 23.46.23.18
```

### Q4
Implement the stop-and-wait protocol-based file transfer program over UDP as shown below.
- You should improve the UDP-based file transfer program that you implemented for Q2.
- You should implement stop-and-wait protocol for reliable data transfer.
- For the stop-wait-protocol, you can consider the rdt 3.0 protocol in the Chapter 3.
- You don’t need to check the packet corruption. You just implement the packet retransmission for the packet
loss.
- You should implement the both server and client program using C language. And upload your source codes
in the server following the guide.
- Client sends the file name and contents of the file to the server.
- Server receives file name and store the received data from the client.
- Server and client can finish the program after file receiving.
- Server and client program should support both ASCII-based file and binary file.
- After uploading the file, the client program should display the elapsed time and throughput.
- Your programs should follow the usage below.
#### Usage
Server: 
<code># ./udp_rdt_ft_server <port></code><br>
Client: 
<code># ./udp_rdt_ft_client <IP> < port> <filename></code>
- Your programs can freely display the status of file transferring, e.g., transmission, retransmission, ACK,
and timeout, etc.
