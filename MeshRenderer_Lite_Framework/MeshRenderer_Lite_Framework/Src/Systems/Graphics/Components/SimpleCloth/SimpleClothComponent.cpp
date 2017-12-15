#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>

#include <Systems/Input/InputSystem.h>
#include <Systems/Input/Keyboard.h>h

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>

using namespace DirectX;

SimpleClothComponent::SimpleClothComponent(const GameObject* owner, float width, float height, int numParticlesWidth, int numParticlesHeight)
	:IComponent(ComponentType::PHYSICS_SIMPLE_CLOTH, owner)
	, m_numParticlesWidth(numParticlesWidth)
	, m_numParticlesHeight(numParticlesHeight)

{
	particles.resize(numParticlesWidth*numParticlesHeight); //I am essentially using this vector as an array with room for numParticlesWidth*numParticlesHeight particles
																// creating particles in a grid of particles from (0,0,0) to (width,height,0)
	for (int x = 0; x < numParticlesWidth; x++)
	{
		for (int y = 0; y < numParticlesHeight; y++)
		{
			const XMVECTOR pos =  XMVectorSet(width * (x / (float)numParticlesWidth),
				-height * (y / (float)numParticlesHeight),
				0,
				1.0f);
			particles[y*numParticlesWidth + x] = Particle(pos); // insert particle in column x at y'th row
		}
	}

	// Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid)
	for (int x = 0; x < numParticlesWidth; x++)
	{
		for (int y = 0; y < numParticlesHeight; y++)
		{
			if (x < numParticlesWidth - 1)
				makeConstraint(getParticle(x, y), getParticle(x + 1, y));
			if (y < numParticlesHeight - 1) 
				makeConstraint(getParticle(x, y), getParticle(x, y + 1));
			if (x < numParticlesWidth - 1 && y < numParticlesHeight - 1) 
				makeConstraint(getParticle(x, y), getParticle(x + 1, y + 1));
			if (x < numParticlesWidth - 1 && y < numParticlesHeight - 1) 
				makeConstraint(getParticle(x + 1, y), getParticle(x, y + 1));
		}
	}


	// Connecting secondary neighbors with constraints (distance 2 and sqrt(4) in the grid)
	for (int x = 0; x < numParticlesWidth; x++)
	{
		for (int y = 0; y < numParticlesHeight; y++)
		{
			if (x < numParticlesWidth - 2) 
				makeConstraint(getParticle(x, y), getParticle(x + 2, y));
			if (y < numParticlesHeight - 2) 
				makeConstraint(getParticle(x, y), getParticle(x, y + 2));
			if (x < numParticlesWidth - 2 && y < numParticlesHeight - 2) 
				makeConstraint(getParticle(x, y), getParticle(x + 2, y + 2));
			if (x < numParticlesWidth - 2 && y < numParticlesHeight - 2)
				makeConstraint(getParticle(x + 2, y), getParticle(x, y + 2));
		}
	}


	// making the upper left most three and right most three particles unmovable
	for (int i = 0; i < 3; i++)
	{
		getParticle(i, 0)->offsetPos(XMVectorSet(0.5f, 0.f, 0.f, 0.f)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
		getParticle(i, 0)->makeUnmovable();

		getParticle(i, 0)->offsetPos(XMVectorSet(-0.5f, 0.f, 0.f, 0.f)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
		getParticle(numParticlesWidth - 1 - i, 0)->makeUnmovable();
	}

	m_spherePos = XMVectorSet(0, -3.0f, -9.f, 1.0f);
}

SimpleClothComponent::~SimpleClothComponent()
{

}

void SimpleClothComponent::timeStep(const float dt)
{
	std::vector<Constraint>::iterator constraint;
	for (int i = 0; i < s_CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
	{
		for (constraint = constraints.begin(); constraint != constraints.end(); constraint++)
		{
			(*constraint).satisfyConstraint(); // satisfy constraint.
		}
	}

	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).timeStep(dt); // calculate the position of each particle at the next time step.
	}

	if (m_spherePos.m128_f32[2] > 10.0f)
	{
		m_sphereMoveDirection = -1.0f;
	}
	else if (m_spherePos.m128_f32[2] < -10.0f)
	{
		m_sphereMoveDirection = 1.0f;
	}

	m_spherePos.m128_f32[2] += m_sphereMoveDirection * m_sphereMoveSpeed * dt;

	ballCollision(m_spherePos, m_sphereRadius);
}

void SimpleClothComponent::addForce(const XMVECTOR& direction)
{
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		particle->addForce(direction); // add the forces to each particle
	}
}

void SimpleClothComponent::windForce(const XMVECTOR& direction)
{
	for (int x = 0; x < m_numParticlesWidth - 1; x++)
	{
		for (int y = 0; y < m_numParticlesHeight - 1; y++)
		{
			addWindForcesForTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), direction);
			addWindForcesForTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), direction);
		}
	}
}

