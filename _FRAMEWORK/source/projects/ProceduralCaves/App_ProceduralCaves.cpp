#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <numeric>
using namespace Elite;

#include "App_ProceduralCaves.h"
App_ProceduralCaves::~App_ProceduralCaves()
{

}
void App_ProceduralCaves::Start()
{
	//Initialize Camera
	//srand(time(NULL));
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(36.782f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(12.9361f, 0.2661f));
	GenerateMap();
	
}
void App_ProceduralCaves::GenerateMap()
{
	m_Caves.clear();
	CalculateBounds();
	InitCells();
	InitFloorSpace();
	for (size_t i = 0; i < m_FirstRoundAmount; ++i)
	{
		ApplyFourToFive(5, 3);
	}

	for (size_t i = 0; i < m_SecondRoundAmount; ++i)
	{
		ApplyFourToFive(5, -1);
	}

	FindCaves();
	if (m_Caves.size() >= 2)
	{
		m_Caves[0].m_IsConnected = true;
		for (int index = 0; index < m_Caves.size(); ++index)
		{
			int targetCave{ GetClosestUnconnectedCaveIndex(index) };
			auto path = PathBetweenCaves(m_Caves[GetClosestConnectedCaveIndex(targetCave)], m_Caves[targetCave]);
			ExplodeNeighbors(path);
		}
		for (size_t i = 0; i < 2; i++)
		{
			ApplyFourToFive(5, 3);
		}
		FindCaves();
	}
}

