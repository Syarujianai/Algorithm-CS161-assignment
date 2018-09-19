/* Assign4.cpp :

The file contains the adjacency list representation of a simple undirected graph. 
There are 200 vertices labeled 1 to 200. The first column in the file represents the vertex label, 
and the particular row (other entries except the first column) tells all the vertices that the vertex is adjacent to. 
So for example, the 6th row looks like : "6 155 56 52 120 ......". 
This just means that the vertex with label 6 is adjacent to (i.e., shares an edge with) the vertices with labels 155,56,52,120,......,etc

Your task is to code up and run the randomized contraction algorithm for the min cut problem and use it on the above graph to compute the min cut.
(HINT: Note that you'll have to figure out an implementation of edge contractions. 
Initially, you might want to do this naively, creating a new graph from the old every time there's an edge contraction.
But you should also think about more efficient implementations.) 
(WARNING: As per the video lectures, please make sure to run the algorithm many times with different random seeds, and remember the smallest cut that you ever find.) 
Write your numeric answer in the space provided. So e.g., if your answer is 5, just type 5 in the space provided.
*/

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <ctime>
#include <algorithm>
#include <stdexcept> //runtime error
using namespace std;

/*@syaru
	Based on https://www.cnblogs.com/ECJTUACM-873284962/p/6905416.html
*/
class AdjacencyList
{
public:
	AdjacencyList();
	~AdjacencyList();

	//store vertices of the edge 
	map<int, int> start; //start v of e
	map<int, int> end; //end v of e
	//store edges of the vertex
	map<int, int> first;//TODO: hash_map
	map<int, int> next;
	//TODO: data structure 'vertices_remain', combine list and tree to improve access efficiency.
    //Based on https://blog.csdn.net/romandion/article/details/2421370
	//Here, use vector for random acessing.
	vector<int> vertices_remain;
	vector<int> edges_remain;
	//id=0 represent None
	size_t edge_id = 0;//edge nums now
	size_t vertex_id = 0;//vertex nums now
	//mapping same edge id, used in 'mergeVertex()'
	map<int, int> same_edge;

	void add(vector<vector<int>>&);
	size_t minCutSearch(void);
};

AdjacencyList::AdjacencyList()
{
}

AdjacencyList::~AdjacencyList()
{
}

//TODO: improve efficiency of graph loading.
/*@syaru
	Edges only be stored in its start node. (Though here is undirected graph)
*/
void AdjacencyList::add(vector<vector<int>> &all) {
	for (auto &line : all) {
		this->vertex_id=line[0];
		//feed 'remain_vertices'
		this->vertices_remain.push_back(line[0]);
		this->first[this->vertex_id] = -1;
		for (int i = 1; i != line.size(); i++) {
			//number new edge
			this->edge_id++;
			this->edges_remain.push_back(this->edge_id);
			//map corresponding vertices
			this->start[this->edge_id] = this->vertex_id;
			this->end[this->edge_id] = line[i];
			//map corresponding edges
			int last_edge = this->first[vertex_id];//if i=1, last_edge=-1
			this->first[vertex_id] = this->edge_id;
			this->next[edge_id] = last_edge;

			//case: at the second time algorithm meet the same edge, mapping this edge_id to previous edge_id.
			if (find(vertices_remain.begin(), vertices_remain.end(), line[i]) != vertices_remain.end()) {
				int now = this->first[line[i]];				
				while (now != -1) {
					//if found, double-directedly (big_id->small_id + small_id->big_id ) mapping 2 same edges.
					if (this->end[now] == this->vertex_id) {
						same_edge[now] = this->edge_id;
						same_edge[this->edge_id] = now;
						break;
					}
					now = this->next[now];
				}
			}
		}
	}
}

/*@syaru
	edg_now: previous edge of the edge you wanna DEL.
*/
void delEdge(AdjacencyList &cp, int edg_now) {
	int temp = cp.next[cp.next[edg_now]];
	cp.next[cp.next[edg_now]] = 0;//deleted edge point to 0
	cp.start[cp.next[edg_now]] = 0;
	cp.end[cp.next[edg_now]] = 0;

	//erase value in 'edges vector'	
	auto iter = find(cp.edges_remain.begin(), cp.edges_remain.end(), cp.next[edg_now]);
	if (iter != cp.edges_remain.end()) cp.edges_remain.erase(iter);

	cp.next[edg_now] = temp;
}

