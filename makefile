all: server rfs

server: server.c
	gcc server.c  serverFunc.c -o server -lpthread

rfs: client.c
	gcc client.c clientFunc.c -o rfs

clean:
	rm server rfs folder/*.txt getFolder/*.txt