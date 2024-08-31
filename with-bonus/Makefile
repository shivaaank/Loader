#invoke make inside following directories and in this order: loader, launch, fib
all:
#move the lib_simpleloader.so and launch binaries inside bin directory
	cd loader && $(MAKE) loader
	 
	cd launcher && $(MAKE) launch

	cd test && $(MAKE) 
	

#Provide the command for cleanup
clean:

	cd test && $(MAKE) clean
	
	cd bin && rm launch

	cd bin && rm lib_simpleloader.so

	cd loader && $(MAKE) clean
	
	
