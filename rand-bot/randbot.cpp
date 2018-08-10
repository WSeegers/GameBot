
/*
 * First C++ program that says hello (hello.cpp)
 */
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

void get_coord(int coord[3])
{
	coord[0] =  rand() % 8;
	coord[1] =  rand() % 8;
	if (coord[0] < 3)
		coord[2] = 2;
	else if (coord[0] < 6)
		coord[2] = 1;
	else
		coord[2] = 0;
}

int main()
{
	ofstream cmd;
	int coord[3];

	srand(time(NULL));
	get_coord(coord);
	cmd.open("command.txt");
	cmd << coord[0] << ',';
	cmd << coord[1] << ',';
	cmd << coord[2];
	cmd.close();
	return (0);
}      