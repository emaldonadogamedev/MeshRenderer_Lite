#pragma once
#include <DirectXMath.h>

using DirectX::XMVECTOR;

class Particle;

class Constraint
{
public:
	Particle *particle1, *particle2; // the two particles that are connected through this constraint

	Constraint(Particle *p1, Particle *p2);

	/* This is one of the important methods, where a single constraint between two particles p1 and p2 is solved
	the method is called by Cloth.time_step() many times per frame*/
	void satisfyConstraint();

private:
	float m_restDistance; // the length between particle p1 and p2 in rest configuration

	static float XmVeclength(const XMVECTOR& v);
	static float XmVeclengthSquared(const XMVECTOR& v);
};