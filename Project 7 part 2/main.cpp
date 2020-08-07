/*main.cpp*/

//
// Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07: open street maps, graphs, and Dijkstra's alg
// 
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:  
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>
#include <queue>
#include <utility>
#include <stack>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"

using namespace std;
using namespace tinyxml2;

class prioritize
{
   public:
   bool operator()(const pair<long long,double>& p1, const pair<long long,double>& p2) const
   {
      if(p1.second>p2.second)
         return true;
      else if(p1.second == p2.second){
         if(p1.first>p2.first){
            return true;
         }
         else{
            return false;
         }
      }
         
      else
         return false;
      
   }
   
};

vector<long long> Dijkstra(graph<long long,double>& G, 
						long long startV, 
						map<long long, double>& distances, map<long long, long long>& predecessors)
{
  double INF = numeric_limits<double>::max();
	
  vector<long long> visited;
  set<long long>    visitedSet;
  vector<long long> V;
  
  priority_queue<
  pair<long long,double>,
  vector<pair<long long,double>>,
  prioritize> pq;
  
  V = G.getVertices();
  for(auto iter : V){
     distances[iter] = INF;
	 predecessors[iter] = -1;
     pq.push(make_pair(iter,INF));
     }

  distances[startV] = 0;
  predecessors[startV] = 0;
  pq.push(make_pair(startV,0));
  
  while(!pq.empty())
  {
     long long currentV = pq.top().first;
     pq.pop(); 
        
     if(distances[currentV] == INF){
         break;
     }
     else if(visitedSet.count(currentV) == 1){
         continue;
     }
     else
     {
        visitedSet.insert(currentV);
        visited.push_back(currentV);
     }
     
     set<long long> N = G.neighbors(currentV);
     
     for(auto e : N){
        
        double edgeWeight = -1.0;
        G.getWeight(currentV, e, edgeWeight);
        double alternativePathDistance = distances[currentV] + edgeWeight;
        
        if(alternativePathDistance < distances[e]){
           distances[e] = alternativePathDistance;
		   predecessors[e] = currentV;
           pq.push(make_pair(e,alternativePathDistance));
           
        }
        
      }
     
     
  }

  return visited;
}

