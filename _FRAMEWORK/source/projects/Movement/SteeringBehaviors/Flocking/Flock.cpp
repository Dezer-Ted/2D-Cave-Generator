#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"

using namespace Elite;
//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{0}
{
	m_Agents.resize(m_FlockSize);
	m_Neighbors.resize(m_FlockSize);
	//TODO: initialize the flock and the memory pool
	m_pSpatialPartitioning = new bool(false);
	m_pRenderNeighborhood = new bool(false);
	m_pRenderPartitions = new bool(false);
	m_pRenderActiveCells = new bool(false);
	m_pRenderSteeringBehavior = new bool(false);
	m_pSeparationSlider = new float(1.f);
	m_pCohesionSlider = new float(1.f);
	m_pVelMatchSlider = new float(1.f);
	m_pSeekSlider = new float(1.f);
	m_pWanderSlider = new float(1.f);

	m_pCellSpace = new CellSpace(m_WorldSize, m_WorldSize, 25, 25, m_FlockSize);
	m_pWanderBehavior = new Wander();
	m_pPursuitBehavior = new Pursuit();
	m_pSeekBehavior = new Seek();
	m_pCohesionBehavior = new Cohesion(this);
	m_pSeparationBehavior = new Separation(this);
	m_pVelMatchBehavior = new VelocityMatch(this);
	m_pEvadeBehavior = new Evade();
	m_pBlendedSteering = new BlendedSteering({ {m_pWanderBehavior,0.2f},{m_pSeekBehavior,0.2f},{m_pCohesionBehavior,0.2f},{m_pSeparationBehavior,0.2f},{m_pVelMatchBehavior,0.2f} });
	m_pPrioritySteering = new PrioritySteering({ m_pEvadeBehavior,m_pBlendedSteering });
	for (int index = 0; index < m_FlockSize; ++index)
	{
		m_Agents[index] = new SteeringAgent();
		m_Agents[index]->SetPosition({ 1 + static_cast<float>(rand() % 10) / 100,1 + static_cast<float>(rand() % 10) / 100 });
		m_Agents[index]->SetSteeringBehavior(m_pPrioritySteering);
		m_Agents[index]->SetMaxLinearSpeed(50.f);
		m_Agents[index]->SetAutoOrient(true);
		m_Agents[index]->SetMass(1.f);
		
		m_pCellSpace->AddAgent(m_Agents[index]);
	}
	m_Agents[0]->SetBodyColor({ 0,1,0 });
	m_pAgentToEvade = new SteeringAgent();
	m_pAgentToEvade->SetSteeringBehavior(m_pWanderBehavior);
	m_pAgentToEvade->SetMaxLinearSpeed(50.f);
	m_pAgentToEvade->SetAutoOrient(true);
	m_pAgentToEvade->SetMass(1.f);
	m_pAgentToEvade->SetBodyColor({ 1,0,0 });
	m_pAgentToEvade->SetPosition({ 1,1 });
}

Flock::~Flock()
{
	//TODO: Cleanup any additional data
	SAFE_DELETE(m_pEvadeBehavior);
	SAFE_DELETE(m_pSpatialPartitioning);
	SAFE_DELETE(m_pAgentToEvade);
	SAFE_DELETE(m_pRenderSteeringBehavior);
	SAFE_DELETE(m_pRenderPartitions);
	SAFE_DELETE(m_pRenderNeighborhood);
	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pVelMatchBehavior);
	SAFE_DELETE(m_pCellSpace);
	SAFE_DELETE(m_pRenderNeighborhood);
	SAFE_DELETE(m_pSeparationSlider);
	SAFE_DELETE(m_pCohesionSlider);
	SAFE_DELETE(m_pVelMatchSlider);
	SAFE_DELETE(m_pSeekSlider);
	SAFE_DELETE(m_pWanderSlider);
	SAFE_DELETE(m_pPursuitBehavior);
	SAFE_DELETE(m_pRenderActiveCells); 
	for(auto pAgent: m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();
	m_Neighbors.clear();
}

void Flock::Update(float deltaT)
{
	// TODO: update the flock
	// for every agent:
		// register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
		// update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
		// trim the agent to the world
	m_pEvadeBehavior->SetTarget(TargetData{ m_pAgentToEvade->GetPosition(),m_pAgentToEvade->GetRotation(),m_pAgentToEvade->GetLinearVelocity(),m_pAgentToEvade->GetAngularVelocity() });
	m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight = *m_pWanderSlider;
	m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight = *m_pSeekSlider;
	m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight = *m_pCohesionSlider;
	m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight = *m_pSeparationSlider;
	m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight = *m_pVelMatchSlider;
	m_pPursuitBehavior->SetTarget(m_Agents[0]->GetPosition());

	m_pCellSpace->testpos = m_pAgentToEvade->GetPosition();
	for (int index = 0; index < m_FlockSize; ++index)
	{
		RegisterNeighbors(m_Agents[index]);
		m_Agents[index]->Update(deltaT);
		m_Agents[index]->TrimToWorld(m_WorldSize);
		m_pCellSpace->AgentPositionChanged(m_Agents[index], {});
		
	}
	m_pAgentToEvade->Update(deltaT);
	m_pAgentToEvade->TrimToWorld(m_WorldSize);
	m_Agents[0]->SetDebugRenderingEnabled(*m_pRenderSteeringBehavior);
}

