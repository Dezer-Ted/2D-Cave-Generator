#pragma once
#include "framework/EliteInterfaces/EIApp.h"
#include "memory"
//class NavigationColliderElement;
class Polygon;

enum class WallState
{
	Wall = 0,
	Floor = 1,
	Cave = 2,
};
enum class SurroundingCells
{
	Starving = 0,
	Gaining = 1,
	Stable = 2
};
struct Cave
{
	std::vector<int> m_CellIndeces;
	Elite::Vector2 m_Center;
	bool m_IsConnected{ false };
};
struct Cell
{
	int m_Index;
	WallState m_State;
	Elite::Vector2 m_position;
	float m_gCost;
	float m_hCost;
	int m_ParentIndex;

	bool operator<(const Cell& cell)
	{
		return (m_gCost + m_hCost) < (cell.m_gCost + cell.m_hCost);
	}
	bool operator==(const Cell& lhs) const
	{
		return lhs.m_Index == m_Index;
	}
};
class App_ProceduralCaves : public IApp
{
public:
	App_ProceduralCaves() = default;
	virtual ~App_ProceduralCaves();

	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;
private:
	App_ProceduralCaves(const App_ProceduralCaves&) = delete;
	App_ProceduralCaves& operator=(const App_ProceduralCaves&) = delete;
	//std::vector<NavigationColliderElement*> m_vNavigationColliders = {};
	int m_currentWidth{ 25 };
	int m_currentHeight{ 25 };
	int m_FirstRoundAmount{ 4 };
	int m_SecondRoundAmount{ 3 };
	int m_ThirdRoundAmount{ 2 };
	int m_cellDimensions{ 1 };
	const unsigned int m_stepCost{ 1 };
	int m_Seed{ 4 };

	std::vector<Elite::Vector2> m_BoundVerts;
	std::unique_ptr<Elite::Polygon> m_pBounds;
	std::vector<Cell> m_Cells;
	std::vector<Cave> m_Caves;
	
	void CalculateBounds();
	void InitCells();
	void UpdateImGui();
	void RenderCells() const;

	void InitFloorSpace();
	void ApplyFourToFive(int gainCutOff, int starvCutOff);
	bool CheckIfInbounds(int cellIndex);
	SurroundingCells CheckSurroundingCells(int cellIndex, int gainCutOff,int starvCutOff);
	void FindCaves();
	void FloodFill(int cellIndex);
	bool DuplicateCheck(int cellIndex,const std::vector<int>& list);
	bool DuplicateCheck(int cellIndex,const std::vector<Cell>& list, int& duplicatePosition);
	const Elite::Vector2& GetAveragePosition(const std::vector<int>& list);
	const std::vector<int> PathBetweenCaves(const Cave& firstCave, const Cave& secondCave);
	int GetIndexFromPosition(const Elite::Vector2& pos) const;
	float GetHeuristicCost(const Elite::Vector2& start, const Elite::Vector2& target);
	bool CheckOpenList(const Cell& neighbor, std::list<Cell> openList, const Cell& currentCell);
	bool CheckClosedList(const Cell& neighbor, std::list<Cell> closedList);
	void AddNeighbors(Cell& currentCell, std::vector<Cell>& neighborCells);
	const Cell& GetParentIndex(int index,const std::list<Cell>& closedList);
	void ExplodeNeighbors(const std::vector<int>& path);
	int GetClosestUnconnectedCaveIndex(int currentCaveIndex);
	int GetClosestConnectedCaveIndex(int currentCaveIndex);
	void GenerateMap();

};

