#pragma once
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

class SteeringAgent;
class BlendedSteering;
class PrioritySteering;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_CombinedSteering final : public IApp
{
public:
	//Constructor & Destructor
	App_CombinedSteering() = default;
	virtual ~App_CombinedSteering() final;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	TargetData m_MouseTarget = {};
	bool m_UseMouseTarget = false;
	bool m_VisualizeMouseTarget = true;
	
	bool m_CanDebugRender = false;
	bool m_TrimWorld = true;
	float m_TrimWorldSize = 50.f;

	//Agent 1: Drunk Seek
	SteeringAgent* m_Agent1Ptr{ nullptr };
	BlendedSteering* m_DrunkSeekBehaviorPtr{ nullptr };
	Wander* m_DrunkWanderPtr{ nullptr };
	Seek* m_DrunkSeekPtr{ nullptr };

	//Agent2: Wander & EVade
	SteeringAgent* m_Agent2Ptr{ nullptr };
	PrioritySteering* m_Agent2SteeringPtr{ nullptr };
	Wander* m_Agent2WanderPtr{ nullptr };
	Evade* m_Agent2EvadePtr{ nullptr };

};