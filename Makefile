finally: program
	./program

test: testprogram
	./testprogram

program: barrier.o matrix_.o
	gcc -o program barrier.o matrix_.o -pthread

testprogram: barrier.o mm.o
	gcc -o testprogram barrier.o mm.o -pthread

barrier.o:
	gcc -c -pthread barrier.c

matrix_.o:
	gcc -c -pthread matrix_.c

mm.o:
	gcc -c -pthread mm.c

clean:
	rm barrier.o matrix_.o program

testclean:
	rm barrier.o mm.o testprogram