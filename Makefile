all : main.out clear.o

clear.o:
	rm *.o

main.out: main.o Data.o Utils.o
	g++ main.o Terrain.o str_manipulation.o -o main.out -std=c++17

Data.o:
	g++ -c Data/Terrain.cpp -std=c++17

Utils.o:
	g++ -c Utils/str_manipulation.cpp -std=c++17

main.o: main.cpp
	g++ -c main.cpp -std=c++17

clean:
	rm -r -f *.o *.out
