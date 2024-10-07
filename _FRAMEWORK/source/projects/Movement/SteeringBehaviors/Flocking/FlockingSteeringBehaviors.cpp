#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_Target = m_pFlock->GetAverageNeighborPos();
	
	SteeringOutput output{ Seek::CalculateSteering(deltaT, pAgent) };
	return output;
}

//*********************
//SEPARATION (FLOCKING)

SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 direction{};
	for (int index = 0; index < m_pFlock->GetNrOfNeighbors(); ++index)
	{
		
		direction = pAgent->GetPosition() - m_pFlock->GetNeighbors()[index]->GetPosition();
		const float distanceToNeighborSqrd{ direction.MagnitudeSquared() };

		if (distanceToNeighborSqrd == 0)
			continue;

		steering.LinearVelocity += (direction / distanceToNeighborSqrd); 
	}
	steering.LinearVelocity = steering.LinearVelocity.GetNormalized() * pAgent->GetMaxLinearSpeed();
	return steering;
}
//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput output{ };

	output.LinearVelocity = m_pFlock->GetAverageNeighborVelocity().GetNormalized()*pAgent->GetMaxAngularSpeed();
	

	return output;
}