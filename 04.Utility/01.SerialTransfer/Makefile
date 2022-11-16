all: NetworkTransfer clean

NetworkTransfer: NetworkTransfer.c
	gcc -o $@ $<
	cp $@ ../../

clean:
	rm -f NetworkTransfer