void App_ProceduralCaves::CalculateBounds()
{
	m_BoundVerts.clear();
	m_pBounds = nullptr;
	m_BoundVerts.push_back(Elite::Vector2{ 0,static_cast<float>(m_currentHeight) });
	m_BoundVerts.push_back(Elite::Vector2{ static_cast<float>(m_currentWidth),static_cast<float>(m_currentHeight) });
	m_BoundVerts.push_back(Elite::Vector2{ static_cast<float>(m_currentWidth),0 });
	m_BoundVerts.push_back(Elite::Vector2{ 0,0 });
	m_pBounds = std::make_unique<Elite::Polygon>(m_BoundVerts);
}
void App_ProceduralCaves::RenderCells() const
{
	std::vector<Elite::Vector2> cellBounds;
	cellBounds.resize(4);
	for (int index = 0; index < m_Cells.size(); ++index)
	{
		cellBounds[0] = m_Cells[index].m_position;
		cellBounds[1] = Elite::Vector2{ m_Cells[index].m_position.x + m_cellDimensions, m_Cells[index].m_position.y };
		cellBounds[2] = Elite::Vector2{ m_Cells[index].m_position.x + m_cellDimensions, m_Cells[index].m_position.y + m_cellDimensions };
		cellBounds[3] = Elite::Vector2{ m_Cells[index].m_position.x , m_Cells[index].m_position.y + m_cellDimensions };
		Elite::Polygon poly{ cellBounds };
		/*for (size_t i = 0; i < m_path.size(); i++)
		{
			if (index == m_path[i])
			{
				DEBUGRENDERER2D->DrawSolidPolygon(&poly, Elite::Color{ 0.f,0.f,1.f }, DEBUGRENDERER2D->NextDepthSlice());

			}
		}*/
		if (m_Cells[index].m_State == WallState::Wall)
			DEBUGRENDERER2D->DrawSolidPolygon(&poly, Elite::Color{ 0.f,0.f,0.f },DEBUGRENDERER2D->NextDepthSlice());
		else if(m_Cells[index].m_State == WallState::Cave)
			DEBUGRENDERER2D->DrawSolidPolygon(&poly, Elite::Color{ 1.f,0.f,0.f }, DEBUGRENDERER2D->NextDepthSlice());
		/*else
			DEBUGRENDERER2D->DrawSolidPolygon(&poly, Elite::Color{ 1.f,1.f,1.f }, DEBUGRENDERER2D->NextDepthSlice());*/

	}
	

}
void App_ProceduralCaves::InitCells()
{
	m_Cells.clear();
	const int cellsPerRow{ m_currentWidth / m_cellDimensions };
	const int cellsPerColumn{ m_currentHeight / m_cellDimensions };
	Cell currentCell{};
	int posX{ 0 };
	int posY{ 0 };
	for (int outerIndex = 0; outerIndex < cellsPerRow; ++outerIndex)
	{
		for (int innerIndex = 0; innerIndex < cellsPerColumn; ++innerIndex)
		{
			currentCell.m_Index = innerIndex+(outerIndex*cellsPerRow);
			currentCell.m_position = Elite::Vector2{ static_cast<float>(posX),static_cast<float>(posY) };
			currentCell.m_State = WallState::Wall;
			m_Cells.push_back(currentCell);
			posX += m_cellDimensions;
		}
		posY += m_cellDimensions;
		posX = 0;
	}
	
}
void App_ProceduralCaves::InitFloorSpace()
{
	const int floorAmount{ static_cast<int>(m_Cells.size() * 0.4f) };
	int randomIndex{};
	for (int index = 0; index < floorAmount; )
	{
		randomIndex = rand() % m_Cells.size();
		if (m_Cells[randomIndex].m_State == WallState::Floor)
		{
			continue;
		}
		if (!CheckIfInbounds(randomIndex))
		{
			continue;
		}

		m_Cells[randomIndex].m_State = WallState::Floor;
		++index;
	}
}
void App_ProceduralCaves::ApplyFourToFive(int gainCutOff, int starvCutOff)
{
	for (int index = 0; index < m_Cells.size(); ++index)
	{
		if (!CheckIfInbounds(index))
			continue;
		switch (CheckSurroundingCells(index, gainCutOff, starvCutOff))
		{
		case SurroundingCells::Starving:
			m_Cells[index].m_State = WallState::Floor;
			break;
		case SurroundingCells::Gaining:
			m_Cells[index].m_State = WallState::Wall;
			break;
		case SurroundingCells::Stable:
			break;
		}
	}

	
}
bool App_ProceduralCaves::CheckIfInbounds(int cellIndex)
{
	
	if (cellIndex < m_currentWidth / m_cellDimensions)
	{
		return false;
	}
	if (cellIndex > m_Cells.size() - (m_currentWidth / m_cellDimensions))
	{
		return false;
	}
	if (cellIndex % m_currentHeight / m_cellDimensions == 0)
	{
		return false;
	}
	if ((cellIndex + 1) % m_currentHeight / m_cellDimensions == 0)
	{
		return false;
	}
	return true;
}

void App_ProceduralCaves::FindCaves()
{
	for (int index = 0; index < m_Cells.size(); ++index)
	{
		if (m_Cells[index].m_State == WallState::Floor)
			FloodFill(index);
	}
}
void App_ProceduralCaves::FloodFill(int cellIndex)
{
	std::vector<int> openList;
	std::vector<int> closedList;
	openList.push_back(cellIndex);
	int currentIndex{};
	while (openList.size() != 0)
	{
		currentIndex = openList[openList.size() - 1];
		openList.pop_back();
		const int indexAbove{ currentIndex - m_currentWidth / m_cellDimensions };
		const int indexBelow{ currentIndex + m_currentWidth / m_cellDimensions };
		for (int index = -1; index < 2; ++index)
		{
			if (m_Cells[indexAbove + index].m_State == WallState::Floor)
			{
				if (DuplicateCheck(indexAbove + index, openList))
				{
					openList.push_back(indexAbove + index);
				}
			}
		}
		//Check Below
		for (int index = -1; index < 2; ++index)
		{
			if (m_Cells[indexBelow + index].m_State == WallState::Floor)
			{
				if (DuplicateCheck(indexBelow + index, openList))
				{
					openList.push_back(indexBelow + index);
				}
			}
		}
		//Check previous
		if (m_Cells[currentIndex - 1].m_State == WallState::Floor)
		{
			if (DuplicateCheck(currentIndex - 1, openList))
			{
				openList.push_back(currentIndex - 1);
			}
		}


		//Check next
		if (m_Cells[currentIndex + 1].m_State == WallState::Floor)
		{
			if (DuplicateCheck(currentIndex + 1, openList))
			{
				openList.push_back(currentIndex + 1);
			}
		}
		closedList.push_back(currentIndex);
		m_Cells[currentIndex].m_State = WallState::Cave;
	}

	m_Caves.push_back(Cave{ closedList,GetAveragePosition(closedList)});
}
const Elite::Vector2& App_ProceduralCaves::GetAveragePosition(const std::vector<int>& list)
{
	Elite::Vector2 averagePos{0,0};
	for (int index = 0; index < list.size(); ++index)
	{
		averagePos += m_Cells[list[index]].m_position;
	}
	averagePos /= static_cast<float>(list.size());
	return averagePos;
}

