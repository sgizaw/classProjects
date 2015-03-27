
myhttpd: init.o log.o network.o main.o thread.o filehandle.o
	gcc -g -o myhttpd main.o init.o log.o network.o filehandle.o thread.o -lpthread

init.o:	init.c init.h
	gcc -c -Wall init.c

log.o:	log.c log.h
	gcc -c -Wall log.c

network.o: network.c network.h
	gcc -c -Wall network.c

thread.o: thread.c thread.h
	gcc -c -Wall thread.c

filehandle.o: filehandle.c filehandle.h
	gcc -c -Wall filehandle.c

main.o:	main.c main.h init.h network.h thread.h log.h filehandle.h
	gcc -c -Wall main.c
##=====================================================
clean:
	@- $(RM) myhttpd 
	@- $(RM) *~ *.o	
	@- echo “Data Cleansing Done.Ready to Compile”
