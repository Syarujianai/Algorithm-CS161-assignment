/* P2_Assign2.cpp : 

The file contains an adjacency list representation of an undirected weighted graph with 200 vertices labeled 1 to 200. 
Each row consists of the node tuples that are adjacent to that particular vertex along with the length of that edge.
For example, the 6th row has 6 as the first entry indicating that this row corresponds to the vertex labeled 6. The 
next entry of this row "141,8200" indicates that there is an edge between vertex 6 and vertex 141 that has length 8200.
The rest of the pairs of this row indicate the other vertices adjacent to vertex 6 and the lengths of the corresponding
edges.

Your task is to run Dijkstra's shortest-path algorithm on this graph, using 1 (the first vertex) as the source vertex, 
and to compute the shortest-path distances between 1 and every other vertex of the graph. If there is no path between 
a vertex vvv and vertex 1, we'll define the shortest - path distance between 1 and vvv to be 1000000.

You should report the shortest - path distances to the following ten vertices, in order : 7, 37, 59, 82, 99, 115, 133, 
165, 188, 197. You should encode the distances as a comma - separated string of integers.So if you find that all ten of
these vertices except 115 are at distance 1000 away from vertex 1 and 115 is 2000 distance away, then your answer should
be 1000, 1000, 1000, 1000, 1000, 2000, 1000, 1000, 1000, 1000. Remember the order of reporting DOES MATTER, and the string
should be in the same order in which the above ten vertices are given.The string should not contain any spaces.Please type
your answer in the space provided.

IMPLEMENTATION NOTES : This graph is small enough that the straightforward O(mn)O(mn)O(mn) time implementation of Dijkstra's
algorithm should work fine. OPTIONAL: For those of you seeking an additional challenge, try implementing the heap-based 
version. Note this requires a heap that supports deletions, and you'll probably need to maintain some kind of mapping 
between vertices and their positions in the heap.

*/

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <set>
#include <utility>  // <pair>
#include <ctime>
#include <limits>
#include <functional>

using namespace std;

#define INF std::numeric_limits<long long>::max()  // inf in C++, refer: https://stackoverflow.com/questions/8690567/setting-an-int-to-infinity-in-c
vector<long long> s; // greedy score

/**@syaru
*/
class AdjacencyList
{
public:
	AdjacencyList(ifstream &f);

	int num_vertices;	
	map<int, list<pair<int, long long>>> edges;  // pair: (vertex, length), refer: https://www.coursera.org/learn/algorithms-graphs-data-structures/discussions/weeks/2/threads/Jor7yTsEEeeAgQrkDbjJSA	
};

AdjacencyList::AdjacencyList(ifstream &f) {
	// Read data from file stream and instantiate graph object.
	size_t max_id = 0;
	int head, tail;
	long long length;
	string line;
	while (getline(f, line)) {
		stringstream ss(line);
		ss >> head;	
		if (head > max_id) max_id = head;
		while (ss >> tail>> length)
		{
			this->edges[head].push_back(make_pair(tail, length));
		}
	}
	this->num_vertices = max_id;
	f.close();
}

/**@syaru
* UnionFind DataStructure used to get rid of repeatedly graph's copy operation.
* Motivated by: https://www.coursera.org/learn/algorithms-divide-conquer/discussions/weeks/4/threads/rStE-lz7EeeMcg4GWtvWkg
* Refer code: https://zhuanlan.zhihu.com/p/29035169
***************************************************************************************************************************
* makeSet(int v): create a single vertex union set.
* unionSet(int v1, int v2): merge 2 union set.
* findSet(int v): return v's belonging vertex.
***************************************************************************************************************************
*/
class UnionFind {
public:
	void makeSet(int v);
	void unionSet(int v1, int v2);
	int findSet(int v);
	map<int, int> v_parent;
};

void UnionFind::makeSet(int v) {
	// Root -1.
	this->v_parent[v] = -1; 
}

void UnionFind::unionSet(int v1, int v2) {
	int root1 = findSet(v1);
	int root2 = findSet(v2);
	// Select left union set's root as representative.
	v_parent[root2] = root1;
}

int UnionFind::findSet(int v) {
	// If key v not found.
	if (v_parent.find(v) == v_parent.end())
		return -1;
	if (this->v_parent[v] == -1)
		return v;
	// Compress path for later O(1) finding.
	this->v_parent[v] = findSet(this->v_parent[v]);
	return this->v_parent[v];
}

/**@syaru
* Dijkstra's shortest path alogrithm.
*/
void Dijkstra(AdjacencyList &g, UnionFind &x, int source) {
	// Initializing.
	x.makeSet(source);
	s.assign(g.num_vertices + 1, INF);
	s[source] = 0;
	// n-1 iterations.
	for (int i = 0; i < g.num_vertices - 1; i++) {		
		int r_x = 0;  // Root of X.
		// Store cross edges' greedy score in current iteration, and decresing score by <map>, 
		// refer: https://www.cnblogs.com/lakeone/p/5599047.html
		map<long long, int, less<long long>> s_current;
		for (int j = 1; j <= g.num_vertices; j++) {
			// If the vertex is in X.
			if (x.findSet(j) != -1) {
				// Remember root of X in first encounter.
				if (r_x == 0) r_x = x.findSet(j);
				// Traversal all v->X, w->V. (i.e., frontier)
				auto beg = g.edges[j].begin(), end = g.edges[j].end();
				for (auto it = beg; it != end; it++) {
					// If the vertex is not in X.
					if (x.findSet(it->first) == -1) {
						int w_candidate = it->first; 
						long long len = it->second;
						// Compute greedy score, and latently ignore local loser if there is multi-mapping.
						s_current.insert(make_pair(s[j] + len, w_candidate));				
					}
				}			
			}
		}
		// Add global winner w_star to X.
		int w_winner = s_current.begin()->second;
		// Update greedy score of winner w_star.
		s[w_winner] = s_current.begin()->first;
		x.makeSet(w_winner);
		x.unionSet(r_x, w_winner);
	}
}

int main()
{
	// TSETING run time of code, 
	// refer: https://blog.csdn.net/ijn842/article/details/80703277
	time_t begin, end;
	// Read data from text file.
	ifstream fin("dijkstraData.txt");
	// Create graph.
	begin = clock();
	AdjacencyList g(fin);
	end = clock();
	cout << "runtime: " << double(end - begin) / CLOCKS_PER_SEC << endl;
	// Dijkstra's shortest path algorithm.
	begin = clock();
	UnionFind x; int source = 1;
	Dijkstra(g, x, source);
	end = clock();
	cout << "runtime: " << double(end - begin) / CLOCKS_PER_SEC << endl;
	// Output the shortest paths from source 1 to terminate 7, 37, 59, 82, 99, 115, 133, 165, 188, 197.
	vector<int> t { 7, 37, 59, 82, 99, 115, 133, 165, 188, 197 };
	for (auto i : t) cout << s[i] << ",";
	cout << endl;

	return 0;
}


