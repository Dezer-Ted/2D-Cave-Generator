#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	//Calculate bounds of a cell
	m_CellWidth = width / cols;
	m_CellHeight = height / rows;
	int verticalIndex{},horizontalIndex{};
	//TODO: create the cells
	
	for (int index = 0; index < rows*cols; ++index)
	{

		if (index % cols == 0 && index !=0)
			verticalIndex++;
		horizontalIndex = index % cols;
		m_Cells.push_back(Cell
			{
				horizontalIndex*m_CellWidth,
				verticalIndex*m_CellHeight,
				m_CellWidth,
				m_CellHeight
			});
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	//Add the agent to the correct cell
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
	agent->SetOldPosition(agent->GetPosition());
}

void CellSpace::AgentPositionChanged(SteeringAgent* agent, Elite::Vector2 oldPos)
{
	// TODO: Check if the agent needs to be moved to another cell.
	// Use the calculated index for oldPos and currentPos for 

	const int newIndex{ PositionToIndex(agent->GetPosition()) };
	const int oldIndex{ PositionToIndex(agent->GetOldPos()) };
	if ( oldIndex != newIndex)
	{
		m_Cells[oldIndex].agents.remove(agent);
		m_Cells[newIndex].agents.push_back(agent);
	}
	agent->SetOldPosition(agent->GetPosition());
}

void CellSpace::RegisterNeighbors(SteeringAgent* pAgent, float neighborhoodRadius)
{
	//TODO: Register the neighbors for the provided agent
	//Only check the cells that are within the radius of the neighborhood
	float widthAndHeight{ neighborhoodRadius * 2 };
	Elite::Rect boundingBox{
		Elite::Vector2{
			pAgent->GetPosition().x - neighborhoodRadius,
			pAgent->GetPosition().y - neighborhoodRadius
	}, widthAndHeight,widthAndHeight
	};
	m_NrOfNeighbors = 0;
	for (Cell& cell : m_Cells)
	{
		if (Elite::IsOverlapping(cell.boundingBox, boundingBox))
		{
			for (auto& agent : cell.agents)
			{
				if ((pAgent->GetPosition() - agent->GetPosition()).MagnitudeSquared() >= neighborhoodRadius * neighborhoodRadius)
					continue;
				m_Neighbors[m_NrOfNeighbors] = agent;
				++m_NrOfNeighbors;
			}
		}
	}
}

void CellSpace::EmptyCells()
{
	for (Cell& c : m_Cells)
		c.agents.clear();
}

void CellSpace::RenderCells(bool renderCells, bool renderActiveCells, SteeringAgent* pAgent, float neighborhoodRadius) const
{
	//TODO: Render the cells with the number of agents inside of it
	//TIP: use DEBUGRENDERER2D->DrawPolygon(...) and Cell::GetRectPoints())
	//TIP: use DEBUGRENDERER2D->DrawString(...)
	if (renderCells)
	{
		Elite::Polygon* polygon = nullptr;
		for (size_t index = 0; index < m_Cells.size(); index++)
		{
			polygon = new Elite::Polygon{ m_Cells[index].GetRectPoints() };
			/*if (index == PositionToIndex(testpos))
				DEBUGRENDERER2D->DrawPolygon(polygon, Elite::Color{ 0.f,1.f,0.f });
			else*/
				DEBUGRENDERER2D->DrawPolygon(polygon, Elite::Color{ 1.f,0.f,0.f });
			SAFE_DELETE(polygon);
			Elite::Rect tempCell{ m_Cells[index].boundingBox };

			DEBUGRENDERER2D->DrawString(Elite::Vector2{
				tempCell.bottomLeft.x,
				tempCell.bottomLeft.y + tempCell.height
				}, std::to_string(m_Cells[index].agents.size()).c_str());

		}
	}
	if (renderActiveCells && renderCells)
	{
		float widthAndHeight{ neighborhoodRadius * 2 };
		Elite::Rect boundingBox{
			Elite::Vector2{
				pAgent->GetPosition().x - neighborhoodRadius,
				pAgent->GetPosition().y - neighborhoodRadius
		}, widthAndHeight,widthAndHeight
		};
		for (auto& cell : m_Cells)
		{
			if (Elite::IsOverlapping(boundingBox, cell.boundingBox))
			{
				Elite::Polygon* poly = new Elite::Polygon(cell.GetRectPoints());
				DEBUGRENDERER2D->DrawPolygon(poly, { 1,1,0 });
				SAFE_DELETE(poly);
			}
		}
	}
	
}

int CellSpace::PositionToIndex(const Elite::Vector2& pos) const
{
	//TODO: Calculate the index of the cell based on the position
	Elite::Vector2 tempPos{ pos.x,pos.y };
	int verticalIndex{}, horizontalIndex{};
	if (tempPos.x >= m_SpaceWidth)
		tempPos.x = m_SpaceWidth - m_Cells[0].boundingBox.width;
	if (tempPos.y >= m_SpaceHeight)
		tempPos.y = m_SpaceHeight - m_Cells[0].boundingBox.height;

	horizontalIndex = static_cast<int>(tempPos.x) / static_cast<int>(m_Cells[0].boundingBox.width);
	verticalIndex = static_cast<int>(tempPos.y) / static_cast<int>(m_Cells[0].boundingBox.height);
	int index{ horizontalIndex + (verticalIndex * m_NrOfCols) };
	if (index >= m_Cells.size())
		index = m_Cells.size();
	return index;
}