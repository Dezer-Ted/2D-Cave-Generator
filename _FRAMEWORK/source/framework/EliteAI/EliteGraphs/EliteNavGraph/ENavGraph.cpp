#include "stdafx.h"
#include "ENavGraph.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraphNodeFactory/EGraphNodeFactory.h"
//#include "framework\EliteAI\EliteGraphs\EliteGraphAlgorithms\EAStar.h"

using namespace Elite;

NavGraph::NavGraph(const std::vector<Polygon>& colliderShapes, float widthWorld, float heightWorld, float playerRadius = 1.0f) :
	Graph(false, new GraphNodeFactoryTemplate<NavGraphNode>()),
	m_pNavMeshPolygon(nullptr)
{
	float const halfWidth = widthWorld / 2.0f;
	float const halfHeight = heightWorld / 2.0f;
	std::list<Vector2> baseBox
	{ { -halfWidth, halfHeight },{ -halfWidth, -halfHeight },{ halfWidth, -halfHeight },{ halfWidth, halfHeight } };

	m_pNavMeshPolygon = new Polygon(baseBox); // Create copy on heap

	//Store all children
	for (auto p : colliderShapes)
	{
		p.ExpandShape(playerRadius);
		m_pNavMeshPolygon->AddChild(p);
	}

	//Triangulate
	m_pNavMeshPolygon->Triangulate();

	//Create the actual graph (nodes & connections) from the navigation mesh
	CreateNavigationGraph();
}

NavGraph::NavGraph(const NavGraph& other): Graph(other)
{
}

NavGraph::~NavGraph()
{
	delete m_pNavMeshPolygon;
	m_pNavMeshPolygon = nullptr;
}

std::shared_ptr<NavGraph> NavGraph::Clone()
{
	return std::shared_ptr<NavGraph>(new NavGraph(*this));
}

int NavGraph::GetNodeIdFromLineIndex(int lineIdx) const
{

	for (auto& pNode : m_pNodes)
	{
		if (reinterpret_cast<NavGraphNode*>(pNode)->GetLineIndex() == lineIdx)
		{
			return pNode->GetId();
		}
	}

	return invalid_node_id;
}

Elite::Polygon* NavGraph::GetNavMeshPolygon() const
{
	return m_pNavMeshPolygon;
}

void NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigationmesh and create a node on the center of each edge
	CreateNodes();
	//2  Now that every node is created, connect the nodes that share the same triangle (for each triangle, ... )
	CreateConnections();
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistances();
}
//Creates all possible connections for each triangle
void Elite::NavGraph::CreateConnections()
{
	for (auto& triangle : m_pNavMeshPolygon->GetTriangles())
	{
		std::vector<int> nodeIdx;
		for (auto& lineIdx : triangle->metaData.IndexLines)
		{
			int index{ this->GetNodeIdFromLineIndex(lineIdx) };
			if (index != -1)
			{
				nodeIdx.push_back(index);
			}
		}
		if (nodeIdx.size() == 2)
		{
			GraphConnection* conn = new GraphConnection{ nodeIdx[0],nodeIdx[1] };

			this->AddConnection(conn);
		}
		else if (nodeIdx.size() == 3)
		{
			GraphConnection* conn = nullptr;
			conn = new GraphConnection{ nodeIdx[0],nodeIdx[1] };
			this->AddConnection(conn);
			conn = new GraphConnection{ nodeIdx[1],nodeIdx[2] };
			this->AddConnection(conn);
			conn = new GraphConnection{ nodeIdx[2],nodeIdx[0] };
			this->AddConnection(conn);
		}
	}
}
//Creates nodes in the middle of each line and adds them to the graph
void Elite::NavGraph::CreateNodes()
{
	auto lines = m_pNavMeshPolygon->GetLines();
	for (auto& line : lines)
	{
		auto triangles = m_pNavMeshPolygon->GetTrianglesFromLineIndex(line->index);
		if (triangles.size() == 1)
			continue;
		Vector2 lineCenter
		{
			(line->p1.x + line->p2.x) / 2,
			(line->p1.y + line->p2.y) / 2
		};
		NavGraphNode* newNode = new NavGraphNode{ line->index,lineCenter };

		this->AddNode(newNode);

	}
}

