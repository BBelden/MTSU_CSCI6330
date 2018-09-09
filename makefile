all:	lab6

lab6:	lab6.c
	mpicc lab6.c -lm -o lab6

clean:   
	rm lab6
	

       
