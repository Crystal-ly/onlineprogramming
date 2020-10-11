#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUFFER_LENGTH 1024

int main(int argc, char *argv[]) {
//定义套接字地址信息结构体变量
	struct sockaddr_in server_addr;
	int sockfd;
	socklen_t server_addr_len = sizeof(struct sockaddr);
//定义缓存buf,长度为BUFFER_LENGTH
	char buf[BUFFER_LENGTH];
	int n;
	if (argc != 3){
		fputs("usage: ./udpclient serverIP serverPort\n", stderr);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1){
		perror("created udp socket...\n");
		exit(1);
	}
//套接字地址信息结构体变量初始化清0
	bzero(&server_addr, sizeof(server_addr));
//指定网络协议类型为IP协议
	server_addr.sin_family = AF_INET;
//设置端口号
	server_addr.sin_port = htons(atoi(argv[2]));
//设置IP地址
	if (inet_aton(argv[1], &server_addr.sin_addr) == 0){
		perror("inet_aton");
		exit(1);
	}
	memset(buf, 0, BUFFER_LENGTH);
	printf("Please input your order:");
	scanf("%s",&buf);
//发送order	
	n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (n == -1){
		perror("fail to send");
		exit(1);
	}
	printf("发送成功\n");
//清空buf
	memset(buf, 0, BUFFER_LENGTH);
	n = recvfrom(sockfd, buf, BUFFER_LENGTH, 0, (struct sockaddr *)&server_addr, &server_addr_len);
	if (n == -1){
		perror("fail to receive");
		exit(1);
	}
	else{
	buf[n] = '\0';
	printf("Server: %s\n", buf);
	}
	if (close(sockfd) == -1){
		perror("fail to close");
		exit(1);
	}
	puts("UDP Client is closed!\n");
	return EXIT_SUCCESS;
}