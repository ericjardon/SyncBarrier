finally: program
	./program

test: testprogram
	./testprogram

program: barrier.o matrix.o
	gcc -o program barrier.o matrix.o -pthread

testprogram: barrier.o mm.o
	gcc -o testprogram barrier.o mm.o -pthread

barrier.o:
	gcc -c -pthread barrier.c

matrix.o:
	gcc -c -pthread matrix.c

mm.o:
	gcc -c -pthread mm.c

clean:
	rm barrier.o matrix.o program

testclean:
	rm barrier.o mm.o testprogram