/*
 A Class implementing Christofides and 2-opt algorithms for a polynomial-time
 solution to the Traveling Salesman Problem (TSP). A TSP is the shortest tour
 which visits once each point in a given set of points. The Christofides
 algorithm is guaranteed to return a solution which is longer than the optimal
 path by a factor of no more than 3/2. It does so in polynomial time, with an
 upper bound of roughly O(n^4).

 Solution researched at:
  https://www.codingalpha.com/christofides-algorithm-c-program/
  https://en.wikipedia.org/wiki/Christofides_algorithm
  https://conferences.mpi-inf.mpg.de/adfocs-15/material/Ola-Lect1.pdf
*/

#ifndef TSP_H
#define TSP_H

#include <iostream>
#include <vector>
#include <stack>
#include <limits>
#include <algorithm>

#include "utility.h"

// Solution.
struct Tour
{
	int cost{ 0 };
	std::vector<int> path{ 0 };
};

// TSP solver.
class TSP
{
	const size_t MIN_VERTICES{ 4 };
	const size_t MAX_VERTICES{ 2000 };

	// Number of vertices.
	size_t n{ 0 };

	// Graph node.
	class Node
	{
	public:
		Node(size_t v, size_t vv, int w) : v1(v), v2(vv), weight(w) { }
		size_t v1, v2; 
		int weight;
		bool removed{ false };
	};

	using AdjMatrix = std::vector<std::vector<int>>;
	using AdjList = std::vector<std::vector<Node>>;

	// Create minimum spanning tree.
	AdjList mst(const AdjMatrix& am)
	{
		AdjList adjList(n);
		std::vector<size_t> key(n, std::numeric_limits<size_t>::max());
		std::vector<int> parent(n);

		key[0] = 0;
		parent[0] = 0;
		for (size_t i = 0; i < n; i++)
		{
			int min = std::distance(key.begin(), std::min_element(key.begin(), key.end()));

			for (size_t j = 0; j < n; j++)
			{
				if (am[min][j] < (int)key[j])
				{
					key[j] = am[min][j];
					parent[j] = min;
				}
			}
		}
	
		for (size_t i = 1; i < n; i++)
		{
			size_t v2 = parent[i];

			adjList[i].emplace_back(Node{ i, v2, am[i][v2] });
			adjList[v2].emplace_back(Node{ v2, i, am[i][v2] });
		}

		return adjList;
	}

	// Perfect match of odd vertices (its not the minimal match, but its a small match).
	void match(AdjList& graph, std::vector<int> oddVertices, const AdjMatrix am)
	{
		int index = 0;
		for (size_t i = 0; i < oddVertices.size() / 2; i++)
		{
			int currVertex = oddVertices[index];
			while (currVertex == -1)
			{
				index++;
				currVertex = oddVertices[index];
			}
			int length = std::numeric_limits<int>::max();
			int nearestVert = -1;
			std::unique_ptr<int> nearestVertPtr = nullptr;

			for (auto j = oddVertices.begin(); j < oddVertices.end(); ++j)
			{
				if (*j != -1 && index != std::distance(oddVertices.begin(), j) && am[currVertex][*j] < length)
				{
					length = am[currVertex][*j];
					nearestVert = *j;
					nearestVertPtr = std::make_unique<int>(*j);
				}
			}

			oddVertices[index] = -1;
			*nearestVertPtr = -1;

			graph[currVertex].emplace_back(Node{ (size_t)currVertex, (size_t)nearestVert, length });
			graph[nearestVert].emplace_back(Node{ (size_t)nearestVert, (size_t)currVertex, length });

			index++;
		}
	}

