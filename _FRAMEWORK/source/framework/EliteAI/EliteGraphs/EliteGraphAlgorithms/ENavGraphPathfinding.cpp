#include "stdafx.h"

#include "ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"

using namespace Elite;

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
{
	//Create the path to return
	std::vector<Vector2> finalPath{};

	//Get the startTriangle and endTriangle
	auto startTriangle{ pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos) };
	auto endTriangle{ pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos) };

	//If we don't have a valid startTriangle or endTriangle -> return empty path
	//If the startTriangle and endTriangle are the same -> return straight line path
	if (startTriangle == nullptr)
		return finalPath;
	if (endTriangle == nullptr)
		return finalPath;

	

	//=> Start looking for a path
	//Clone the graph (returns shared_ptr!)
	auto navGraphClone{ pNavGraph->Clone() };
	
	//Create extra node for the Start Node (Agent's position) and add it to the graph. 
	//Make connections between the Start Node and the startTriangle nodes.
	NavGraphNode* startNode = new NavGraphNode{ -1,startPos };
	GraphConnection* conn = nullptr;
	int nodeID;
	int startID = navGraphClone->AddNode(startNode);
	for (int index = 0; index < 3; ++index)
	{
		nodeID =  pNavGraph->GetNodeIdFromLineIndex(startTriangle->metaData.IndexLines[index]);
		if (nodeID == -1)
			continue;
		conn = new GraphConnection{ startID,nodeID };
		conn->SetCost(Elite::Distance(navGraphClone->GetNode(nodeID)->GetPosition(), startPos));
		navGraphClone->AddConnection(conn);
	}
	//Create extra node for the End Node (endpos) and add it to the graph. 
	//Make connections between the End Node and the endTriangle nodes.
	NavGraphNode* endNode = new NavGraphNode{-1 , endPos };
	int endID = navGraphClone->AddNode(endNode);
	for (int index = 0; index < 3; ++index)
	{
		nodeID = pNavGraph->GetNodeIdFromLineIndex(endTriangle->metaData.IndexLines[index]);
		if (nodeID == -1)
			continue;
		conn = new GraphConnection{ nodeID,endID };
		conn->SetCost(Elite::Distance(navGraphClone->GetNode(nodeID)->GetPosition(), endPos));
		navGraphClone->AddConnection(conn);
	}
	auto pathfinder = AStar{ navGraphClone.get(),HeuristicFunctions::Euclidean };
	auto path = pathfinder.FindPath(navGraphClone->GetNode(startID), navGraphClone->GetNode(endID));
	for (auto& node : path)
	{
		finalPath.push_back(node->GetPosition());
	}
	//Run AStar on the new graph
	//Run optimiser on new graph, MAKE SURE the AStar path is working properly before starting the following section:
	debugPortals = SSFA::FindPortals(path, pNavGraph->GetNavMeshPolygon());
	finalPath = SSFA::OptimizePortals(debugPortals);
	debugNodePositions = finalPath;

	return finalPath;
}
std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph)
{
	std::vector<Vector2> tempNodes;
	std::vector<Portal> tempPortals;
	return FindPath(startPos, endPos, pNavGraph, tempNodes, tempPortals);
}
