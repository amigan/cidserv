all:
	cd src && make
	cd xcid && make
clean:
	cd src && make clean
	cd xcid && make clean