void SimpleClothComponent::ballCollision(const XMVECTOR& center, const float radius)
{
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		const XMVECTOR particleToCenter = particle->getPos() - center;
		const float lengthSquared = XmVec_LengthSquared(particleToCenter);
		if (lengthSquared < radius * radius) // if the particle is inside the ball
		{
			//only use sqrt when we need it
			const float length = sqrt(lengthSquared);
			particle->offsetPos(XMVector3Normalize(particleToCenter) * (radius - lengthSquared)); // project the particle to the surface of the ball
		}
	}
}

void SimpleClothComponent::generateVertexBuffers(DX11Renderer* renderContext)
{
	std::vector<VertexAnimation> vertices(particles.size());
	std::vector<unsigned int> indices;// ((m_numParticlesWidth - 1) * (m_numParticlesHeight - 1) * 12);

	for (int p = 0; p < particles.size(); ++p)
	{
		DirectX::XMStoreFloat3(&vertices[p].position, particles[p].getPos());
	}

	for (int j  = 0; j <  m_numParticlesHeight - 1; ++j)
	{
		for (int i = 0; i < m_numParticlesWidth - 1; ++i)
		{
			//face 2 of quad
			indices.emplace_back(GetParticleIndex(i, j));
			indices.emplace_back(GetParticleIndex(i + 1, j + 1));
			indices.emplace_back(GetParticleIndex(i, j + 1));

			//face 1 of quad
			indices.emplace_back(GetParticleIndex(i, j));
			indices.emplace_back(GetParticleIndex(i + 1, j));
			indices.emplace_back(GetParticleIndex(i + 1, j + 1));
		}
	}

	m_indexCount = indices.size();

	renderContext->CreateVertexBuffer(m_drawPointsVB, BufferUsage::USAGE_DEFAULT, sizeof(VertexAnimation) * vertices.size(), vertices.data());
	renderContext->CreateIndexBuffer(m_drawPointsIB, BufferUsage::USAGE_DEFAULT, sizeof(unsigned int) * indices.size(), indices.data());
}

int SimpleClothComponent::GetParticleIndex(int x, int y) const
{
	return y * m_numParticlesWidth + x;
}

Particle* SimpleClothComponent::getParticle(int x, int y)
{
	return &particles[GetParticleIndex(x,y)];
}

void SimpleClothComponent::makeConstraint(Particle *p1, Particle *p2)
{
	constraints.emplace_back(std::move(Constraint(p1, p2)));
}

XMVECTOR SimpleClothComponent::calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3) const
{
	XMVECTOR& pos1 = p1->getPos();
	XMVECTOR& pos2 = p2->getPos();
	XMVECTOR& pos3 = p3->getPos();

	const XMVECTOR v1 = pos2 - pos1;
	const XMVECTOR v2 = pos3 - pos1;

	return XmVec_Cross(v1,v2);
}

void SimpleClothComponent::addWindForcesForTriangle(Particle *p1, Particle *p2, Particle *p3, const XMVECTOR& direction)
{
	const XMVECTOR normalRaw = calcTriangleNormal(p1, p2, p3);
	const XMVECTOR nowNormalized = XMVector3Normalize(normalRaw);
	const XMVECTOR force = normalRaw * XmVec_Dot(nowNormalized,direction);

	p1->addForce(force);
	p2->addForce(force);
	p3->addForce(force);
}

float SimpleClothComponent::XmVec_Dot(const XMVECTOR& a, const XMVECTOR& b) const
{
	return (a.m128_f32[0] * b.m128_f32[0]) + (a.m128_f32[1] * b.m128_f32[1]) + (a.m128_f32[2] * b.m128_f32[2]);
}

XMVECTOR SimpleClothComponent::XmVec_Cross(const XMVECTOR& a, const XMVECTOR& b) const
{
	return DirectX::XMVector3Cross(a, b);

	//XMVECTOR result;
	//
	//result.m128_f32[0] =   (a.m128_f32[1] * b.m128_f32[2]) - (b.m128_f32[1] * a.m128_f32[2]);
	//result.m128_f32[1] = -((a.m128_f32[0] * b.m128_f32[2]) - (b.m128_f32[0] * a.m128_f32[2]));
	//result.m128_f32[2] =   (a.m128_f32[0] * b.m128_f32[1]) - (b.m128_f32[0] * a.m128_f32[1]);
	//
	//return result;
}

float SimpleClothComponent::XmVec_Length(const XMVECTOR& a) const
{
	return sqrt(XmVec_LengthSquared(a));
}

float SimpleClothComponent::XmVec_LengthSquared(const XMVECTOR& a) const
{
	return (a.m128_f32[0] * a.m128_f32[0]) + (a.m128_f32[1] * a.m128_f32[1]) + (a.m128_f32[2] * a.m128_f32[2]);
}

const int SimpleClothComponent::s_CONSTRAINT_ITERATIONS = 100;
