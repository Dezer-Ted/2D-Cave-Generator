#pragma once
#include <stack>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath
	{
	public:

		EulerianPath(Graph* pGraph);

		Eulerianity IsEulerian() const;
		std::vector<GraphNode*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{

		// If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected())
			return Eulerianity::notEulerian;

		auto nodes{ m_pGraph->GetAllNodes() };
		int oddCount{};
		// Count nodes with odd degree 
		for (auto& node : nodes)
		{
			auto connections = m_pGraph->GetConnectionsFromNode(node);
			if (connections.size() & 1)
				++oddCount;
		}
		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian

		if (oddCount > 2)
			return Eulerianity::notEulerian;

		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// An Euler trail can be made, but only starting and ending in these 2 nodes

		if (oddCount == 2)
			return Eulerianity::semiEulerian;

		// A connected graph with no odd nodes is Eulerian
		return Eulerianity::eulerian;

	}

	inline std::vector<GraphNode*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto path = std::vector<GraphNode*>();
		if (eulerianity == Eulerianity::notEulerian)
			return path;
		auto graphCopy = m_pGraph->Clone();
		int nrOfNodes = graphCopy->GetAmountOfNodes();
		GraphNode* pCurrentNode{ nullptr };
		// Check if there can be an Euler path
		// If this graph is not eulerian, return the empty path
		// Find a valid starting index for the algorithm
		switch (eulerianity)
		{
		case Eulerianity::semiEulerian:
			for (int index = 0; index < graphCopy->GetAllNodes().size(); ++index)
			{
				if (graphCopy->GetConnectionsFromNode(graphCopy->GetAllNodes()[index]).size() & 1)
				{
					pCurrentNode = graphCopy->GetAllNodes()[index];
					break;
				}
			}
			break;
		case Eulerianity::eulerian:
			pCurrentNode = graphCopy->GetAllNodes()[0];
			break;
		}
		// Start algorithm loop

		std::stack<int> nodeStack;
		while (graphCopy->GetConnectionsFromNode(pCurrentNode).size() != 0 || nodeStack.size() != 0)
		{
			auto conns{ graphCopy->GetConnectionsFromNode(pCurrentNode) };
			if (conns.size() != 0)
			{
				nodeStack.push(pCurrentNode->GetId());
				pCurrentNode = graphCopy->GetNode(conns[0]->GetToNodeId());
				graphCopy->RemoveConnection(conns[0]);
			}
			else
			{
				path.push_back(m_pGraph->GetNode(pCurrentNode->GetId()));
				pCurrentNode = graphCopy->GetNode(nodeStack.top());
				nodeStack.pop();
			}
		}
		std::reverse(path.begin(), path.end()); // reverses order of the path
		return path;
	}


	inline void EulerianPath::VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex ) const
	{
		// mark the visited node
		visited[startIndex] = true;
		auto neighbors{ m_pGraph->GetConnectionsFromNode(pNodes[startIndex]->GetId()) };
		
		// recursively visit any valid connected nodes that were not visited before
		for (int index = 0; index < pNodes.size(); ++index)
		{
			for (auto& neighbor : neighbors)
			{
				if (pNodes[index]->GetId() == neighbor->GetToNodeId())
				{
					if (visited[index])
						continue;

					VisitAllNodesDFS(pNodes, visited, index);
				}
			}
		}
		
	}

	inline bool EulerianPath::IsConnected() const
	{
		
		auto nodes = m_pGraph->GetAllNodes();
		if (nodes.size() == 0)
			return false;

		// find a valid starting node that has connections
		
		// if no valid node could be found, return false
		// start a depth-first-search traversal from the node that has at least one connection
		std::vector<bool> visited;
		visited.resize(nodes.size());
		for (int index = 0; index < visited.size(); index++)
		{
			visited[index] = false;
		}
		VisitAllNodesDFS(nodes, visited, 0);

		// if a node was never visited, this graph is not connected

		for (int index = 0; index < visited.size(); ++index)
		{
			if (!visited[index])
				return false;
		}
		return true;
	}

}