/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
// BT TODO:

namespace BT_Actions 
{
	Elite::BehaviorState StartPatrol(Elite::Blackboard* pBlackBoard)
	{
		SmartAgent* pAgent;
		pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
		std::vector<Elite::Vector2> path;
		pBlackBoard->GetData("PatrolPath", path);
		PathFollow* pPathFollow{ nullptr };
		pBlackBoard->GetData("GuardPathFollow", pPathFollow);
		pPathFollow->SetPath(path);
		pAgent->SetSteeringBehavior(pPathFollow);
		return Elite::BehaviorState::Success;
	}
	Elite::BehaviorState Patrol(Elite::Blackboard* pBlackBoard)
	{
		SmartAgent* pAgent;
		PathFollow* pPathFollow = nullptr;
		pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
		pBlackBoard->GetData<PathFollow*>("GuardPathFollow", pPathFollow);
		pAgent->SetSteeringBehavior(pPathFollow);
		if (pPathFollow->HasArrived())
		{
			std::vector<Elite::Vector2> path;
			pBlackBoard->GetData<std::vector<Elite::Vector2>>("PatrolPath", path);
			pPathFollow->SetPath(path);

		}
		return Elite::BehaviorState::Running;
	}
	Elite::BehaviorState Chase(Elite::Blackboard* pBlackBoard)
	{
		SmartAgent* pAgent;
		pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
		SteeringAgent* pPlayer = nullptr;
		pBlackBoard->GetData<SteeringAgent*>("TargetAgent", pPlayer);
		auto playerPos{ pPlayer->GetPosition() };
		pAgent->GetSeekBehavior()->SetTarget(pPlayer->GetPosition());
		pPlayer->SetOldPosition(pPlayer->GetPosition());
		pAgent->SetSteeringBehavior(pAgent->GetSeekBehavior());
		return Elite::BehaviorState::Running;
	}
}

namespace BT_Conditions
{
	bool IsTargetVisible(Elite::Blackboard* pBlackBoard)
	{
		SmartAgent* pAgent;
		pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
		SteeringAgent* pPlayer = nullptr;
		pBlackBoard->GetData<SteeringAgent*>("TargetAgent", pPlayer);
		return pAgent->HasLineOfSight(pPlayer->GetPosition());
	}
	bool ShouldSearch(Elite::Blackboard* pBlackBoard)
	{
		SmartAgent* pAgent = nullptr;
		pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
		SteeringAgent* pPlayer = nullptr;
		pBlackBoard->GetData<SteeringAgent*>("TargetAgent", pPlayer);

		/*if (pPlayer->GetOldPos() == Vector2{ 0,0 })
			return false;*/
		return true;

	}
}

#endif