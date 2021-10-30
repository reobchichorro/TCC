FLAGS = -std=c++17 -g -O3

all : main.out #clear.o

clear.o:
	rm *.o

main.out: main.o Utils.o Defs.o
	g++ main.o Terrain.o Utils.o GuardType.o -o main.out $(FLAGS)

Defs.o: Utils.o
	g++ -c Domain/Terrain.cpp $(FLAGS)
	g++ -c Domain/GuardType.cpp $(FLAGS)

Utils.o:
	g++ -c Utils/Utils.cpp $(FLAGS)

main.o: main.cpp
	g++ -c main.cpp $(FLAGS)

clean:
	rm -r -f *.o *.out
