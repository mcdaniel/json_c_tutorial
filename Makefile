#
# File          : Makefile
# Description   : Build file for curses tutorial
# Created       : Thu Jan  4 08:41:48 EST 2024
# By            : Patrick Mcdaniel

#
# Project Protections

jtest : jtest.o
	gcc -o $@ jtest.o -ljson-c

clean : 
	rm -f jtest jtest.o
