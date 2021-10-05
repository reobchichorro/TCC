FLAGS = -std=c++17 -g

all : main.out clear.o

clear.o:
	rm *.o

main.out: main.o Utils.o Classes.o Situation.o Solver.o
	g++ main.o Terrain.o Utils.o GuardType.o Situation.o Solver.o -o main.out $(FLAGS)

Utils.o:
	g++ -c Utils/Utils.cpp $(FLAGS)

Solver.o: Classes.o Utils.o Situation.o
	g++ -c Classes/Solver.cpp $(FLAGS)

Situation.o: Classes.o Utils.o
	g++ -c Classes/Situation.cpp $(FLAGS)

Classes.o: Utils.o
	g++ -c Classes/Terrain.cpp $(FLAGS)
	g++ -c Classes/GuardType.cpp $(FLAGS)

main.o: main.cpp
	g++ -c main.cpp $(FLAGS)

clean:
	rm -r -f *.o *.out
