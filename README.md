senmqd
======

Gpio Inputs distributed subscribable via zmq

Currently waits for input on Gpio 17, 23 and 24. 
23 and 24 are interpreted as the two switches of a dial of an oldschool telephone.
The subscription on the W: channel, where dialed numbers are distributed is still left to
be fully implemented

## Before build
check submodule dependency....

## Build
	autoreconf -si
	./configure ...
	make

## Configuration
... is done in a compiled file for now... This is subject of future improvement.

## Use
see client example on  how to subscribe for notifictions of a specific gpio.





