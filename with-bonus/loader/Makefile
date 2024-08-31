#Create lib_simpleloader.so from loader.c
loader:
	gcc -m32 -c -Wall -Werror -fpic loader.c
	gcc -m32 -shared -o ../bin/lib_simpleloader.so loader.o

#Provide the command for cleanup
clean:
	rm loader.o