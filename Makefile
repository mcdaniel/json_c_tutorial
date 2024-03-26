#
# File          : Makefile
# Description   : Build file for curses tutorial
# Created       : Thu Jan  4 08:41:48 EST 2024
# By            : Patrick Mcdaniel

# Environment
CFLAGS=-g -Wall
CC=gcc

# Suffix rules
.c.o :
	${CC} -c ${CFLAGS} $< -o $@

#
# Project Protections

jtest : jtest.o
	$(CC) -g -o $@ jtest.o -ljson-c

clean : 
	rm -f jtest jtest.o
