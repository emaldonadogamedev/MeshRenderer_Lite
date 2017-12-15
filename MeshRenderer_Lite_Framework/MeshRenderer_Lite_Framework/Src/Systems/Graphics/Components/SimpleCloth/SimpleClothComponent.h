#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/Components/SimpleCloth/Particle.h>
#include <Systems/Graphics/Components/SimpleCloth/Constraint.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <vector>

class DX11Renderer;

class SimpleClothComponent : public IComponent
{
public:
	SimpleClothComponent(const GameObject* owner, float width, float height, int numParticlesWidth, int numParticlesHeight);
	~SimpleClothComponent();

	/* this is an important methods where the time is progressed one time step for the entire cloth.
	This includes calling satisfyConstraint() for every constraint, and calling timeStep() for all particles
	*/
	void timeStep(const float dt);

	/* used to add gravity (or any other arbitrary vector) to all particles*/
	void addForce(const XMVECTOR& direction);

	/* used to add wind forces to all particles, is added for each triangle since the final force is proportional to the triangle area as seen from the wind direction*/
	void windForce(const XMVECTOR& direction);

	/* used to detect and resolve the collision of the cloth with the ball.
	This is based on a very simples scheme where the position of each particle is simply compared to the sphere and corrected.
	This also means that the sphere can "slip through" if the ball is small enough compared to the distance in the grid bewteen particles
	*/
	void ballCollision(const XMVECTOR& center, const float radius);

	//Initialization
	void generateVertexBuffers(DX11Renderer* renderContext);

private:
	XMVECTOR m_spherePos;
	float m_sphereRadius = 2.0;
	float m_sphereMoveDirection = 1.0f;
	float m_sphereMoveSpeed = 3.0f;

	ObjectHandle m_drawPointsVB;
	ObjectHandle m_drawPointsIB;
	int m_indexCount;

	int m_numParticlesWidth; // number of particles in "width" direction
	int m_numParticlesHeight; // number of particles in "height" direction
							  // total number of particles is num_particles_width*num_particles_height

	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Constraint> constraints; // all constraints between particles as part of this cloth

	int GetParticleIndex(int x, int y)const;
	Particle* getParticle(int x, int y);
	void makeConstraint(Particle *p1, Particle *p2);

	/* A private method used by drawShaded() and addWindForcesForTriangle() to retrieve the
	normal vector of the triangle defined by the position of the particles p1, p2, and p3.
	The magnitude of the normal vector is equal to the area of the parallelogram defined by p1, p2 and p3
	*/
	XMVECTOR calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3) const;
	
	/* A private method used by windForce() to calcualte the wind force for a single triangle
	defined by p1,p2,p3*/
	void addWindForcesForTriangle(Particle *p1, Particle *p2, Particle *p3, const XMVECTOR& direction);

	float XmVec_Dot(const XMVECTOR& a, const XMVECTOR& b) const;
	XMVECTOR XmVec_Cross(const XMVECTOR& a, const XMVECTOR& b) const;

	float XmVec_Length(const XMVECTOR& a) const;
	float XmVec_LengthSquared(const XMVECTOR& a) const;

	static const int s_CONSTRAINT_ITERATIONS;

	friend class ForwardRenderStage;
	friend class GraphicsSystem;
};