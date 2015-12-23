#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<signal.h>
#include<stdbool.h>

void openServer();
void sighandler(int signo);
void ErrorHandling(const char *);

int mypipe1[2];	// USER 1 Write, USER 2 Read
int mypipe2[2]; // USER 2 Write, USER 1 Read
int mypipeP[2];	// USER 1,2 Write, Parent Read
int pid1=0;		// USER 1 pid
int pid2=0;		// USER 2 pid
int set = 0;	// Input Set

bool connection = false;	//true when Connected

int main()
{
	pipe(mypipe1);
	pipe(mypipe2);
	pipe(mypipeP);
	openServer();
	return 0;
}

void sighandler(int signo)		//for SIGUSR1. Parent announce connection to USER
{
	connection = true;
}

void openServer()
{
	int i, clnt, serv_sock, clnt_sock, optval, pid;
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_sz;
	char buf[30];			//buf -> buf1 + buf2
	char buf1[20];
	char buf2[20];
	
	serv_sock = socket(PF_INET, SOCK_STREAM,0);
	if(serv_sock < 0)
		ErrorHandling("socket Error!");
	
	setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,(char *)&optval,sizeof(optval));

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(9191);
	serv_addr.sin_addr.s_addr = inet_addr("220.70.1.92");	//We Use fixed ip '220.70.1.92'.

	if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		ErrorHandling("bind Error!");
	
	if(listen(serv_sock,5) < 0)
		ErrorHandling("listen Error!");

	clnt_addr_sz = sizeof(clnt_addr);
	while(1)
	{
		clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_addr, &clnt_addr_sz);
		if(clnt_sock < 0)
			continue;
		read(clnt_sock,(void *)buf,sizeof(buf));		//Read message
		sscanf(buf,"%s %S",buf1, buf2);
		clnt = atoi(buf2);								//Read Serial Number of dumbell In message (In this Project, only 1 and 2)
		signal(SIGUSR1,sighandler);					
		pid = fork();									//for New USER
		if(pid != 0)									//Parent Task
		{
			read(mypipeP[0],buf,sizeof(buf));			//read disconnection message from USER
			if(sscanf(buf,"%s %s",buf1,buf2) == 2)		//if all USER disconnected
			{
				pid1 = 0;
				pid2 = 0;
			}
			else if(sscanf(buf,"%s %s",buf1,buf2) == 0);//if no message
			else if(buf1[0] = '1') pid1 = 0;			//Update disconnection.
			else pid2 = 0;
			if(clnt == 1) pid1 = pid;	//Update pid of USER
			else pid2 = pid;
			if(pid1 != 0 && pid2 != 0)		//announce connection to USER
			{
				kill(pid1,SIGUSR1);
				kill(pid2,SIGUSR1);
			}
		}
		else											// USER Task
		{
			signal(SIGUSR1,sighandler);
			time_t timer,nowTime,oldTime;
			bool setflag = false;
			time(&timer);
			time(&nowTime);
			oldTime = nowTime;
			timer += 10;
			write(mypipeP[1]," ",2);					//send 'no message'. It Depend Parant from 'read() block'.
			while(timer > time(NULL))					//use custom Timer for connection.
			{
				for(i=0;i<20;i++)
				{
					buf[i] = '\0';
					buf1[i] = '\0';
					buf2[i] = '\0';
				}
				if(recv(clnt_sock,(void *)buf, sizeof(buf),MSG_DONTWAIT) > 0)	//read clnt Message
				{
					if(buf[0] == EOF) {
						close(clnt_sock);
						printf("EOF!\n");
						exit(0);
					}
					sscanf(buf,"%s %s",buf1,buf2);
					if(strcmp(buf1,"Live") == 0)		//if connection alive
					{
						//printf("%d live!\n",clnt);
						timer = time(NULL) + 10;		//update timer
					}
					else	//when set update
					{
						printf("%d buf : %s\n",clnt,buf);
						set = atoi(buf2);	//update set
						setflag = true;		//true when set update
					}
				}
				for(i=0;i<20;i++) buf[i] = '\0';
				time(&nowTime);
				if(connection && (oldTime != nowTime))	//when connected && every second
				{
					oldTime = nowTime;
					//printf("serverTimer\n");
					if(clnt == 1)	//when USER1
					{
						if(setflag)		//update set of other USER
						{
							sprintf(buf,"%d",set);
							setflag = false;
							write(mypipe1[1],buf,2);
						}
						else write(mypipe1[1],"",1);	//It Depend USER2 from 'read block'
						if(read(mypipe2[0],buf,sizeof(buf)) > 0)	//read message from USER2
						{
							if(sscanf(buf,"%s",buf1) == 0);	//no message
							else
							{
								if(buf1[0] == 'e') connection = false;	//connection end message
								else
								{
									sprintf(buf,"Set %c",buf1[0]);		//Update set message
									write(clnt_sock,buf,6);
								}
							}
						}
					}
					else	//same as upper, USER! -> USER2, USER2 -> USER1
					{
						if(setflag)
						{
							sprintf(buf,"%d",set);
							setflag = false;
							write(mypipe2[1],buf,2);
						}
						else write(mypipe2[1],"",1);
						if(read(mypipe1[0],buf,sizeof(buf)) > 0)
						{
							if(sscanf(buf,"%s",buf1) == 0);
							else
							{
								if(buf1[0] == 'e') connection = false;
								else
								{
									sprintf(buf,"Set %c",buf1[0]);
									write(clnt_sock,buf,6);
								}
							}
						}
					}
				}
			}
			printf("timer over!\n");
			if(clnt==1)		//send END message to other USER
				write(mypipe1[1],"e",2);
			else write(mypipe2[1],"e",2);
			sprintf(buf,"%d ",clnt);	//send END message to Parent
			write(mypipeP[1],buf,2);
			close(clnt_sock);
			exit(0);		//exit child process
		}
	}
}

void ErrorHandling(const char *message)
{
	fprintf(stderr,"%s\n",message);
	exit(0);
}
