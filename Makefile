main.out: solution.out target.out
	./solution.out
	./target.out
	cat summary.txt
solution.out: solution.c
	gcc -o solution.out solution.c
target.out: target.c
	gcc -o target.out target.c

clean:
	rm -f *.o
	rm -f *.txt
	rm -f *.out

