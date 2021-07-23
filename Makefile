all: compile link run

compile:
	g++ -I../SFML-2.5.1/include -c *.cpp 

link:
	g++ *.o -o main -L../SFML-2.5.1/lib -lsfml-graphics -lsfml-window -lsfml-system

run: 
	main