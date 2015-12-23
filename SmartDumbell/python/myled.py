#!/usr/bin/python

import time
import datetime
from Adafruit_8x8 import EightByEight

# ===========================================================================
# 8x8 Pixel Example
# ===========================================================================
grid = EightByEight(address=0x70)

print "Press CTRL+Z to exit"

# Continually update the 8x8 display one pixel at a time
#while(True):
#  for x in range(0, 8):
#    for y in range(0, 8):
#      grid.setPixel(x, y)
def good():
	while(True):
		grid.setPixel(0,2)
		grid.setPixel(0,3)
		grid.setPixel(0,4)
		grid.setPixel(0,5)
		grid.setPixel(7,2)
		grid.setPixel(7,3)
		grid.setPixel(7,4)
		grid.setPixel(7,5)
		grid.setPixel(2,0)
		grid.setPixel(3,0)
		grid.setPixel(4,0)
		grid.setPixel(5,0)
		grid.setPixel(2,7)
		grid.setPixel(3,7)
		grid.setPixel(4,7)
		grid.setPixel(5,7)
		grid.setPixel(1,1)
		grid.setPixel(1,6)
		grid.setPixel(6,1)	
		grid.setPixel(6,6)
		grid.setPixel(5,5)
		grid.setPixel(5,2)
		grid.setPixel(2,3)
		grid.setPixel(2,4)
		grid.setPixel(3,2)
		grid.setPixel(3,5)

def soso():
	while(True):
		grid.setPixel(0,2)
		grid.setPixel(0,3)
		grid.setPixel(0,4)
		grid.setPixel(0,5)
		grid.setPixel(7,2)
		grid.setPixel(7,3)
		grid.setPixel(7,4)
		grid.setPixel(7,5)
		grid.setPixel(2,0)
		grid.setPixel(3,0)
		grid.setPixel(4,0)
		grid.setPixel(5,0)
		grid.setPixel(2,7)
		grid.setPixel(3,7)
		grid.setPixel(4,7)
		grid.setPixel(5,7)
		grid.setPixel(1,1)
		grid.setPixel(1,6)
		grid.setPixel(6,1)
		grid.setPixel(6,6)
		grid.setPixel(5,5)
		grid.setPixel(5,2)
	
		grid.setPixel(2,2)
		grid.setPixel(2,3)
		grid.setPixel(2,4)
		grid.setPixel(2,5)

def bad():
	while(True):
		grid.setPixel(0,2)
		grid.setPixel(0,3)
		grid.setPixel(0,4)
		grid.setPixel(0,5)
		grid.setPixel(7,2)
		grid.setPixel(7,3)
		grid.setPixel(7,4)
		grid.setPixel(7,5)
		grid.setPixel(2,0)
		grid.setPixel(3,0)
		grid.setPixel(4,0)
		grid.setPixel(5,0)
		grid.setPixel(2,7)
		grid.setPixel(3,7)
		grid.setPixel(4,7)
		grid.setPixel(5,7)
		grid.setPixel(1,1)
		grid.setPixel(1,6)
		grid.setPixel(6,1)
		grid.setPixel(6,6)
		grid.setPixel(5,5)
		grid.setPixel(5,2)
	
		grid.setPixel(2,2)
		grid.setPixel(2,5)
		grid.setPixel(3,3)
		grid.setPixel(3,4)
def perfect():
	while(True):
		grid.setPixel(5,0)
		grid.setPixel(6,1)
		grid.setPixel(5,2)
		grid.setPixel(5,7)
		grid.setPixel(6,6)
		grid.setPixel(5,5)
		grid.setPixel(2,0)
		grid.setPixel(1,1)
		grid.setPixel(0,2)
		grid.setPixel(2,7)
		grid.setPixel(1,6)
		grid.setPixel(0,5)
		grid.setPixel(0,3)
		grid.setPixel(0,4)
	
print good()		
#      time.sleep(0.05)
#  time.sleep(0.5)
#  grid.clear()
#  time.sleep(0.5)
