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
#include <algorithm> //find method
#include <stdexcept> //runtime error
#include <utility> //pair container
#include <set> //O(logn) search tree
//#include <ext/rb_tree> //g++ STL extension: rb_tree

using namespace std;

/*---------------------------------------------------------------------------------------------------*/
/**@syaru
 * Based on https://www.cnblogs.com/ECJTUACM-873284962/p/6905416.html
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
	map<int, int> first;//TODO: hash_map might be faster?
	map<int, int> next;
	//TODO: data structure 'vertices_remain', combine list and tree to improve access efficiency.
    //Based on: https://blog.csdn.net/romandion/article/details/2421370
	//Here, use vector for random acessing.
	vector<int> vertices_remain;
	vector<int> edges_remain;
	//Ps: id=0 represent None
	size_t edge_id = 0;//edge nums now
	size_t vertex_id = 0;//vertex nums now

	void add(vector<vector<int>>&);
	pair<int, int> minCutSearch(void);
};

AdjacencyList::AdjacencyList()
{
}

AdjacencyList::~AdjacencyList()
{
}

/**@syaru
 * Edges only be stored in its start node. (Though here is undirected graph)
 */
//TODO: improve efficiency of graph loading.
void AdjacencyList::add(vector<vector<int>> &all) {
	for (auto &line : all) {
		this->vertex_id = line[0];
		//feed 'remain_vertices'
		this->vertices_remain.push_back(line[0]);
		this->first[this->vertex_id] = -1;
		for (int i = 1; i != line.size(); i++) {
			//case1: at the first time (start node), number edge and feed 'remain_edges'
			if (find(this->vertices_remain.begin(), this->vertices_remain.end(), line[i]) == this->vertices_remain.end()) {
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
			}
			//case1 or case2?
			else {
				int now = this->first[line[i]];
				while (this->end.find(now) != this->end.end() && this->end[now] != this->vertex_id) {
					//case1
					if (now == -1) {
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
						break;
					}
					now = this->next[now];
				}
			}
		}
	}
}

/*---------------------------------------------------------------------------------------------------*/

/**@syaru
 * Adopt UnionFind DataStructure to get rid of repeatedly copy operation of graph object,
 * Motivated by: https://www.coursera.org/learn/algorithms-divide-conquer/discussions/weeks/4/threads/rStE-lz7EeeMcg4GWtvWkg
 * Refer code: https://zhuanlan.zhihu.com/p/29035169
 */
class UnionFind {
public:
	void makeSet(int v, AdjacencyList &adj); //create a single element Union set.
	void unionSet(int v1, int v2); //merge 2 Union set.
	int findSet(int v); //return Union set's representative.
	map<int, int> v_parent; //parental node, vertex Union set's representative.
	map<int, set<int>> e_set; //Union set-> edge tree of vertex Union set.
};

void UnionFind::makeSet(int v, AdjacencyList &adj) {
	this->v_parent[v] = -1; //root node -1
	//@syaru edition
	set<int> edgs;
	int now = adj.first[v];
	if (now != -1) {
		edgs.insert(now);
		while (adj.next[now] != -1) {
			edgs.insert(adj.next[now]);
			now = adj.next[now];
		}
	}
	this->e_set[v] = edgs;
}

void UnionFind::unionSet(int v1, int v2) {
	int root1 = findSet(v1);
	int root2 = findSet(v2);
	v_parent[root2] = root1; //select left union set's root as representative element of union
	//@syaru edition
	this->e_set[root1].insert(
		this->e_set[root2].begin(), this->e_set[root2].end());
}

int UnionFind::findSet(int v) {
	//not found
	if (v_parent.find(v) == v_parent.end())
		return -1;
	if (this->v_parent[v] == -1)
		return v;
	//return findSet(this->v_parent[v]); //TEST
	this->v_parent[v] = findSet(this->v_parent[v]); //compress path for later O(1) finding
	return this->v_parent[v];
}

/*---------------------------------------------------------------------------------------------------*/

void delEdge(UnionFind &u, AdjacencyList &adj, vector<int> &e_re, int u_sta, int u_end) {
	for (auto iter = u.e_set[u_sta].begin(); iter != u.e_set[u_sta].end();) {
		if (u.findSet(adj.end[*iter]) == u_end) {
			//erase edge in edges_remain - log(n), then shuffle.
			auto it = find(e_re.begin(), e_re.end(), *iter);
			if (it != e_re.end()) e_re.erase(it);
			//erase edge in edge set, based on: https://blog.csdn.net/u011261430/article/details/75043641.
			u.e_set[u_sta].erase(iter++);
		}
		else {
			iter++;
		}
	}
}

