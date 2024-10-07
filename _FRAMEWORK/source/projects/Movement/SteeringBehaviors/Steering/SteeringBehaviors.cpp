//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"
#include <limits>
using namespace Elite;
//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position;
	Vector2 direction = targetPos - currentPos;
	direction.Normalize();
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0,1,0 });
	}
	return steering;
}
//Flee
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position;
	Vector2 direction = targetPos - currentPos;
	direction.Normalize();

	steering.LinearVelocity = -direction * pAgent->GetMaxLinearSpeed();
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0,1,0 });
	}
	return steering;
}
//Arrive
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position;
	Vector2 direction = targetPos - currentPos;
	float maxSpeed = pAgent->GetMaxLinearSpeed();

	float distance = direction.Magnitude();
	float a = distance - m_TargetRadius;
	float b = m_SlowRadius - m_TargetRadius;

	if (distance < m_SlowRadius)
	{
		float ratio{ a / b };
		if (ratio < 0.1f)
			ratio = 0.1f;
		maxSpeed *= ratio;
	}

	steering.LinearVelocity = direction.GetNormalized() * maxSpeed;
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0,1,0 });
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(),m_TargetRadius, { 0.f,0.f,1.f }, DEBUGRENDERER2D->NextDepthSlice());

	}
	return steering;
}
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	float timePrediction = (pAgent->GetPosition() - m_Target.Position).Magnitude() / pAgent->GetMaxLinearSpeed();
	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position + m_Target.LinearVelocity * timePrediction;
	Vector2 direction = m_Target.Position - currentPos;
	direction.Normalize();
	
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0,1,0 });
	}
	return steering;
}
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{

	SteeringOutput steering = {};
	

	float timePrediction = (pAgent->GetPosition() - m_Target.Position).Magnitude() / pAgent->GetMaxLinearSpeed();
	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position + m_Target.LinearVelocity * timePrediction;


	if ((pAgent->GetPosition() - targetPos).MagnitudeSquared() > m_EvadeRadius * m_EvadeRadius)
	{
		
		SteeringOutput out{};
		out.IsValid = false;
		if (pAgent->GetDebugRenderingEnabled())
		{
			DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRadius, { 1.f,0.f,0.f }, DEBUGRENDERER2D->NextDepthSlice());
			DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0,1,0 });
			DEBUGRENDERER2D->DrawPoint(m_Target.Position, 5, { 1,1,0 });

		}
		return out;
	}
	Vector2 direction = m_Target.Position - currentPos;
	direction.Normalize();

	steering.LinearVelocity = -direction * pAgent->GetMaxLinearSpeed();
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRadius, { 1.f,0.f,0.f }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0,1,0 });
		DEBUGRENDERER2D->DrawPoint(m_Target.Position, 5, { 1,1,0 });

	}
	return steering;
}
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 circleCenter = pAgent->GetPosition() + pAgent->GetDirection() * (m_CircleSize * 1.5f);


	m_RandomAngle = (rand() % m_MaxAngleChange - static_cast<float>(m_MaxAngleChange)/2.f) + m_RandomAngle;
	if (m_RandomAngle >= 360)
		m_RandomAngle -= 360;
	Vector2 targetPos{ circleCenter + Vector2{cos(ToRadians(m_RandomAngle)) * m_CircleSize,sin(ToRadians(m_RandomAngle)) * m_CircleSize} };
	
	Vector2 direction = targetPos - pAgent->GetPosition();
	direction.Normalize();
	steering.LinearVelocity = direction * pAgent->GetMaxLinearSpeed();


	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawCircle(circleCenter, m_CircleSize, { 1,0,0 },0);
		DEBUGRENDERER2D->DrawSolidCircle(targetPos, 3, {0,0}, {1,0,0});
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition() , steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0,1,0 });

	}

	return steering;
}