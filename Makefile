
FLAGS = -std=c++11

mypython: main.o ast.o tokenizer.o
	g++ $(FLAGS) main.o ast.o tokenizer.o -o mypython

main.o: main.cpp
	g++ $(FLAGS) -c main.cpp

ast.o: ast.cpp ast.h
	g++ $(FLAGS) -c ast.cpp

tokenizer.o: tokenizer.cpp tokenizer.h
	g++ $(FLAGS) -c tokenizer.cpp

run: mypython
	./mypython

clean:
	rm *.o mypython