all: talker manager

manager: manager.o nom.h
	gcc -o manager manager.o

manager.o: manager.c nom.h
	gcc -c manager.c

talker: talker.o nom.h
	gcc -o talker talker.o

talker.o: talker.c nom.h
	gcc -c talker.c
	
clean:
	rm talker.o manager.o
	
