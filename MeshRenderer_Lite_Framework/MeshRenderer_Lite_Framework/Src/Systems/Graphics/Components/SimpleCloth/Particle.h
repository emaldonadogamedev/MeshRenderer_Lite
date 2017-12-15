#pragma once
#include <DirectXMath.h>

using DirectX::XMVECTOR;

class Particle
{
public:
	Particle(const XMVECTOR& pos);
	Particle();
	~Particle();

	void addForce(const XMVECTOR& f);

	/* This is one of the important methods, where the time is progressed a single step size (TIME_STEPSIZE)
	The method is called by Cloth.time_step()
	Given the equation "force = mass * acceleration" the next position is found through verlet integration*/
	void timeStep(const float dt); 
	XMVECTOR& getPos();

	void resetAcceleration(); 
	void offsetPos(const XMVECTOR& v); 
	void makeUnmovable();

	void addToNormal(XMVECTOR& normal);

	XMVECTOR& getNormal();// notice, the normal is not unit length

	void resetNormal();

private:
	bool m_movable;
	float m_mass;
	XMVECTOR m_position;
	XMVECTOR m_oldPosition;
	XMVECTOR m_acceleration;
	XMVECTOR m_accumulatedNormal;

	static const float s_DAMPING;
	static const float s_TIME_STEPSIZE2;
	static const int s_CONSTRAINT_ITERATIONS;
};