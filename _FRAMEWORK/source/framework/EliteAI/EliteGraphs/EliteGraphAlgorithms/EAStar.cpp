#include "stdafx.h"
#include "EAStar.h"

using namespace Elite;
AStar::AStar(Graph* pGraph, Heuristic hFunction)
	: m_pGraph(pGraph)
	, m_HeuristicFunction(hFunction)
{
}

std::vector<GraphNode*>AStar::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{
	std::vector<GraphNode*> path{};
	std::list<NodeRecord> openList;
	std::list<NodeRecord> closedList;

	NodeRecord currentNodeRecord{};
	currentNodeRecord.pNode = pStartNode;
	currentNodeRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
	openList.push_back(currentNodeRecord);

	
	float totalGCost{};
	NodeRecord newNode{};
	bool endLoop{};
	while (openList.size() > 0)
	{
		currentNodeRecord = *std::min_element(openList.begin(), openList.end());
		
		auto conns{ m_pGraph->GetConnectionsFromNode(currentNodeRecord.pNode->GetId()) };
		for (auto& conn : conns)
		{
			if (conn->GetToNodeId() != pGoalNode->GetId())
				continue;
			endLoop = true;
			break;
		}
		if (endLoop)
			break;
		for (auto& conn : conns)
		{
			auto pNextNode{ m_pGraph->GetNode(conn->GetToNodeId()) };
			totalGCost = currentNodeRecord.costSoFar + conn->GetCost();
			for (auto& node : closedList)
			{
				if (node.pNode != pNextNode)
					continue;

				if (node.costSoFar <= totalGCost)
					break;

				closedList.remove(node);
			}
			for (auto& node : openList)
			{
				if (node.pNode != pNextNode)
					continue;

				if (node.costSoFar <= totalGCost)
					break;

				closedList.remove(node);
			}
			newNode.costSoFar = totalGCost;
			newNode.pNode = pNextNode;
			newNode.estimatedTotalCost = totalGCost + GetHeuristicCost(pNextNode, pGoalNode);
			newNode.pConnection = m_pGraph->GetConnection(currentNodeRecord.pNode->GetId(), pNextNode->GetId());
			openList.push_back(newNode);
		}
		closedList.push_back(currentNodeRecord);
		openList.remove(currentNodeRecord);
	}
	path.push_back(pGoalNode);
	while (currentNodeRecord.pNode != pStartNode)
	{
		path.push_back(currentNodeRecord.pNode);
		for (auto& node : closedList)
		{
			if (node.pNode == m_pGraph->GetNode(currentNodeRecord.pConnection->GetFromNodeId()))
			{
				currentNodeRecord = node;
				break;
			}
		}
	}
	path.push_back(currentNodeRecord.pNode);
	std::reverse(path.begin(), path.end());
	return path;
}


float AStar::GetHeuristicCost(GraphNode* pStartNode, GraphNode* pEndNode) const
{
	Vector2 toDestination = m_pGraph->GetNodePos(pEndNode->GetId()) - m_pGraph->GetNodePos(pStartNode->GetId());
	return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
}