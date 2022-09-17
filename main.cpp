#include"GBase.h"
#include <string>
#include<iostream>

struct Vertex
{
	int number_of_coins;
	std::string get_data_as_str()
	{
		return std::to_string(number_of_coins);
	}
};
struct Edge
{
	double the_length_of_the_path_to_the_chest;
	std::string get_data_as_str()
	{
		return std::to_string(the_length_of_the_path_to_the_chest);
	}
};

int main() 
{

	Graph<std::string, Vertex, Edge> graph;
	graph.InsertVertex(
		"1", 
		{ .number_of_coins = 100},
		{
			{"2",{.the_length_of_the_path_to_the_chest=10.0}},
			{"5",{.the_length_of_the_path_to_the_chest = 20.0}},
		});
	graph.InsertVertex(
		"2",
		{ .number_of_coins = 20 },
		{

		});
	graph.InsertVertex(
		"5",
		{ .number_of_coins = 30 },
		{

		});
	graph.SendAdjListToOSTREAM();
	graph.SendVertexesInfoToOSTREAM();
	graph.SendEdgesInfoToOSTREAM();
	graph.DeleteVertex("5");
	graph.SendAdjListToOSTREAM();
	graph.SendVertexesInfoToOSTREAM();
	graph.SendEdgesInfoToOSTREAM();
	return 0;
}