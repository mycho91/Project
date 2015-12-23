/*  
 *  ADXL345 Acelerometer for Raspberry Pi
 *
 *  Copyright (C) 2015 EverPi - everpi[at]tsar[dot]in
 *  blog.everpi.net 
 * 
 *  This file is part of adxl345.
 *
 *  adxl345 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  adxl345 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with adxl345.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <wiringPiI2C.h>

#define X_REG 0x32
#define Y_REG 0x34
#define Z_REG 0x36

short int axis_sample_average(int axis, int fd);
short int axis_sample(int axis,int fd);

int main(int argc, char *argv[]){
	
	int fd = 0;
	short int data = 0;	
	short int data2 = 0;
	int datasimple = 0;	

	fd = wiringPiI2CSetup(0x53);
	
	datasimple = wiringPiI2CReadReg8(fd,0x31);
	wiringPiI2CWriteReg8(fd,0x31,datasimple|0xb);
	
	wiringPiI2CWriteReg8(fd,0x2d,0x08); //POWER_CTL	
	usleep(11000);
	// erase offset bits
	wiringPiI2CWriteReg8(fd,0x1e,0);
	wiringPiI2CWriteReg8(fd,0x1f,0);
	wiringPiI2CWriteReg8(fd,0x20,0);
	usleep(11000);
	// calibrate X axis
	data = axis_sample_average(X_REG,fd);
	wiringPiI2CWriteReg8(fd,0x1e,-(data / 4));
	// calibrate Y axis
	data = axis_sample_average(Y_REG,fd);
	wiringPiI2CWriteReg8(fd,0x1f,-(data / 4));
	// calibrate Z axis
	data = axis_sample_average(Z_REG,fd);
	wiringPiI2CWriteReg8(fd,0x20,-((data - 256 ) / 4));
	
	usleep(100000);


    while(1){

	fprintf(stderr,"x:%f\n",axis_sample(X_REG,fd) / 128.0); // X
	fprintf(stderr,"y:%f\n",axis_sample(Y_REG, fd) / 128.0); // Y	
	fprintf(stderr,"z:%f\n\n",axis_sample(Z_REG,fd) / 128.0); // Z
    	usleep(100000);
    }

	return 0;
}

short int axis_sample(int axis,int fd){

	short int data = 0;
	short int data2 = 0;
	usleep(10000);
	data  =  wiringPiI2CReadReg8(fd,axis);
	data2 =  wiringPiI2CReadReg8(fd,axis+1); 
	return ( (data2<<8)|data );
}

short int axis_sample_average(int axis, int fd){

	int c = 10;
	int value = 0;
	
	while(c--){
		value += axis_sample(axis, fd);
	}
	
	return ( value/10 );
}