void delEdgeAndSelfLoop(UnionFind &u, AdjacencyList &adj, vector<int> &e_re, int v_sta, int v_end) {
	int u_sta = u.findSet(v_sta);
	int u_end = u.findSet(v_end);
	delEdge(u, adj, e_re, u_sta, u_end);
	delEdge(u, adj, e_re, u_end, u_sta);
}

/**@syaru
 * UnionFind() approach.
 */
//TODO: Visualize graph
pair<int,int> AdjacencyList::minCutSearch(void) {
	int min_edg_nums = 0, min_u_sta = 0, min_u_end = 0;
	int count_all = this->vertex_id*this->vertex_id*log(this->vertex_id);
	cout << count_all << endl;
	int count_each = vertex_id - 2;
	for (int i = 0; i < count_all; i++) {
		//initialize in every contraction
		vector<int> e_remain = this->edges_remain;
		UnionFind u_set;
		//reset seed, and then rand()
		srand((unsigned)time(NULL));
		//srand(unsigned(i)); //TEST: when i is the random seed
		for (int j = 0; j < count_each; j++) {
			//randomly sample and contract
			int index = rand() % e_remain.size();
			int edge_id = e_remain[index];
			int v_sta = this->start[edge_id], v_end = this->end[edge_id];			
			int rt1 = u_set.findSet(v_sta), rt2 = u_set.findSet(v_end);
			//case1: both of sta and end don't belong to sets
			if ((rt1 == -1) && (rt2 == -1)) {
				u_set.makeSet(v_sta, *this);
				u_set.makeSet(v_end, *this);
				delEdge(u_set, *this, e_remain, v_sta, v_end);
				u_set.unionSet(v_sta, v_end);
			}
			//case2: both of sta and end belong to sets
			else if ((rt1 != -1) && (rt2 != -1)) {
				delEdgeAndSelfLoop(u_set, *this, e_remain, v_sta, v_end);
				u_set.unionSet(v_sta, v_end);
			}
			//case3: sta or end belong to sets
			else {
				if (rt1 == -1) {
					u_set.makeSet(v_sta, *this);
					delEdgeAndSelfLoop(u_set, *this, e_remain, v_sta, v_end);
					u_set.unionSet(v_end, v_sta);
				}
				else {
					u_set.makeSet(v_end, *this);
					delEdgeAndSelfLoop(u_set, *this, e_remain, v_sta, v_end);
					u_set.unionSet(v_sta, v_end);
				}
			}
		}
		//remember min cut (edge nums + start node)
		if (i == 0) {
			min_edg_nums = e_remain.size();
			min_u_sta = u_set.findSet(this->start[e_remain[0]]);
			min_u_end = u_set.findSet(this->end[e_remain[0]]);

			//if one of vertices hasn't united before
			if (min_u_sta == -1) min_u_sta = this->start[e_remain[0]];
			else if (min_u_end == -1) min_u_end = this->end[e_remain[0]];

			cout << "min_sta: " << min_u_sta << ", "
				<< "min_end: " << min_u_end << ", "
				<< "min_edges: " << min_edg_nums << endl;
		}
		else {
			int now_edg_nums = e_remain.size();
			int now_u_sta = u_set.findSet(this->start[e_remain[0]]);
			int now_u_end = u_set.findSet(this->end[e_remain[0]]);
			
			//if one of vertices hasn't united before
			if (now_u_sta == -1) now_u_sta = this->start[e_remain[0]];
			else if (now_u_end == -1) now_u_end = this->end[e_remain[0]];

			if (now_edg_nums < min_edg_nums) {
				min_edg_nums = now_edg_nums;
				min_u_sta = now_u_sta;
				min_u_end = now_u_end;
				cout << "min_sta: " << min_u_sta << ", "
					<< "min_end: " << min_u_end << ", "
					<< "min_edges: " << min_edg_nums << endl;
			}
			else {
				cout << "failed, " << "min_sta: " << now_u_sta << ", "
					<< "min_end: " << now_u_end << ", "
					<< "min_edges: " << now_edg_nums << endl;;
			}
		}				
	}
	return make_pair(min_u_sta, min_u_end);
}


/*---------------------------------------------------------------------------------------------------*/
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
	pair<int,int> final_supervertex = adj.minCutSearch();
	cout << "min_cut_final: " << final_supervertex.first << ","
		<< final_supervertex.second << endl;
	
    return 0;
}