void delEdgeOfVertex(AdjacencyList &cp, int vert_sta, int vert_end) {
	try {
		int now = cp.first[vert_sta];
		//case1: first edge, finally 'end[now] != vert_end'
		while (cp.end[now] == vert_end) {
			int temp = cp.next[now];
			cp.next[now] = 0;
			cp.start[now] = 0;
			cp.end[now] = 0;

			//erase value in 'edges vector'	
			auto iter = find(cp.edges_remain.begin(), cp.edges_remain.end(), now);
			if (iter != cp.edges_remain.end()) cp.edges_remain.erase(iter);

			now = cp.first[vert_sta] = temp;
		}

		//TEST: throw exception
		if (now == 0) throw runtime_error("now must be non-zero");//throw error	

		//case2: else edges 
		if (now != -1) { //skip when 'now == -1'
			while (cp.next[now] != -1) {
				if (cp.end[cp.next[now]] == vert_end)
					delEdge(cp, now);
				else now = cp.next[now];
			}
		}	
	}catch (runtime_error err) {//catch error
		cout << err.what() << endl;
		cout << vert_sta <<" "<< vert_end << endl;
	}
}

/*@syaru
	case1: find edge which start in 'vert_sta' and end in 'vert_end' in now;
	case2: find edge which start in 'vert_sta' and end in 'vert_end' in next[now].
*/
void delEdgeAndSelfLoop(AdjacencyList &cp, int vert_sta, int vert_end) {
	//traversal all edge of this start/end vertex, delete self loop
	delEdgeOfVertex(cp, vert_sta, vert_end);
	delEdgeOfVertex(cp, vert_end, vert_sta);	
}

void mergeVertex(AdjacencyList &cp, int vert_sta, int vert_end) {
	//Note: end id > start id
	int now = cp.first[vert_end];
	if (now != -1) { //skip when 'now == -1'
		cp.start[now] = vert_sta;//edges start from deleted node change to exist node
		cp.end[cp.same_edge[now]] = vert_sta;//edges end to deleted node
		while (cp.next[now] != -1) {
			//substitude end-point-edge's start node
			cp.start[cp.next[now]] = vert_sta;
			cp.end[cp.same_edge[cp.next[now]]] = vert_sta;
			now = cp.next[now];	
		}
		cp.next[now] = cp.first[vert_sta];
		cp.first[vert_sta] = cp.first[vert_end];
		cp.first[vert_end] = 0;

		//erase value in 'vertices vector'
		auto iter = find(cp.vertices_remain.begin(), cp.vertices_remain.end(), vert_end);
		if (iter != cp.vertices_remain.end()) cp.vertices_remain.erase(iter);
	}
	else {
		cp.first[vert_end] = 0;
		//erase value in 'vertices vector'
		auto iter = find(cp.vertices_remain.begin(), cp.vertices_remain.end(), vert_end);
		if (iter != cp.vertices_remain.end()) cp.vertices_remain.erase(iter);
	}	
}

/*@syaru
	1.minCutSearch() = delEdgeAndSelfLoop() + mergeVertex();
	2.naive approach (extra storage).
*/
//TODO: Visualize graph
size_t AdjacencyList::minCutSearch(void) {
	int min_edgs = 0, min_sta = 0;
	for (int i = 0, count = this->vertex_id*this->vertex_id; i < count; i++) {
		//copy a graph from object every contraction
		AdjacencyList copy = (*this);
		//reset seed, and then rand()
		srand((unsigned)time(NULL));//test: as i is random seed
		//srand(unsigned(i));
		while (copy.vertices_remain.size() != 2){
			//randomly sample and contract an edge
			int index = rand() % copy.edges_remain.size();
			int edge_id = copy.edges_remain[index];
			int start = copy.start[edge_id], end = copy.end[edge_id];
			//TODO: combine delEdgeAndSelfLoop() and mergeVertex() to run faster!
			delEdgeAndSelfLoop(copy, start, end);
			mergeVertex(copy, start, end);
		}
		//remember min cut (edge nums + start node)
		if (i == 0) {
			min_edgs = copy.edges_remain.size();
			min_sta = copy.start[copy.edges_remain[0]];
			cout << "min_sta: " << min_sta << ", " << "min_edges: " << min_edgs << endl;
		}
		else {
			int now_edgs = copy.edges_remain.size();
			if (now_edgs < min_edgs) {
				min_edgs = copy.edges_remain.size();
				min_sta = copy.start[copy.edges_remain[0]];
				cout << "min_sta: " << min_sta << ", " << "min_edges: " << min_edgs << endl;
			}
			else {
				cout << "failed! " << "min_sta: " << copy.start[copy.edges_remain[0]] << ", " << "min_edges: " << now_edgs << endl;
			}
		}				
	}
	return min_sta;
}

int main()
{
	ifstream fin("kargerMinCut.txt");
	ofstream fout;

	//read data from text file
	vector<vector<int>> all_data;
	string line;
	while (getline(fin, line)) {
		vector<int> line_data;
		stringstream ss(line); int temp;
		while (ss >> temp)
		{
			line_data.push_back(temp);
		}
		all_data.push_back(line_data);
	}
	fin.close();

	//create graph and search min cut
	AdjacencyList adj;
	adj.add(all_data);
	size_t min_cut_start_node = adj.minCutSearch();
	cout << "min_cut_sta: " << min_cut_start_node << endl;
	
    return 0;
}