void Flock::Render(float deltaT)
{
	// TODO: Render all the agents in the flock
	for (int index = 0; index < m_FlockSize; ++index)
	{
		m_Agents[index]->Render(deltaT);
	}
	m_pAgentToEvade->Render(deltaT);
	if (*m_pRenderNeighborhood) //TODO: switch with imGUI checkbox
		RenderNeighborhood();
	m_pCellSpace->RenderCells(*m_pRenderPartitions,*m_pRenderActiveCells,m_Agents[0],m_NeighborhoodRadius);
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
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

	ImGui::Text("Flocking");
	ImGui::Spacing();

	//TODO: implement ImGUI checkboxes for debug rendering here
	ImGui::Checkbox("Render SteeringBehavior", m_pRenderSteeringBehavior);
	ImGui::Checkbox("Render Neighborhood", m_pRenderNeighborhood);
	ImGui::Checkbox("Render Partitions", m_pRenderPartitions);
	ImGui::Checkbox("Render Active Cells", m_pRenderActiveCells);
	ImGui::Checkbox("Use Spatial Partitioning", m_pSpatialPartitioning);

	ImGui::Text("Behavior Weights");
	ImGui::Spacing();
	ImGui::SliderFloat("Separation", m_pSeparationSlider, 0, 2);
	ImGui::SliderFloat("Cohesion", m_pCohesionSlider, 0, 2);
	ImGui::SliderFloat("VelocityMatch", m_pVelMatchSlider, 0, 2);
	ImGui::SliderFloat("Seek", m_pSeekSlider, 0, 2);
	ImGui::SliderFloat("Wander", m_pWanderSlider, 0, 2);

	//TODO: implement ImGUI sliders for steering behavior weights here
	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RenderNeighborhood()
{
	// TODO: Implement
	// Register the neighbors for the first agent in the flock
	// DebugRender the neighbors in the memory pool
	RegisterNeighbors(m_Agents[0]);
	DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, { 1,1,1 }, DEBUGRENDERER2D->NextDepthSlice());
	std::vector<Vector2> vec = std::vector<Vector2>();
	vec.push_back(Vector2{
		m_Agents[0]->GetPosition().x - m_NeighborhoodRadius,
		m_Agents[0]->GetPosition().y - m_NeighborhoodRadius
		});
	vec.push_back(Vector2{
		m_Agents[0]->GetPosition().x + m_NeighborhoodRadius,
		m_Agents[0]->GetPosition().y - m_NeighborhoodRadius
		});
	vec.push_back(Vector2{
		m_Agents[0]->GetPosition().x + m_NeighborhoodRadius,
		m_Agents[0]->GetPosition().y + m_NeighborhoodRadius
		});
	vec.push_back(Vector2{
		m_Agents[0]->GetPosition().x - m_NeighborhoodRadius,
		m_Agents[0]->GetPosition().y + m_NeighborhoodRadius
		});


	Elite::Polygon* poly = new Elite::Polygon(vec);
	DEBUGRENDERER2D->DrawPolygon(poly, { 1,1,0 });
	SAFE_DELETE(poly);
	if (GetNrOfNeighbors() == 0)
		return;
	for (int index = 0; index < GetNrOfNeighbors() - 1; index++)
	{
		DEBUGRENDERER2D->DrawCircle(GetNeighbors()[index]->GetPosition(), GetNeighbors()[index]->GetRadius(), {0,1,0}, DEBUGRENDERER2D->NextDepthSlice());
	}
}


void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	// TODO: Implement
	if (*m_pSpatialPartitioning == true)
		m_pCellSpace->RegisterNeighbors(pAgent,m_NeighborhoodRadius);
	else
	{
		m_NrOfNeighbors = 0;
		for (int index = 0; index < m_FlockSize; index++)
		{
			if ((pAgent->GetPosition() - m_Agents[index]->GetPosition()).MagnitudeSquared() >= m_NeighborhoodRadius * m_NeighborhoodRadius)
				continue;
			m_Neighbors[m_NrOfNeighbors] = m_Agents[index];
			++m_NrOfNeighbors;
		}
	}
}

Vector2 Flock::GetAverageNeighborPos() const
{
	Vector2 avgPosition = Elite::ZeroVector2;

	// TODO: Implement
	for (int index = 0; index < GetNrOfNeighbors(); ++index)
	{
		avgPosition += GetNeighbors()[index]->GetPosition();
	}
	avgPosition /= static_cast<float>(GetNrOfNeighbors());
	return avgPosition;
}

Vector2 Flock::GetAverageNeighborVelocity() const
{
	Vector2 avgVelocity = Elite::ZeroVector2;
	
	// TODO: Implement
	for (int index = 0; index < GetNrOfNeighbors(); ++index)
	{
		avgVelocity += GetNeighbors()[index]->GetLinearVelocity();
	}
	avgVelocity /= static_cast<float>(GetNrOfNeighbors());
	return avgVelocity;
}
void Flock::SetTarget_Seek(const TargetData& target)
{
	// TODO: Implement
	for (int index = 0; index < m_FlockSize; ++index)
	{
		m_pSeekBehavior->SetTarget(target);
	}
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
int Flock::GetNrOfNeighbors() const 
{ 
	if (*m_pSpatialPartitioning)
	{
		return m_pCellSpace->GetNrOfNeighbors();
	}
	else
	return m_NrOfNeighbors;
}
const std::vector<SteeringAgent*>& Flock::GetNeighbors() const
{
	if (*m_pSpatialPartitioning)
		return m_pCellSpace->GetNeighbors();
	return m_Neighbors;
}
