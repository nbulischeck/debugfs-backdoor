default: all

all:
	gcc -o client client.c

run:
	@echo "Sudo is required to send packets on raw sockets"
	sudo ./client -i 127.0.0.1 -p 80 -f example.py

clean:
	rm client
