all: compile link run

compile:
	g++ -Iinclude -IC:/Users/Eric/Downloads/eigen-3.3.9/* -c *.cpp

link:
	g++ *.o -o main -Llib -lsfml-graphics -lsfml-window -lsfml-system

run: 
	main