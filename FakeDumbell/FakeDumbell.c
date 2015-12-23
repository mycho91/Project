#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<time.h>

int set = 2;
int count = 0;

int connectServer(void);
int main()
{
	connectServer();	
	return 0;
}

int connectServer()
{
	int sock;
	char buf[500];
	char buf1[20];
	char buf2[20];
	char ip[] = "220.70.1.92";	//use fixed ip
	char live[] = "Live 2";
	struct sockaddr_in serv_addr;
	time_t timer,oldtimer;
	time(&timer);
	oldtimer = timer;

	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock == -1)
		return -1;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(9191);
	inet_pton(AF_INET,ip,&(serv_addr.sin_addr));

	printf("connecct to %s\n",ip);
	if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("why?\n");
		return -1;
	}
	printf("connected!\n");
	while( write(sock,live,sizeof(live)) <= 0);
	while(1)
	{
		time(&timer);
		if(timer != oldtimer)	//Assume action at each 2 second
		{
			count += 2;
			if(count == 12)	//when action 12 times (1 set)
			{
				count = 0;
				set++;
				set = set%10;
				sprintf(buf,"Set %d",set); //update set
				write(sock,buf,6);
			}
			else
				write(sock,"Live 2",7);	//alive message to server every second
			oldtimer = timer;
		}
		if(recv(sock,buf,sizeof(buf),MSG_DONTWAIT) > 0)	//read update message from server. (other USER's set update)
			if(buf[0] >= '0' && buf[0] <= '9')printf("buf : %s\n",buf);
	}
}