//////////////////////////////////////////////////////////////////
//
// main
//
int main()
{
  map<long long, Coordinates>  Nodes;     // maps a Node ID to it's coordinates (lat, lon)
  vector<FootwayInfo>          Footways;  // info about each footway, in no particular order
  vector<BuildingInfo>         Buildings; // info about each building, in no particular order
  XMLDocument                  xmldoc;
  
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "")
  {
    filename = def_filename;
  }

  //
  // Load XML-based map file 
  //
  if (!LoadOpenStreetMap(filename, xmldoc))
  {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }
  
  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == Nodes.size());
  assert(footwayCount == Footways.size());
  assert(buildingCount == Buildings.size());
  
  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;


  //
  // TODO: build the graph, output stats:
  //

  graph<long long, double> G;

  for(auto iter = Nodes.begin(); iter != Nodes.end(); iter++){
	  
	  G.addVertex(iter->first);
  }

  for(auto iter : Footways){
	  vector<long long> oneWay = iter.Nodes;
	  
	  for(size_t i = 0; i < oneWay.size()-1; ++i){
		  long long FirstNode = oneWay.at(i);
		  long long SecondNode = oneWay.at(i+1);
	  
		  double distanceBetweeenTwoNodes = distBetween2Points(Nodes[FirstNode].Lat, Nodes[FirstNode].Lon, Nodes[SecondNode].Lat, Nodes[SecondNode].Lon);
		  G.addEdge(FirstNode, SecondNode, distanceBetweeenTwoNodes);
		  G.addEdge(SecondNode, FirstNode, distanceBetweeenTwoNodes);
	  }
	  
  }
  

  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#")
  {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);


    //
    // TODO: lookup buildings, find nearest start and dest nodes,
    // run Dijkstra's alg, output distance and path to destination:
    //

	bool startFound = false;
	bool destFound = false;
	BuildingInfo StartBuilding;
	BuildingInfo DestBuilding;
	Coordinates NearestStartNode;
	Coordinates NearestDestNode;
	
	for(auto B : Buildings){
		
		if(startFound == false && (B.Abbrev == startBuilding || B.Fullname.find(startBuilding) != std::string::npos)){
			StartBuilding.Fullname = B.Fullname;
			StartBuilding.Coords.Lat = B.Coords.Lat;
			StartBuilding.Coords.Lon = B.Coords.Lon;
			startFound = true;
		}
		
		if(destFound == false && (B.Abbrev == destBuilding || B.Fullname.find(destBuilding) != std::string::npos)){
			DestBuilding.Fullname = B.Fullname;
			DestBuilding.Coords.Lat = B.Coords.Lat;
			DestBuilding.Coords.Lon = B.Coords.Lon;
			destFound = true;
		}
	}
	  
	if(startFound == false){
		cout << "Start building not found" << endl;
	}
	else if(destFound == false){
		cout << "Destination building not found" << endl;
	}
	else{
		cout << "Starting Point:" << endl;
		cout << " " << StartBuilding.Fullname << endl;
		cout << " (" << StartBuilding.Coords.Lat << ", " << StartBuilding.Coords.Lon << ")" << endl;
		cout << "Destination Point:" << endl;
		cout << " " << DestBuilding.Fullname << endl;
		cout << " (" << DestBuilding.Coords.Lat << ", " << DestBuilding.Coords.Lon << ")" << endl;
		cout << endl;
		
		//Search the footways for nearest start and dest nodes
		double StartMin = distBetween2Points(StartBuilding.Coords.Lat, StartBuilding.Coords.Lon, Nodes[Footways[0].Nodes[0]].Lat, Nodes[Footways[0].Nodes[0]].Lon);
		double DestMin = distBetween2Points(DestBuilding.Coords.Lat, DestBuilding.Coords.Lon, Nodes[Footways[0].Nodes[0]].Lat, Nodes[Footways[0].Nodes[0]].Lon);
		for(auto iter : Footways){
			vector<long long> oneWay = iter.Nodes;
	  
			for(size_t i = 0; i < oneWay.size(); ++i){
				long long RandomNode = oneWay.at(i);
	  
				double distanceBetweeenTwoNodes = distBetween2Points(StartBuilding.Coords.Lat, StartBuilding.Coords.Lon, Nodes[RandomNode].Lat, Nodes[RandomNode].Lon);
		    
				if(distanceBetweeenTwoNodes <= StartMin){
					StartMin = distanceBetweeenTwoNodes;
					NearestStartNode = Nodes[RandomNode];

				}
				
				distanceBetweeenTwoNodes = distBetween2Points(DestBuilding.Coords.Lat, DestBuilding.Coords.Lon, Nodes[RandomNode].Lat, Nodes[RandomNode].Lon);
				
				if(distanceBetweeenTwoNodes <= DestMin){
					DestMin = distanceBetweeenTwoNodes;
					NearestDestNode = Nodes[RandomNode];
				}
			}
		}
		
		cout << "Nearest start Node:" << endl;
		cout << " " << NearestStartNode.ID << endl;
		cout << " (" << NearestStartNode.Lat << ", " << NearestStartNode.Lon << ")" << endl;
		cout << "Nearest destination Node:" << endl;
		cout << " " << NearestDestNode.ID << endl;
		cout << " (" << NearestDestNode.Lat << ", " << NearestDestNode.Lon << ")" << endl;
		cout << endl;
		
		//
		//Applying Dijkstra's algorithm
		//
		map<long long, double> distances;
		map<long long, long long> predecessors;
		Dijkstra(G, NearestStartNode.ID, distances, predecessors);
		
		//Output the distance and the path to the destination
		
		
		long long predecessor = predecessors[NearestDestNode.ID];
		stack<long long> path;
		int stackSize = 0;
		while(predecessor > 0.00001){
			
			path.push(predecessor);
			predecessor = predecessors[predecessor];
			
			stackSize++;
		}
		
		cout << "Navigating with Dijkstra..." << endl;
		if(predecessors[NearestDestNode.ID] < -0.5){
			cout << "Sorry, destination unreachable" << endl;
		}
		else{
			
			cout << "Distance to dest: " << distances[NearestDestNode.ID] << " miles" << endl;
			cout << "Path: ";
			
			while(!path.empty()){
				cout << path.top() << "->";
				path.pop();
			}
			cout << NearestDestNode.ID << endl;
		}
	}

    //
    // another navigation?
    //
    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }

  //
  // done:
  //
  cout << "** Done **" << endl;

  return 0;
}
