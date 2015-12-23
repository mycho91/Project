#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<fcntl.h>
#include<time.h>
#include<signal.h>
#include"ledRing.c"
#include"wiringfnd.c"
#include"adxl345.c"
//#include<Python.h>

int connectServer(void);
void loop();
void checkDay();
bool readFile();
bool getca();
bool writeFile(int, int);
void ledDraw();
bool getCount();
void killChild(int signo);


int set = 0;
int counterSet = 0;
bool complete = false;
bool setFlag = false;
time_t timer;
int count = -1;
int pid = 0;
int mypipe_w[2];
int mypipe_r[2];


time_t tempTimer;	// for count test
time_t tempTimer2;	// for count test

int main()
{
	//Py_Initialize();
	//PyRun_SimpleString("from myled import *");
	//good();
	if(pipe(mypipe_r) == -1) return 0;
	pipe(mypipe_w);
	wiringPiSetup();
	accInit();
	pid = fork();
	if(pid == -1)		//exit when fork error
		return 0;
	else if(pid == 0)	//child task
	{
		time(&tempTimer2);	//Init
		readFile();
		ledInit(set);
		ledDraw();
		while(1) loop();
	}
	else				//parent task
	{
		signal(SIGINT, (void *)killChild);
		while(1) connectServer();
	}
	return 0;	//unreachable code
}

void killChild(int signo) // for Ctrl+c command
{
	kill(pid,SIGINT);
	exit(0);
}

void loop()
{
	static char buf[20];
	static char buf1[20];
	static char buf2[20];
	int i;
	if(getCount())		//true when count++
	{
		if(count == 12)	//update Set
		{
			setFlag = true;
			count = 0;
			set++;
			if(set > 9) set = 0;
			writeFile(0,set);
		}
		printf("set : %d, count : %d\n",set,count);
		ledDraw();
	}
	if(getca())	//true when Every Seconds
	{
		if(set >= 3 && setFlag)	//send 'update Set message to server'
		{
			setFlag = false;
			sprintf(buf,"Set %d",set);
			write(mypipe_r[1],buf,sizeof(buf));
		}
		else	//send alive Message to server
		{
			sprintf(buf,"Live 1");
			write(mypipe_r[1],buf,sizeof(buf));
		}
		for(i=0;i<20;i++)
		{
			buf[i] = '\0';
			buf1[i] = '\0';
			buf2[i] = '\0';
		}
		if(read(mypipe_w[0],buf,sizeof(buf)) > 0)	//update CounterSet
		{
			sscanf(buf,"%s %s",buf1, buf2);
			if(strcmp(buf1,"None") == 0) return;
			counterSet = atoi(buf2);
			printf("counterSet = %d!\n",counterSet);
		}
	}
}

bool getCount()
{
	static float max = 0, min = 900;
	static int check = 0;
	bool flag = false;	//return flag
	float s = getS();
	if(max<s) max = s;
	if(min>s) min = s;

	if(min < 200 && max > 400 && s < 300 && s > 220) // default value is close to 250 (because of gravity)
	{
		max = 0;
		min = 900;
		check++;
	}
	if(check == 2)
	{
		flag = true;
		count++;
		check = 0;
	}
	return flag;
}

/*											//Useless function because LED MATRIX is not working
void checkDay()	//distict each day
{
	static time_t nowtime = time(NULL);
	static time_t oldtime = time(NULL);	

	nowtime = time(NULL);
	if(nowtime /(24*3600) != oldtime/(24*3600))
	{
		oldtime = nowtime;
		counterSet = 0;
		set = 0;
		writeFile(0,set);
		writeFile(1,nowtime);
	}
}
*/
bool getCa()	//seconds count
{
	bool flag = false;
	time(&tempTimer);
	if(tempTimer != tempTimer2)
	{
		flag = true;
		tempTimer2 = tempTimer;
		count++;
	}
	return flag;
}

void ledDraw()	//draw LED Ring and LED number
{
	fndControl(set);		//led number

	matrix_setValue(count,set);	//led ring
	matrix_render();
	ws2811_render(&ledstring);
}

bool writeFile(int idx, int number)	//write set information to file
{
	int fd;
	char buf[30];
	int off = 0;
	if(idx==1) off = 2;
	sprintf(buf,"%d\n",number);
	fd = open("file.txt",O_WRONLY);
	lseek(fd,off,SEEK_SET);
	write(fd,(void *)buf,strlen(buf));
	close(fd);
	return true;
}

bool readFile()	//read information from file when program Init
{
	int fd;
	char buf[30];
	char tmpSet;
	char tmpTime[30];
	fd = open("file.txt",O_RDONLY);
	read(fd,(void *)buf,29);
	sscanf(buf,"%c\n%s",&tmpSet,tmpTime);
	set = tmpSet-'0';
	if(set >= 3) setFlag = true;
	timer = atoi(tmpTime);
	close(fd);
	printf("read set : %d, timer = %ld\n",set,timer);
	return true;
}

int connectServer(void)	// connect to Server.
{
	int sock,i;
	char buf[20];
	char buf1[20];
	char buf2[20];
	char ip[] = "220.70.1.92";
	char live[] = "Live 1";
	struct sockaddr_in serv_addr;
	
	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock == -1)
		return -1;
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(9191);
	inet_pton(AF_INET,ip,&(serv_addr.sin_addr));
	
	printf("connect to %s\n",ip);
	if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
	{
		printf("why?\n");	//Server closed
		return -1;
	}
	printf("connected!\n");
	while(write(sock,live,strlen(live)) <= 0);
	while(1)
	{
		for(i=0;i<20;i++)
		{
			buf[i] = '\0';
			buf1[i] = '\0';
			buf2[i] = '\0';
		}
		if(read(mypipe_r[0],buf,sizeof(buf)) > 0){	//send 'set update message' to server
			sscanf(buf,"%s %s",buf1,buf2);
			if(strcmp(buf1,"Set") == 0)
				printf("set!\n");
			if(write(sock,buf,strlen(buf)) == -1) return -1;
		}
		for(i=0;i<20;i++)
		{
			buf[i] = '\0';
			buf1[i] = '\0';
			buf2[i] = '\0';
		}
		if(recv(sock,buf,sizeof(buf),MSG_DONTWAIT) > 0)	//update conterSet
		{
			if(buf[0] == EOF) return -1;
			if(sscanf(buf,"%s %s",buf1,buf2) == 2)
			{
				printf("%s\n",buf);
				printf("buf1 : %s, buf2 : %s\n",buf1,buf2);
				if(strcmp(buf1,"Set") == 0)
					write(mypipe_w[1],buf,strlen(buf));
			}
			else //It Depend childTask from 'read block'
				write(mypipe_w[1],"None 1",7);
		}
		else //It Depend childtask from 'read block'
			write(mypipe_w[1],"None 1",7);
		
	}
	close(sock);
	return 1;
}