	// Create euler path.
	std::vector<int> euler(AdjList& graph, int v1)
	{
		int numAvailable;
		std::stack<Node> available;
		std::stack<int> currPath;
		std::vector<int> path;
		currPath.push(v1);

		do {
			do {
				numAvailable = 0;

				for (size_t i = 0; i < graph[v1].size(); i++)
				{
					if (!graph[v1][i].removed)
					{
						available.push(graph[v1][i]);
						numAvailable++;
					}
				}

				if (numAvailable == 0)
				{
					path.push_back(v1);
					currPath.pop();
					if (!currPath.empty())
						v1 = currPath.top();
					else
						v1 = std::numeric_limits<int>::min();
				}
			} while (numAvailable == 0 && !currPath.empty());

			if (v1 == std::numeric_limits<int>::min())
				break;
			else
			{
				int v2 = available.top().v2;

				currPath.push(v2);
				available.pop();

				for (auto i = graph[v1].begin(); i < graph[v1].end(); ++i)
					if (i->v2 == v2)
						i->removed = true;

				for (auto i = graph[v2].begin(); i < graph[v2].end(); ++i)
					if (i->v2 == v1)
						i->removed = true;

				v1 = v2;
			}
		} while (!available.empty());

		std::reverse(path.begin(), path.end());
		return path;
	}

	// Optimize TSP solution by taking a route that crosses over itself and reordering so that it does not.
	// https://en.wikipedia.org/wiki/2-opt
	bool twoOpt(std::vector<int>& tour, const AdjMatrix& am, int bestDist)
	{
		bool repeat = false;

		for (size_t i = 0; i < tour.size() - 1; i++)
		{
			for (size_t j = i + 1; j < tour.size(); j++)
			{
				if (i == 0 && j == tour.size() - 1)
					continue;

				int newDist = 0;

				if (i == 0)
					newDist = bestDist - (am[tour[j]][tour[j + 1]] + am[tour[i]][tour.back()]) + am[tour[i]][tour[j + 1]] + am[tour[j]][tour.back()];
				else if (j == tour.size() - 1)
					newDist = bestDist - (am[tour[i]][tour[i - 1]] + am[tour[j]][tour[0]]) + am[tour[j]][tour[i - 1]] + am[tour[i]][tour[0]];
				else
					newDist = bestDist - (am[tour[i]][tour[i - 1]] + am[tour[j]][tour[j + 1]]) + am[tour[j]][tour[i - 1]] + am[tour[i]][tour[j + 1]];

				if (newDist < bestDist)
				{
					bestDist = newDist;

					auto it1 = tour.begin() + i, it2 = tour.begin() + j;
					while (it1 < it2)
						std::iter_swap(it1++, it2--);

					repeat = true;
				}
			}
		}

		return repeat;
	}

public:
	// Ctor.
	explicit TSP(const size_t num) : n(num)
	{
		if (n < MIN_VERTICES || num > MAX_VERTICES)
		{
			std::cerr << "Invalid number of vertices.\n";
			exit(-1);
		}
	}

	// Solve.
	Tour operator() (AdjMatrix& adjMatrix)
	{
		// Solution.
		Tour tour;

		if (adjMatrix.empty() || adjMatrix.size() != n)
			return tour;

		// Create minimum spanning tree.
		AdjList mstAdjList = mst(adjMatrix);

		// Make subgraph of mst for vertices with an odd degree, then perfect match odd vertices to get multigraph.
		match(mstAdjList, [&](){ std::vector<int> odds;
			for (auto i = mstAdjList.begin(); i < mstAdjList.end(); ++i)
			if ((*i).size() % 2)
				odds.push_back(std::distance(mstAdjList.begin(), i));
			return odds; 
		}(), adjMatrix);

		// Create euler path from multigraph starting from first vertex.
		tour.path = euler(mstAdjList, mstAdjList[0][0].v1);

		// Create hamiltonian cycle by removing duplicates.
		tour.path.erase(removeDuplicates(tour.path.begin(), tour.path.end()), tour.path.end());

		// Calculate cost of a tour.
		auto cost = [&tour, am=adjMatrix, n=n]() { int d = am[tour.path[0]][tour.path[n - 1]];
		for (auto i = tour.path.begin(); i < (tour.path.end() - 1); ++i)
			d += am[*i][*(i + 1)];
		return d; };

		while (twoOpt(tour.path, adjMatrix, cost()));
		tour.cost = cost();

		return tour;
	}
};

#endif
