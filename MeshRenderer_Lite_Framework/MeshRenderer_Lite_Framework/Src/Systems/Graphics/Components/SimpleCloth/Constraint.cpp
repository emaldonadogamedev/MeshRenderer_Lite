#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/SimpleCloth/Constraint.h>

#include <Systems/Graphics/Components/SimpleCloth/Particle.h>

using namespace DirectX;

Constraint::Constraint(Particle *p1, Particle *p2) 
	: particle1(p1)
	, particle2(p2)
{
	XMVECTOR vec = DirectX::XMVectorSubtract(p1->getPos(), p2->getPos());
	m_restDistance = XmVeclength(vec);
}

void Constraint::satisfyConstraint()
{
	XMVECTOR p1_to_p2 = particle2->getPos() - particle1->getPos(); // vector from p1 to p2
	float current_distance = XmVeclength(p1_to_p2); // current distance between p1 and p2
	XMVECTOR correctionVector = p1_to_p2 * (1.0f - m_restDistance / current_distance); // The offset vector that could moves p1 into a distance of rest_distance to p2
	XMVECTOR correctionVectorHalf = correctionVector*0.5; // Lets make it half that length, so that we can move BOTH p1 and p2.
	particle1->offsetPos(correctionVectorHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
	particle2->offsetPos(-correctionVectorHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.	
}

float Constraint::XmVeclength(const XMVECTOR& v)
{
	return sqrt(XmVeclengthSquared(v));
}

float Constraint::XmVeclengthSquared(const XMVECTOR& v)
{
	return (v.m128_f32[0] * v.m128_f32[0]) +
		(v.m128_f32[1] * v.m128_f32[1]) +
		(v.m128_f32[2] * v.m128_f32[2]);
}
