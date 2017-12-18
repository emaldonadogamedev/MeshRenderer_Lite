#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/SimpleCloth/Particle.h>

using namespace DirectX;

Particle::Particle(const XMVECTOR& pos)
	: m_position(pos)
	, m_oldPosition(pos)
	, m_acceleration()
	, m_movable(true)
	, m_accumulatedNormal()
{
}

Particle::Particle()
{

}

Particle::~Particle()
{

}

void Particle::addForce(const XMVECTOR& f)
{
	m_acceleration = DirectX::XMVectorAdd(m_acceleration, XMVectorScale(f, 1.0f/ s_PARTICLE_MASS));// / mass;
}

void Particle::timeStep(const float dt)
{
	if (m_movable)
	{
		XMVECTOR temp = m_position;
		m_position = m_position + (m_position - m_oldPosition) * (1.0 - s_DAMPING) + m_acceleration * dt;// s_TIME_STEPSIZE2;
		m_oldPosition = temp;
		m_acceleration = XMVectorSet(0, 0, 0, 0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)	
	}
}

XMVECTOR& Particle::getPos()
{
	return m_position;
}

void Particle::resetAcceleration()
{
	m_acceleration = XMVectorSet(0,0,0,0);
}


void Particle::offsetPos(const XMVECTOR& v)
{
	if (m_movable)
	{
		m_position = XMVectorAdd(m_position, v);
	}
}

void Particle::makeUnmovable()
{
	m_movable = false;
}

void Particle::addToNormal(XMVECTOR& normal)
{
	m_accumulatedNormal = DirectX::XMVector3Normalize(normal);
}

XMVECTOR& Particle::getNormal()
{
	return m_accumulatedNormal;
}

void Particle::resetNormal()
{
	m_accumulatedNormal = XMVectorSet(0, 0, 0, 0);
}

float Particle::s_PARTICLE_MASS = 1.0f;

const float Particle::s_DAMPING = 0.001f;

const float Particle::s_TIME_STEPSIZE2= 0.5f * 0.5f;

const int Particle::s_CONSTRAINT_ITERATIONS = 15;