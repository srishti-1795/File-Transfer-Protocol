all: a.c
	gcc a.c -lssl -lcrypto
