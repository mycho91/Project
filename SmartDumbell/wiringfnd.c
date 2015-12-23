#include <stdio.h>
#include <wiringPi.h>

int fndControl(int num)
{
	int i;
	int gpiopins[4] = {10,6,5,4};
	int number[10][4]={ {0,0,0,0},
						{0,0,0,1},
						{0,0,1,0},
						{0,0,1,1},
						{0,1,0,0},
						{0,1,0,1},
						{0,1,1,0},
						{0,1,1,1},
						{1,0,0,0},
						{1,0,0,1}};
	for(i=0;i<4;i++){
		pinMode(gpiopins[i],OUTPUT);
	};

	for(i=0;i<4;i++){
		digitalWrite(gpiopins[i],number[num][i]?HIGH:LOW);
	};

	for(i=0;i<4;i++){
		digitalWrite(number[num][i],LOW);
	}

	return 0;
}