bool App_ProceduralCaves::DuplicateCheck(int cellIndex, const std::vector<int>& list)
{
	for (int index = 0; index < list.size(); ++index)
	{
		if (list[index] == cellIndex)
			return false;
	}
	return true;
}
bool App_ProceduralCaves::DuplicateCheck(int cellIndex, const std::vector<Cell>& list, int& duplicatePosition)
{
	for (int index = 0; index < list.size(); ++index)
	{
		if (list[index].m_Index == cellIndex)
		{
			duplicatePosition = index;
			return false;
		}
	}
	return true;
}
SurroundingCells App_ProceduralCaves::CheckSurroundingCells(int cellIndex, int gainCutOff, int starvCutOff)
{
	int wallAmount{};
	//Check above
	const int indexAbove{ cellIndex - m_currentWidth / m_cellDimensions };
	const int indexBelow{ cellIndex + m_currentWidth / m_cellDimensions };
	for (int index = -1; index < 2; ++index)
	{
		if (m_Cells[indexAbove + index].m_State == WallState::Wall)
			++wallAmount;
	}
	//Check Below
	for (int index = -1; index < 2; ++index)
	{
		if (m_Cells[indexBelow + index].m_State == WallState::Wall)
			++wallAmount;
	}
	//Check previous
	if (m_Cells[cellIndex - 1].m_State == WallState::Wall)
		++wallAmount;
	
	//Check next
	if (m_Cells[cellIndex + 1].m_State == WallState::Wall)
		++wallAmount;
	if (wallAmount > gainCutOff)
		return SurroundingCells::Gaining;
	else if (wallAmount <= starvCutOff)
		return SurroundingCells::Starving;
	return SurroundingCells::Stable;
}
void App_ProceduralCaves::Update(float deltaTime)
{
	UpdateImGui();
}
void App_ProceduralCaves::Render(float deltaTime) const 
{
	DEBUGRENDERER2D->DrawPolygon(m_pBounds.get(), Elite::Color{1.f,0.f,0.f}, DEBUGRENDERER2D->NextDepthSlice());
	RenderCells();
}
void App_ProceduralCaves::UpdateImGui()
{
	//------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 150;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("Settings");

		ImGui::Spacing();
		ImGui::Spacing();
		if (ImGui::InputInt("Seed", &m_Seed))
		{
			srand(m_Seed);
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::SliderInt("Width", &m_currentWidth, 10, 1000);
		ImGui::SliderInt("Height", &m_currentHeight, 10, 1000);
		ImGui::SliderInt("Smoothing 1", &m_FirstRoundAmount, 1, 10);
		ImGui::SliderInt("Smoothing 2", &m_SecondRoundAmount, 1, 10);
		ImGui::SliderInt("Smoothing 3", &m_ThirdRoundAmount, 1, 10);
		
		ImGui::Spacing();
		ImGui::Spacing();
		if (ImGui::Button("Generate"))
		{
			GenerateMap();
		}
		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

const std::vector<int> App_ProceduralCaves::PathBetweenCaves(const Cave& firstCave, const Cave& secondCave)
{
	
	std::list<Cell> openList;
	std::list<Cell> closedList;
	std::vector<int> path;
	const int goalIndex = GetIndexFromPosition(secondCave.m_Center);
	const int startIndex = GetIndexFromPosition(firstCave.m_Center);
	Cell currentCell{};
	currentCell = m_Cells[startIndex];
	currentCell.m_gCost = 0;
	currentCell.m_hCost = GetHeuristicCost(currentCell.m_position,secondCave.m_Center);
	currentCell.m_ParentIndex = -1;
	openList.push_back(currentCell);
	std::vector<Cell> neighborCells;
	bool breakLoop{ false };
	while (openList.size() != 0)
	{
		currentCell = *std::min_element(openList.begin(), openList.end());
		openList.remove(currentCell);
		AddNeighbors(currentCell, neighborCells);
		
		for (auto& neighbor : neighborCells)
		{
			if (neighbor.m_Index == -2)
				continue;
			if (neighbor.m_Index == goalIndex)
			{
				breakLoop = true;
				break;
			}
			else
			{
				neighbor.m_gCost = currentCell.m_gCost+m_stepCost;
				neighbor.m_hCost = GetHeuristicCost(neighbor.m_position, secondCave.m_Center);
			}
			
			if (!CheckClosedList(neighbor, closedList))
				continue;
			if (CheckOpenList(neighbor, openList, currentCell))
			{
				openList.push_back(neighbor);
			}
		}
		closedList.push_back(currentCell);
		if (breakLoop)
			break;
		
	}
	Cell tempCell = currentCell;
	while (tempCell.m_ParentIndex != -1)
	{
		path.push_back(tempCell.m_ParentIndex);
		tempCell = GetParentIndex(tempCell.m_ParentIndex,closedList);
	}
	path.push_back(startIndex);

	return path;
}

const Cell& App_ProceduralCaves::GetParentIndex(int index, const std::list<Cell>& closedList)
{
	for (auto& elem : closedList)
	{
		if (elem.m_Index == index)
			return elem;
	}
}

bool App_ProceduralCaves::CheckOpenList(const Cell& neighbor, std::list<Cell> openList,const Cell& currentCell)
{
	for (auto& cell : openList)
	{
		if (cell.m_Index == neighbor.m_Index)
		{
			if (cell.m_gCost > neighbor.m_gCost)
			{
				cell.m_ParentIndex = neighbor.m_ParentIndex;
				cell.m_gCost = currentCell.m_gCost + m_stepCost;
			}
			return false;
		}
	}
	return true;
}
bool App_ProceduralCaves::CheckClosedList(const Cell& neighbor, std::list<Cell> closedList)
{
	for (auto& cell : closedList)
	{
		if (cell.m_Index == neighbor.m_Index)
		{
			return false;
		}
	}
	return true;
}
void App_ProceduralCaves::AddNeighbors(Cell& currentCell, std::vector<Cell>& neighborCells)
{
	neighborCells.resize(8);
	const int indexAbove{ currentCell.m_Index - m_currentWidth / m_cellDimensions };
	const int indexBelow{ currentCell.m_Index + m_currentWidth / m_cellDimensions };
	for (int index = -1; index < 2; ++index)
	{
		if (!CheckIfInbounds(indexAbove + index))
		{
			neighborCells[index + 1] = Cell{};
			neighborCells[index + 1].m_ParentIndex = -2;
			continue;
		}
		neighborCells[index + 1] = m_Cells[indexAbove + index];
		neighborCells[index + 1].m_ParentIndex = currentCell.m_Index;
	}
	//Check Below
	for (int index = -1; index < 2; ++index)
	{
		if (!CheckIfInbounds(indexBelow + index))
		{
			neighborCells[index + 4] = Cell{};
			neighborCells[index + 4].m_ParentIndex = -2;
			continue;
		}

		neighborCells[index + 4] = m_Cells[indexBelow + index];
		neighborCells[index + 4].m_ParentIndex = currentCell.m_Index;


	}
	if (CheckIfInbounds(currentCell.m_Index - 1))
	{
		neighborCells[6] = m_Cells[currentCell.m_Index - 1];
		neighborCells[6].m_ParentIndex = currentCell.m_Index;
	}
	else
	{
		neighborCells[6] = Cell{};
		neighborCells[6].m_ParentIndex = -2;
	}
	if (CheckIfInbounds(currentCell.m_Index + 1))
	{
		neighborCells[7] = m_Cells[currentCell.m_Index + 1];
		neighborCells[7].m_ParentIndex = currentCell.m_Index;
	}
	else
	{
		neighborCells[7] = Cell{};
		neighborCells[7].m_ParentIndex = -2;
	}

	
	
}
int App_ProceduralCaves::GetIndexFromPosition(const Elite::Vector2& pos) const
{
	return floor(pos.x) + floor(pos.y) * (m_currentWidth / m_cellDimensions);
}
float App_ProceduralCaves::GetHeuristicCost(const Elite::Vector2& start, const Elite::Vector2& target)
{
	return target.Distance(start);
}
void App_ProceduralCaves::ExplodeNeighbors(const std::vector<int>& path)
{
	for (int index = 0; index < path.size(); ++index)
	{
		const int indexAbove{ path[index] - m_currentWidth / m_cellDimensions};
		const int indexBelow{ path[index] + m_currentWidth / m_cellDimensions };
		for (int index = -1; index < 2; ++index)
		{
			if (!CheckIfInbounds(indexAbove + index))
				continue;
			m_Cells[indexAbove + index].m_State = WallState::Cave;
		}
		//Check Below
		for (int index = -1; index < 2; ++index)
		{
			if (!CheckIfInbounds(indexBelow + index))
				continue;
			m_Cells[indexBelow + index].m_State = WallState::Cave;
		}
		//Check previous
		if (!CheckIfInbounds(path[index] - 1))
			m_Cells[path[index] - 1].m_State = WallState::Cave;

		//Check next
		if (!CheckIfInbounds(path[index] + 1))
			m_Cells[path[index] + 1].m_State= WallState::Cave;
	}
}
int App_ProceduralCaves::GetClosestUnconnectedCaveIndex(int currentCaveIndex)
{
	float tempDistance{ 10000000 };
	float currentDistance{};
	int tempIndex{};
	for (int index = 0; index < m_Caves.size(); ++index)
	{
		if (m_Caves[index].m_IsConnected)
			continue;
		if (index == currentCaveIndex)
			continue;
		currentDistance = m_Caves[currentCaveIndex].m_Center.Distance(m_Caves[index].m_Center);
		if (currentDistance < tempDistance)
		{
			tempDistance = currentDistance;
			tempIndex = index;
		}

	}
	return tempIndex;
}

int App_ProceduralCaves::GetClosestConnectedCaveIndex(int currentCaveIndex)
{
	float tempDistance{ 10000000 };
	float currentDistance{};
	int tempIndex{};
	for (int index = 0; index < m_Caves.size(); ++index)
	{
		if (!m_Caves[index].m_IsConnected)
			continue;
		if (index == currentCaveIndex)
			continue;
		currentDistance = m_Caves[currentCaveIndex].m_Center.Distance(m_Caves[index].m_Center);
		if (currentDistance < tempDistance)
		{
			tempDistance = currentDistance;
			tempIndex = index;
		}

	}
	return tempIndex;
}
