#BY SUBMITTING THIS FILE TO CARMEN, I CERTIFY THAT I HAVE STRICTLY ADHERED
#TO THE TENURES OF THE OHIO STATE UNIVERSITY’S ACADEMIC INTEGRITY POLICY
#WITH RESPECT TO THIS ASSIGNMENT.


# comments in a Makefile start with sharp 

all: lab5.zip lab5 

# this target is the .zip file that must be submitted to Carmen
lab5.zip: Makefile bounded_buffer.c bounded_buffer.h main.c
	zip lab5.zip Makefile bounded_buffer.c bounded_buffer.h word_count.c word_count.h main.c hashmap.c hashmap.h
	
lab5: main.o bounded_buffer.o hashmap.o word_count.o
	gcc main.o bounded_buffer.o hashmap.o word_count.o -o lab5 -lpthread -lrt

hashmap.o: main.c
	gcc -ansi -pedantic -g -c hashmap.c

	
word_count.o: main.c
	gcc -g -c word_count.c -lpthread -lrt

bounded_buffer.o: main.c
	gcc -ansi -pedantic -g -c bounded_buffer.c

	
main.o: main.c
	gcc -ansi -pedantic -g -c main.c 

# this target deletes all files produced from the Makefile
# so that a completely new compile of all items is required
clean: 
	rm -rf *.o lab5 lab5.zip 
	


