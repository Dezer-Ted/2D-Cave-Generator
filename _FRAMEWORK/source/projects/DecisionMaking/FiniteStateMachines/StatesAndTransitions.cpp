#include "stdafx.h"
#include "StatesAndTransitions.h"

#include "projects/Shared/NavigationColliderElement.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"
// FSM TODO:

//------------
//---STATES---
//------------
void PatrolState::OnEnter(Blackboard* pBlackBoard)
{
	PathFollow* pPathFollow;
	std::vector<Elite::Vector2> path;
	SmartAgent* pAgent = nullptr;
	pBlackBoard->GetData<PathFollow*>("PathFollow", pPathFollow);
	pBlackBoard->GetData("Path", path);
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	pAgent->SetSteeringBehavior(pPathFollow);
	pPathFollow->SetPath(path);
	
	
}
void PatrolState::OnExit(Blackboard* pBlackBoard)
{

}
void PatrolState::Update(Blackboard* pBlackBoard, float deltaTime)
{
	SmartAgent* pAgent;
	PathFollow* pPathFollow = nullptr;
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	pBlackBoard->GetData<PathFollow*>("PathFollow", pPathFollow);
	if (pPathFollow->HasArrived())
	{
		std::vector<Elite::Vector2> path;
		pBlackBoard->GetData<std::vector<Elite::Vector2>>("Path", path);
		pPathFollow->SetPath(path);
		
	}


}
void ChaseState::OnEnter(Blackboard* pBlackBoard)
{
	SmartAgent* pAgent;
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	
	pAgent->SetSteeringBehavior(pAgent->GetSeekBehavior());
	
}
void ChaseState::OnExit(Blackboard* pBlackBoard)
{
	SteeringAgent* pPlayer = nullptr;
	pBlackBoard->GetData<SteeringAgent*>("Player", pPlayer);
	pPlayer->SetOldPosition(pPlayer->GetPosition());

}
void ChaseState::Update(Blackboard* pBlackBoard,float deltaTime)
{
	SteeringAgent* pPlayer = nullptr;
	pBlackBoard->GetData<SteeringAgent*>("Player", pPlayer);
	SmartAgent* pAgent;
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	TargetData target{ pPlayer->GetPosition()};
	pAgent->GetSeekBehavior()->SetTarget(target);
}
void SearchState::OnEnter(Blackboard* pBlackBoard)
{
	SmartAgent* pAgent;
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	SteeringAgent* pPlayer = nullptr;
	pBlackBoard->GetData<SteeringAgent*>("Player", pPlayer);
	TargetData target{ pPlayer->GetOldPos() };
	pAgent->SetSteeringBehavior(pAgent->GetSeekBehavior());
	pAgent->GetSeekBehavior()->SetTarget(target);
	

}
void SearchState::OnExit(Blackboard* pBlackBoard)
{
	float* pElapsedTime = nullptr;
	pBlackBoard->GetData<float*>("ElapsedSearchTime", pElapsedTime);
	*pElapsedTime = 0;

}
void SearchState::Update(Blackboard* pBlackBoard,float deltaTime)
{
	SmartAgent* pAgent;
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	SteeringAgent* pPlayer = nullptr;
	pBlackBoard->GetData<SteeringAgent*>("Player", pPlayer);
	float* pElapsedTime = nullptr;
	pBlackBoard->GetData<float*>("ElapsedSearchTime", pElapsedTime);
	*pElapsedTime += deltaTime;
	if (pAgent->GetSteeringBehavior()!=pAgent->GetSeekBehavior() && ((pPlayer->GetOldPos() - pAgent->GetPosition()).MagnitudeSquared() < pAgent->GetRadius() * pAgent->GetRadius()))
	{
		pAgent->SetSteeringBehavior(pAgent->GetWanderBehavior());
	}
}
//-----------------
//---TRANSITIONS---
//-----------------

bool IsTargetVisible::Evaluate(Blackboard* pBlackBoard) const
{
	SmartAgent* pAgent;
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	SteeringAgent* pPlayer = nullptr;
	pBlackBoard->GetData<SteeringAgent*>("Player", pPlayer);
	return pAgent->HasLineOfSight(pPlayer->GetPosition());
}

bool IsTargetNotVisible::Evaluate(Blackboard* pBlackBoard) const
{
	SmartAgent* pAgent;
	pBlackBoard->GetData<SmartAgent*>("Agent", pAgent);
	SteeringAgent* pPlayer = nullptr;
	pBlackBoard->GetData<SteeringAgent*>("Player", pPlayer);
	return !(pAgent->HasLineOfSight(pPlayer->GetPosition()));
}
bool IsSearchingTooLong::Evaluate(Blackboard* pBlackBoard) const
{
	float* pElapsedTime = nullptr;
	float maxSearchTime;
	pBlackBoard->GetData<float*>("ElapsedSearchTime", pElapsedTime);
	pBlackBoard->GetData<float>("MaxSearchTime", maxSearchTime);
	if (*pElapsedTime >= maxSearchTime)
		return true;
	return false;
}