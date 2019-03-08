/* P2_Assign1.cpp : 
The file contains the edges of a directed graph.Vertices are labeled as positive integers from 1 to 875714. 
Every row indicates an edge, the vertex label in first column is the tail and the vertex label in second column 
is the head(recall the graph is directed, and the edges are directed from the first column vertex to the second 
column vertex).So for example, the 11th11^{ th }11th row looks liks : "2 47646".This just means that the vertex 
with label 2 has an outgoing edge to the vertex with label 47646

Your task is to code up the algorithm from the video lectures for computing strongly connected components(SCCs), 
and to run this algorithm on the given graph.

Output Format : You should output the sizes of the 5 largest SCCs in the given graph, in decreasing order of sizes, 
separated by commas(avoid any spaces).So if your algorithm computes the sizes of the five largest SCCs to be 
500, 400, 300, 200 and 100, then your answer should be "500,400,300,200,100" (without the quotes).
If your algorithm finds less than 5 SCCs, then write 0 for the remaining terms.Thus, if your algorithm computes 
only 3 SCCs whose sizes are 400, 300, and 100, then your answer should be "400,300,100,0,0" (without the quotes). 
(Note also that your answer should not have any spaces in it.)
*/

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <queue>
#include <ctime>

using namespace std;

// Global variable:
// 1. find SCCs.
size_t finishing_time = 0;
int vertex_start = NULL;
vector<bool> is_explored;
vector<int> order_topo;
vector<int> leader_meta;
// 2. compute and remeber the sizes of largest SCCs.
priority_queue<int> SCC_size;
size_t finishing_time_single_loop = 0;


/**@syaru
* DATA STRUCTURE - adjacency list of given graph specialized in DFS.
*/
class AdjacencyList
{
public:
	AdjacencyList(ifstream &f);
	//~AdjacencyList();
	
	int num_vertices;
	// vector -> two bidirectional lists, 1: "g", 2: 'grev'.
	map<int, map<string, list<int>>> edges;
};


/**@syaru
* Variant of graph creation function in Assign4.cpp.
*/
// Note: 1. randomize the orders of selected vertices when TESTING.
// 2. exchange the graph and reverse graph when TESTING.
// 3. set IDE saved stack size to 2147483648 Bytes and soultion to x64.
// Refer: https://blog.csdn.net/u010452239/article/details/70238105.
AdjacencyList::AdjacencyList(ifstream &f) {
	// Read data from file stream and instantiate graph object.
	size_t max_id = 0;
	int tail, head;
	while (f >> tail >> head) {
		if (tail > max_id) max_id = tail;
		this->edges[tail]["g"].push_back(head);
		this->edges[head]["grev"].push_back(tail);
	}
	this->num_vertices = max_id;
	f.close();
}


//AdjacencyList::AdjacencyList(const vector<vector<int>> &all) {
//	size_t max_id = 0;
//	for (auto &line : all) {
//		if (line[0] > max_id) max_id = line[0];
//		// line[0]: tail, line[1]: head.
//		this->edges[line[0]]["g"].push_back(line[1]);
//		this->edges[line[1]]["grev"].push_back(line[0]);
//	}
//	this->num_vertices = max_id;
//}


/**@syaru
* DFS subroutine will be recursively called by DFSLoop.
*/
void DFS(AdjacencyList &g, int i, int t) {
	is_explored[i] = true;
	// First pass or second pass.
	if (t == 0) {
		// Attention: judge whether current vertex has incoming edges in 'g' or not.
		if (g.edges[i].find("grev") != g.edges[i].end()) {
			// Traversal all reverse edges of vertex i.
			list<int> &edges = g.edges[i]["grev"];
			for (auto it = edges.begin(); it != edges.end(); it++) {
				if (is_explored[*it] == false) {
					DFS(g, *it, t);
				}
			}		
		}
		// First pass or second pass.
		finishing_time++;
		// Trick: reversal reflect so that needn't to sort when second pass. 
		order_topo[finishing_time] = i;
	}else {
		// Set leader of current SCC.
		leader_meta[i] = vertex_start;
		// Attention: judge whether current vertex has outgoing edges in 'g' or not.
		if (g.edges[i].find("g") != g.edges[i].end()) {
			// Traversal all forward edges of vertex i.
			list<int> &edges = g.edges[i]["g"];
			for (auto it = edges.begin(); it != edges.end(); it++) {
				if (is_explored[*it] == false) {
					DFS(g, *it, t);
				}
			}
		}
		finishing_time_single_loop++;
	}
}

#pragma optimize( "", off )
/**@syaru
* Kosaraju's two pass SCC algorithm.
*/
void DFSLoop(AdjacencyList &g) {
	// First pass: descending order of vertex_id.
	// Mark all vertices unexplored.
	is_explored.assign(g.num_vertices + 1, false);
	order_topo.assign(g.num_vertices + 1, NULL);
	for (int i = g.num_vertices; i > 0; i--) {
		// Attention: judge whether current vertex is explored or not before DFS.
		if (is_explored[i] == false) {
			DFS(g, i, 0);
		}
	}

	// Second pass: descending order of topological order.
	// Mark all vertices unexplored.
	is_explored.assign(g.num_vertices + 1, false);
	leader_meta.assign(g.num_vertices + 1, -1);
	for (int i = g.num_vertices; i > 0; i--) {
		// DFS by descending finishing time of first pass.
		vertex_start = order_topo[i];
		// Reset finishing time of single loop.
		finishing_time_single_loop = 0;
		DFS(g, vertex_start, 1);
		// DATA STRUCTURE - priority queue to remember top-5 largest SCCs.
		// Refer: https://www.cnblogs.com/zwfymqz/p/7800654.html
		SCC_size.push(finishing_time_single_loop);
	}	
}


int main()
{	
	// TSETING run time of code.
	// Refer: https://blog.csdn.net/ijn842/article/details/80703277.
	time_t begin, end;
	// Read data from text file.
	ifstream fin("SCC.txt");
	// Create graph.
	begin = clock();
	AdjacencyList g(fin);
	end = clock();
	cout << "runtime: " << double(end - begin) / CLOCKS_PER_SEC << endl;
	// Find and compute the sizes of top-5 largest SCCs.
	begin = clock();
	DFSLoop(g);
	end = clock();
	cout << "runtime: " << double(end - begin) / CLOCKS_PER_SEC << endl;
	// Output the sizes of top-5 largest SCCs.
	for (int i = 0; (!SCC_size.empty()) && (i < 5); i++) {
		cout << SCC_size.top() << endl;
		SCC_size.pop();
	}

    return 0;
}

