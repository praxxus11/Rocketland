# all: compile link run

# compile:
# 	g++ -I../SFML-2.5.1/include -c *.cpp 

# link:
# 	g++ *.o -o main -L../SFML-2.5.1/lib -lsfml-graphics -lsfml-window -lsfml-system

# run: 
# 	main
all: compile link run

compile:
	g++ -Iinclude -IC:/Users/Eric/Downloads/eigen-3.3.9/* -c *.cpp

link:
	g++ *.o -o main -Llib -lsfml-graphics -lsfml-window -lsfml-system

run: 
	main