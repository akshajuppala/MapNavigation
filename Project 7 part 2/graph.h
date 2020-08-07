/*graph.h*/

//
// << AKSHAJ UPPALA >>
//
// Basic graph class using adjacency List representation.
// 
//
// original author: Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>

using namespace std;

template<typename VertexT, typename WeightT>
class graph
{
private:
  //
  // We are using adjacency List implementation

  map<VertexT, map< VertexT, WeightT>> AdjList;
  int numVertices;

  //
  // _LookupVertex
  //
  // Finds the vertex in the AdjList and returns
  // true if found, otherwise returns false.
  //
  bool _LookupVertex(VertexT v) const
  {

	if(AdjList.count(v) == 1){
		return true;
	}
	
    // if get here, not found:
    return false;
  }


public:

  //
  // constructor:
  //
  // Constructs an empty graph 
  graph()
  {
	  numVertices = 0;
  }
	
	
  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const
  {
    return numVertices;
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const
  {
    int count = 0;

    for(auto iter = AdjList.begin(); iter != AdjList.end(); ++iter){
		
		count = count + iter->second.size();
	}

    return count;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph, and if added
  // returns true.  If the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v)
  {

    //
    // is the vertex already in the graph?  If so, we do not 
    // insert again otherwise Vertices may fill with duplicates:
    //
    if (_LookupVertex(v) == true)
      return false;

    //
    // if we get here, vertex does not exist so insert.
    //
    map<VertexT,WeightT> K;
    AdjList.emplace(v,K);
	numVertices++;
    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from,VertexT to, WeightT weight)
  {
    //
    // we need to search the Vertices:
    //

    if (_LookupVertex(from) == false)  // not found:
      return false;

	if (_LookupVertex(to) == false)  // not found:
      return false;
	  
	AdjList.at(from).erase(to);

    //The vertices exist and the existing edges were removed to insert the new entry

	AdjList.at(from).emplace(to,weight);
    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If 
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not 
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const
  {
    //
    // we need to search the Vertices and see if they exist
    // 
    if (_LookupVertex(from) == false)  // not found:
      return false;

    bool test = AdjList.at(from).count(to);

    if (test == false)  // not found:
      return false;

    //
    // Okay, the edge exists, return the weight via the 
    // reference parameter:
    //
    weight = AdjList.at(from).at(to);

    return true;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const
  {
    set<VertexT>  S;

    //
    // we need to search the Vertex

    if (_LookupVertex(v) == false)  // not found:
      return S;

	//if vertex is found keep the neighbors in the set S
	map<VertexT,WeightT> temp = AdjList.at(v);
	for(auto iter = temp.begin(); iter!=temp.end(); ++iter){
		S.insert(iter->first);
	}
	
    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const
  {
	vector<VertexT> vertices;
	for(auto iter = AdjList.begin(); iter!= AdjList.end(); ++iter){
		
		vertices.push_back(iter->first);
	}
    return vertices;  // returns a copy:
  }

  //
  // dump
  // 
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G;
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const
  {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;
	
	auto temp = AdjList.begin();
	
    for (int i = 0; i < this->NumVertices(); ++i)
    {
      output << " " << temp->first << ": ";
	  auto temp2 = temp->second.begin();
	  for(size_t j =0; j < temp->second.size(); ++j){
		  output << "(" << temp2->first << "," << temp2->second << ") ";
		  temp2++;
	  }
	  output << endl;
	  temp++;
    }

    output << "**************************************************" << endl;
  }

};