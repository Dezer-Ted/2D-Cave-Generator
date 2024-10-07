/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"

using namespace Elite;
// FSM TODO:

//------------
//---STATES---
//------------
class PatrolState final : public FSMState
{
public:
	PatrolState() {};
	PatrolState(const PatrolState& patrolState) = delete;
	PatrolState(PatrolState&& patrolState) = delete;
	PatrolState operator=(const PatrolState&) = delete;
	PatrolState operator=(PatrolState&&) = delete;
	~PatrolState() override = default;

	void OnEnter(Blackboard* pBlackboard) override;
	void OnExit(Blackboard* pBlackboard) override;
	void Update(Blackboard* pBlackboard, float deltaTime) override;
};
class ChaseState final : public FSMState
{
public:
	ChaseState() {};
	ChaseState(const ChaseState& chaseState) = delete;
	ChaseState(ChaseState&& chaseState) = delete;
	ChaseState operator=(const ChaseState&) = delete;
	ChaseState operator=(ChaseState&&) = delete;
	~ChaseState() override = default;

	void OnEnter(Blackboard* pBlackboard) override;
	void OnExit(Blackboard* pBlackboard) override;
	void Update(Blackboard* pBlackboard, float deltaTime) override;
};
class SearchState final : public FSMState
{
public:
	SearchState() {};
	SearchState(const SearchState& searchState) = delete;
	SearchState(SearchState&& searchState) = delete;
	SearchState operator=(const SearchState&) = delete;
	SearchState operator=(SearchState&&) = delete;
	~SearchState() override = default;

	void OnEnter(Blackboard* pBlackboard) override;
	void OnExit(Blackboard* pBlackboard) override;
	void Update(Blackboard* pBlackboard, float deltaTime) override;
};
//-----------------
//---TRANSITIONS---
//-----------------
class IsTargetVisible final : public FSMCondition
{
public:
	IsTargetVisible() {};
	~IsTargetVisible() = default;
	bool Evaluate(Blackboard* pBlackBoard) const override;
};
class IsTargetNotVisible final : public FSMCondition
{
public:
	IsTargetNotVisible() {};
	~IsTargetNotVisible() = default;
	bool Evaluate(Blackboard* pBlackBoard) const override;
};
class IsSearchingTooLong final : public FSMCondition
{
public:
	IsSearchingTooLong() {};
	~IsSearchingTooLong() = default;
	bool Evaluate(Blackboard* pBlackBoard) const override;
};
#endif