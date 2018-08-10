#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
using namespace std;

enum e_buildingType
{
	non, defence, attack, energy, tesla
};

class MapCell
{
	private:
		e_buildingType	type;
		bool			isConstructing;

	public:
		
		MapCell(e_buildingType type, bool isConstructing)
		{
			this->type = type;
			this->isConstructing = isConstructing;
		}

		void	set(e_buildingType pType, bool pIsConstructing)
		{
			type = pType;
			isConstructing = pIsConstructing;
		}

		e_buildingType	getType()	{ return (type); }
		bool			getConstructing() { return (isConstructing); }
};

class GameMap
{
	private:
		size_t			mapWidth;
		size_t			mapHeight;

	public:
		vector<MapCell>	map;
	

	public: 
		GameMap(size_t mapWidth, size_t mapHeight)
		{
			this->mapWidth = mapWidth;
			this->mapHeight = mapHeight;
			map.reserve(mapWidth * mapHeight);
		}

		void	setCell(size_t x, size_t y, e_buildingType type, bool isConstructing)
		{
			MapCell	*cell;

			cell = getCell(x, y);
			cell->set(type, isConstructing);
		}

		void	setCell(size_t x, size_t y, MapCell newCell)
		{
			MapCell	*cell;

			cell = getCell(x, y);
			*cell = newCell;
		}

		MapCell *getCell(int x, int y)	{ return (&map[y * mapWidth + x]); }
		size_t	getWidth()				{ return (mapWidth);}
		size_t	getHeight()				{ return (mapHeight);}
};

MapCell	getType(char c)
{
	e_buildingType	type;
	bool			isConstructing;

	isConstructing = islower(c);
	switch (toupper(c))
	{
		case('E'):
			type = energy;
			break;
		case('A'):
			type = attack;
			break;
		case('D'):
			type = defence;
			break;
		case('T'):
			type = tesla;
		default:
			type = non;
	}
	return (MapCell(type, isConstructing));
}

int	parseMap(GameMap *map)
{
	string		input;
	ifstream	textMap("textMap.txt");
	string		token;
	int			score;

	while (getline(textMap, input))
	{
		if (input == "############# MAP #############")
		{
			while (getline(textMap, input))
			{	
				int		start;
				int		x, y;
				char	type;

				if (input == "###############################")
					break;

				while ((start = input.find("[")) != string::npos)
				{
					token = input.substr(start, input.find("]") + 1);
					input = input.substr(start + 1);
					x = stoi(token.substr(1));
					y = stoi(token = token.substr(token.find(",") + 1));
					type = token.substr(token.find(",") + 1)[0];
					map->setCell(x, y, getType(type));
				}
			}
		}

		if (input == "---------- PLAYER A ----------")
		{
			getline(textMap, input);
			getline(textMap, input);
			getline(textMap, input);
			score = stoi(input.substr(input.find(":") + 1));
		}
	}
	textMap.close();
	return (score);
}

class Node
{
	private:
		double			value;
		vector<Node*>	edges;
		vector<double>	weights;
		bool			evaluated;

	public:
		Node(double value, bool isSensor)
		{
			this->value = value;
			evaluated = isSensor;
		}

		void	add(double value)
		{
			this->value += value; 
		}

		void	addEdge(Node *source, double weight)
		{	
			for (vector<Node*>::iterator i = edges.begin(); i < edges.end(); i++)
			{
				if (source == *i)
					return ;
			}
			edges.push_back(source);
			weights.push_back(weight);
		}

		void	evaluate()
		{
			if (evaluated || edges.empty())
			{
				evaluated = true;
				return ;
			}

			vector<double>::iterator w = weights.begin();
			for (vector<Node*>::iterator i = edges.begin(); i < edges.end(); i++)
			{
				if (!(*i)->isEvaluated())
					(*i)->evaluate();
				// printf("w: %f, ", *w, (*i)->getValue())
				add(*w * (*i)->getValue());
				w++;
			}
			evaluated = true;
		}

		bool	isEvaluated(void)	{ return (evaluated);}
		double	getValue(void)		{ return (value);}		
};

class Net
{
	public:
		vector<Node>	sensor;
		vector<Node>	output;	

	public:

		Net(size_t sensors, size_t outputs)
		{
			while (sensors--)
				this->sensor.push_back(Node(0, true));
			while (outputs--)
				this->output.push_back(Node(0, false));
		}

		void evaluate(void)
		{
			for (vector<Node>::iterator it = output.begin(); it < output.end(); it++)
			{
				it->evaluate();
			}
		}

		int		getMax()
		{
			double	max = 0;
			int		iMax = 0;
			Node	node(0, true);

			for (int i = 0; i < output.size(); i++)
			{
				node = output[i];
				if (node.getValue() > max)
				{
					max = node.getValue();
					iMax = i;
				}
			}
			if (max == 0)
				return (-1);
			return (iMax);
		}

		void	setSensor(size_t x, size_t y, e_buildingType type, double value)
		{
			sensor[x * y * type].add(value);
		}
};

#define WIDTH	16
#define HEIGHT	8
#define TOWERS	4
#define OPTS	5

#define HWIDTH 	(WIDTH / 2)
#define INPUT	(WIDTH * HEIGHT * TOWERS)
#define OUTPUT	(HWIDTH * HEIGHT * OPTS)


void printResult(int result)
{
	int res[3];

	ofstream out("command.txt");
	if (result < 0)
		out << endl;
	res[2] = result / (HWIDTH * HEIGHT);
	res[1] = (result % (HWIDTH * HEIGHT)) / HWIDTH;
	res[0] = (result % (HWIDTH * HEIGHT)) % HWIDTH;
	out << res[0] << "," << res[1] << "," << res[2] << endl; 
}

int		main(void)
{
	GameMap map(WIDTH, HEIGHT);
	MapCell	*cell;
	parseMap(&map);

	Net net(INPUT, OUTPUT);

	// Set sensors based on map
	int counter = 0;
	int offset;
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
		{
			MapCell *cell = map.getCell(x, y);
			net.setSensor(x, y, cell->getType(), 1.0);
		}
	
	string line;
	ifstream genome("genome.dna");
	getline(genome, line);
	while(getline(genome, line))
	{
		int 	o;
		int 	s;
		double	w;

		s = stoi(line.substr(line.find("s") + 1));
		o = stoi(line.substr(line.find("o") + 1));
		w = stod(line.substr(line.find("w") + 1));
		net.output[o].addEdge(&net.sensor[s], w);
	}
	net.evaluate();
	printResult(net.getMax());
}