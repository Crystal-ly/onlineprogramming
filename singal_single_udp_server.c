#include <stdio.h> // 标准输入输出
#include <stdlib.h> //标准库
#include <string.h> //字符数组相关函数
#include <sys/types.h> //基本系统数据类型
#include <sys/socket.h> //提供socket函数及数据结构
#include <netinet/in.h> //定义数据结构sockaddr_in
#include <unistd.h> //系统调用接口
#include <arpa/inet.h> //提供IP地址转换函数
#include<time.h>//获取时间信息
#include <signal.h>
#include <fcntl.h>
#define BUFFER_LENGTH 1024

typedef void (*sighandler_t)(int);

int sockfd;

void sig_io_handler(int signo){
//	
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(struct sockaddr);
	int n;
//定义时间的数据结构
	time_t timep;
	struct tm *p;
//定义接收缓存buf,长度为BUFFER_LENGTH
	char buf[BUFFER_LENGTH];
//定义发送缓存buf_1,长度为BUFFER_LENGTH
	char buf_1[BUFFER_LENGTH];
//无限循环，收发数据
	memset(buf, 0, BUFFER_LENGTH);
	memset(buf_1, 0, BUFFER_LENGTH);
	printf("信号驱动%d",signo);
// 清空buf变量存储空间
	n = recvfrom(sockfd, buf, BUFFER_LENGTH, 0, (struct sockaddr *)&client_addr, &client_addr_len);
//以阻塞I/O的方式从网络接收数据
//所接收数据的源地址存放在
// client_addr套接字地址信息结构体变量
	if(n == -1){perror("fail to receive");exit(1);}
	else{
		buf[n] = '\0';
		printf("接受order成功%s\n",buf);
		time(&timep);
		p=gmtime(&timep);
//打包获取的时间信息
		if(strcmp(buf,"Gettime")==0){
			sprintf(buf_1,"%d:%d:%d",8+p->tm_hour,p->tm_min,p->tm_sec);
			//printf("%s\n",buf_1);
		}else if(strcmp(buf,"Getdate")==0){
			sprintf(buf_1,"%d.%d.%d",(p->tm_year)+1900,1+p->tm_mon,p->tm_mday);
			//printf("%s\n",buf_1);
		}else if(strcmp(buf,"Getdatetime")==0){
			sprintf(buf_1,"%d.%d.%d %d:%d:%d",(p->tm_year)+1900,1+p->tm_mon,p->tm_mday,8+p->tm_hour,p->tm_min,p->tm_sec);
			//printf("%s\n",buf_1);
		}else if(strcmp(buf,"close")==0){
			puts("UDP Server is closed!\n");
			exit(0);
		}else{
			strcpy(buf_1,"wrong order");
		}	
	n = sendto(sockfd, buf_1, strlen(buf_1), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
//以阻塞I/O方式向网络发送数据
//发送数据的目的地，由client_addr指定
	printf("时间发送成功");
	}
}



int main(int argc, char *argv[]) {
	int file_flags;
//定义套接字地址信息结构体变量
	struct sockaddr_in server_addr;
	if (argc != 3){
		fputs("usage: ./udpserver serverIP serverPort\n", stderr);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//创建UDP套接字
	if (sockfd == -1){
		perror("created udp socket");
		exit(1);
	}
	signal(SIGIO, sig_io_handler);//注册处理函数
	fcntl(sockfd, F_SETOWN, getpid()); //设置套接字的属主
	file_flags = fcntl(sockfd, F_GETFL);
	fcntl(sockfd, F_SETFL, file_flags | O_ASYNC);
	bzero(&server_addr, sizeof(server_addr));
//套接字地址信息结构体变量初始化清0
	server_addr.sin_family = AF_INET;
//指定网络协议类型为IP协议
	server_addr.sin_port = htons(atoi(argv[2]));
//设置端口号
	if (inet_aton(argv[1], &server_addr.sin_addr) == 0){
//设置IP地址
		perror("inet_aton");
		exit(1);
	}
	printf("套接字初始化成功\n");
	if (bind(sockfd, (struct sockaddr *)&server_addr,sizeof(server_addr)) == -1){
//将地址信息绑定到套接字
		perror("call to bind");
		exit(1);
	}
	printf("bind成功\n");
//无限循环	
	while(1){
		sleep(1000);
	}
	return EXIT_SUCCESS;
}