#include <Utilities/precompiled.h>
#include <Systems/Graphics/ModelClasses/PrimitiveGenerator/PrimitiveGenerator.h>


#define PI 3.1415926535897932384626433832795f
float AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f*static_cast<float>(PI); // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + static_cast<float>(PI); // in [0, 2*pi).

	return theta;
}

PrimitiveGenerator::PrimitiveGenerator(void)
{

}

PrimitiveGenerator::~PrimitiveGenerator(void)
{

}

void PrimitiveGenerator::CreateBox(float width, float height, float depth, Model& ModelData)
{
	//
	// Create the vertices.
	//

	std::vector<VertexAnimation> v;
	v.reserve(24);

	const float w2 = 0.5f*width;
	const float h2 = 0.5f*height;
	const float d2 = 0.5f*depth;

	// Fill in the front face Vertex data.
	v.emplace_back(VertexAnimation(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f)); //White
	v.emplace_back(VertexAnimation(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f)); //Black
	v.emplace_back(VertexAnimation(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f)); //Red
	v.emplace_back(VertexAnimation(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f)); //Green

																												 // Fill in the back face Vertex data.
	v.emplace_back(VertexAnimation(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f)); //Blue
	v.emplace_back(VertexAnimation(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f)); //Magenta
	v.emplace_back(VertexAnimation(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f)); //Cyan
	v.emplace_back(VertexAnimation(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f)); //Yellow
																										 // Fill in the top face Vertex data.
	v.emplace_back(VertexAnimation(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f)); //Black
	v.emplace_back(VertexAnimation(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f)); //Yellow
	v.emplace_back(VertexAnimation(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f)); //Cyan
	v.emplace_back(VertexAnimation(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f)); //Red
																											// Fill in the bottom face Vertex data.
	v.emplace_back(VertexAnimation(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f)); //White
	v.emplace_back(VertexAnimation(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f)); //Green
	v.emplace_back(VertexAnimation(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f)); //Magenta
	v.emplace_back(VertexAnimation(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f)); //Blue
																												  // Fill in the left face Vertex data.
	v.emplace_back(VertexAnimation(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f)); //Blue
	v.emplace_back(VertexAnimation(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f)); //Yellow
	v.emplace_back(VertexAnimation(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f)); //Black
	v.emplace_back(VertexAnimation(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f)); //White
																												  // Fill in the right face Vertex data.
	v.emplace_back(VertexAnimation(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f)); //Green
	v.emplace_back(VertexAnimation(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f)); //Red
	v.emplace_back(VertexAnimation(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f)); //Cyan
	v.emplace_back(VertexAnimation(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f)); //Magenta


	ModelData.m_vertices.assign(v.begin(), v.end());

	//
	// Create the indices.
	//

	unsigned i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	ModelData.m_indices.assign(&i[0], &i[sizeof(i)/sizeof(i[0])]);

	MeshEntry me;
	me.baseIndex = 0;
	me.baseVertex = 0;
	me.numIndices = ModelData.m_indices.size();
	ModelData.m_meshEntryList.push_back(me);
}

void PrimitiveGenerator::CreateQuad(float width, float height, float zLevel, Model& ModelData)
{
	// Create the vertices.
	std::vector<VertexAnimation> v;
	v.reserve(4);

	const float w2 = 0.5f*width;
	const float h2 = 0.5f*height;

	// Fill in the front face Vertex data.
	v.emplace_back(VertexAnimation(-w2, -h2, zLevel, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f)); //White
	v.emplace_back(VertexAnimation(-w2, +h2, zLevel, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f)); //Black
	v.emplace_back(VertexAnimation(+w2, +h2, zLevel, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f)); //Red
	v.emplace_back(VertexAnimation(+w2, -h2, zLevel, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f)); //Green

	ModelData.m_vertices.assign(v.begin(), v.end());

	// Create the indices.
	unsigned i[6];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	ModelData.m_indices.assign(&i[0], &i[sizeof(i) / sizeof(i[0])]);

	MeshEntry me;
	me.baseIndex = 0;
	me.baseVertex = 0;
	me.numIndices = ModelData.m_indices.size();
	ModelData.m_meshEntryList.push_back(me);
}

void PrimitiveGenerator::CreateCircle(float radius, int vertexCount, Model& ModelData)
{
		ModelData.m_vertices.clear();
		ModelData.m_indices.clear();

		const float angleDiff = DirectX::XM_2PI / float(vertexCount);

		for (float angle = 0.f; angle < DirectX::XM_2PI; angle += angleDiff)
		{
		}
}

void PrimitiveGenerator::CreateSphere(float radius, unsigned sliceCount, unsigned stackCount, Model& ModelData)
{
	ModelData.m_vertices.clear();
	ModelData.m_indices.clear();

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	VertexAnimation topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);   //WHITE
	VertexAnimation bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);//BLACK

	ModelData.m_vertices.emplace_back(topVertex);

	float phiStep = DirectX::XM_PI / static_cast<float>(stackCount);
	float thetaStep = 2.0f * DirectX::XM_PI / static_cast<float>(sliceCount);

	XMFLOAT4 colorIncrement(
		(bottomVertex.color.x - topVertex.color.x) / static_cast<float>(sliceCount),
		(bottomVertex.color.y - topVertex.color.y) / static_cast<float>(sliceCount),
		(bottomVertex.color.z - topVertex.color.z) / static_cast<float>(sliceCount),
		(bottomVertex.color.w - topVertex.color.w) / static_cast<float>(sliceCount)
	);

	XMFLOAT4 ringcolor( 
		topVertex.color.x + colorIncrement.x,
		topVertex.color.y + colorIncrement.y,
		topVertex.color.z + colorIncrement.z,
		topVertex.color.w + colorIncrement.w
	);

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (unsigned i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*phiStep;

		ringcolor.x += colorIncrement.x;
		ringcolor.y += colorIncrement.y;
		ringcolor.z += colorIncrement.z;
		ringcolor.w += colorIncrement.w;

		// Vertices of ring.
		for (unsigned j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			VertexAnimation v;

			v.color = ringcolor;

			// spherical to cartesian
			v.position.x = radius*sinf(phi)*cosf(theta);
			v.position.y = radius*cosf(phi);
			v.position.z = radius*sinf(phi)*sinf(theta);

			// Partial derivative of P with respect to theta
			v.tangent.x = -radius*sinf(phi)*sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = +radius*sinf(phi)*cosf(theta);

			XMFloat3Normalize(v.tangent);
			//DirectX::XMVECTOR T = XMLoadVector3(&v.tangent);
			//DirectX::XMStoreVector3(&v.tangent, Vector3normalize(T));

			//DirectX::XMVECTOR p = XMLoadVector3(&v.position);
			//DirectX::XMStoreVector3(&v.normal, Vector3normalize(p));

			XMFLOAT3 p = v.position;
			
			XMFloat3Normalize(p);
			v.normal = p;

			v.uv.x = theta / DirectX::XM_2PI;
			v.uv.y = phi / DirectX::XM_PI;

			ModelData.m_vertices.emplace_back(v);
		}
	}

	ModelData.m_vertices.emplace_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the Vertex buffer
	// and connects the top pole to the first ring.
	//

	for (unsigned i = 1; i <= sliceCount; ++i)
	{
		ModelData.m_indices.emplace_back(0);
		ModelData.m_indices.emplace_back(i + 1);
		ModelData.m_indices.emplace_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first Vertex in the first ring.
	// This is just skipping the top pole Vertex.
	unsigned baseIndex = 1;
	unsigned ringVertexCount = sliceCount + 1;
	for (unsigned i = 0; i < stackCount - 2; ++i)
	{
		for (unsigned j = 0; j < sliceCount; ++j)
		{
			ModelData.m_indices.emplace_back(baseIndex + i*ringVertexCount + j);
			ModelData.m_indices.emplace_back(baseIndex + i*ringVertexCount + j + 1);
			ModelData.m_indices.emplace_back(baseIndex + (i + 1)*ringVertexCount + j);

			ModelData.m_indices.emplace_back(baseIndex + (i + 1)*ringVertexCount + j);
			ModelData.m_indices.emplace_back(baseIndex + i*ringVertexCount + j + 1);
			ModelData.m_indices.emplace_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the Vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole Vertex was added last.
	unsigned southPoleIndex = (unsigned)ModelData.m_vertices.size() - 1;

	// Offset the indices to the index of the first Vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (unsigned i = 0; i < sliceCount; ++i)
	{
		ModelData.m_indices.emplace_back(southPoleIndex);
		ModelData.m_indices.emplace_back(baseIndex + i);
		ModelData.m_indices.emplace_back(baseIndex + i + 1);
	}

	MeshEntry me;
	me.baseIndex = 0;
	me.baseVertex = 0;
	me.numIndices = ModelData.m_indices.size();
	ModelData.m_meshEntryList.emplace_back(me);
}

XMFLOAT3 PrimitiveGenerator::XMFloat3GetNormalized(const XMFLOAT3& v) const
{
	float l = (v.x*v.x) + (v.y*v.y) + (v.z*v.z);
	if (std::fabsf(l) <= 0.0001f) {
		return XMFLOAT3(0, 0, 0);
	}

	l = sqrt(l);
	return XMFLOAT3(v.x / l, v.y/l, v.z/l);
}

void PrimitiveGenerator::XMFloat3Normalize(XMFLOAT3& v) const
{
	float l = (v.x*v.x) + (v.y*v.y) + (v.z*v.z);
	if (std::fabsf(l) > 0.0001f) {
		l = sqrt(l);
		v.x /= l; 
		v.y /= l;
		v.z /= l;
		return;
	}
	v.x = v.y = v.z = 0.f;
}

/*
void PrimitiveGenerator::Subdivide(ModelData& modelData)
{
	// Save a copy of the input geometry.
	ModelData inputCopy = modelData;


	modelData.m_vertices.resize(0);
	modelData.m_indices.resize(0);

	//		         v1
	//		         *
	//		        / \
	//		       /   \
	//		    m0*-----*m1
	//		     / \   / \
	//		    /   \ /   \
	//		   *-----*-----*
	//        v0    m2     v2

	unsigned numTris = inputCopy.m_indices.size() / 3;
	for (unsigned i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.m_vertices[inputCopy.m_indices[i * 3 + 0]];
		Vertex v1 = inputCopy.m_vertices[inputCopy.m_indices[i * 3 + 1]];
		Vertex v2 = inputCopy.m_vertices[inputCopy.m_indices[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// Vertex components in CreateGeosphere.

		m0.position = Vector3(
			0.5f*(v0.position.x + v1.position.x),
			0.5f*(v0.position.y + v1.position.y),
			0.5f*(v0.position.z + v1.position.z));

		m1.position = Vector3(
			0.5f*(v1.position.x + v2.position.x),
			0.5f*(v1.position.y + v2.position.y),
			0.5f*(v1.position.z + v2.position.z));

		m2.position = Vector3(
			0.5f*(v0.position.x + v2.position.x),
			0.5f*(v0.position.y + v2.position.y),
			0.5f*(v0.position.z + v2.position.z));

		//
		// Add new geometry.
		//

		modelData.m_vertices.emplace_back(v0); // 0
		modelData.m_vertices.emplace_back(v1); // 1
		modelData.m_vertices.emplace_back(v2); // 2
		modelData.m_vertices.emplace_back(m0); // 3
		modelData.m_vertices.emplace_back(m1); // 4
		modelData.m_vertices.emplace_back(m2); // 5

		modelData.m_indices.emplace_back(i * 6 + 0);
		modelData.m_indices.emplace_back(i * 6 + 3);
		modelData.m_indices.emplace_back(i * 6 + 5);
							
		modelData.m_indices.emplace_back(i * 6 + 3);
		modelData.m_indices.emplace_back(i * 6 + 4);
		modelData.m_indices.emplace_back(i * 6 + 5);
							
		modelData.m_indices.emplace_back(i * 6 + 5);
		modelData.m_indices.emplace_back(i * 6 + 4);
		modelData.m_indices.emplace_back(i * 6 + 2);
							
		modelData.m_indices.emplace_back(i * 6 + 3);
		modelData.m_indices.emplace_back(i * 6 + 1);
		modelData.m_indices.emplace_back(i * 6 + 4);
	}
}

void PrimitiveGenerator::CreateGeosphere(float radius, unsigned numSubdivisions, ModelData& modelData)
{
	// Put a cap on the number of subdivisions.
	numSubdivisions = Min(numSubdivisions, 5u);

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vector3 pos[12] =
	{
		Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),
		Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
		Vector3(0.0f, Z, X), Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X),
		Vector3(Z, X, 0.0f), Vector3(-Z, X, 0.0f),
		Vector3(Z, -X, 0.0f), Vector3(-Z, -X, 0.0f)
	};

	unsigned long k[60] =
	{
		1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4,
		1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7, 2,
		3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0,
		10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7
	};

	modelData.m_vertices.resize(12);
	modelData.m_indices.resize(60);

	for (unsigned i = 0; i < 12; ++i)
		modelData.m_vertices[i].position = pos[i];

	for (unsigned i = 0; i < 60; ++i)
		modelData.m_indices[i] = k[i];

	for (unsigned i = 0; i < numSubdivisions; ++i)
		Subdivide(modelData);

	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4 colorIncrement = (Vector4(0.0f, 0.0f, 0.0f, 1.0f) - color) / static_cast<float>(modelData.m_vertices.size());



	// Project vertices onto sphere and scale.
	for (unsigned i = 0; i < modelData.m_vertices.size(); ++i)
	{
		// Project onto unit sphere.
		//DirectX::XMVECTOR n = Vector3normalize(XMLoadVector3(&modelData.m_vertices[i].position));
		Vector3 p = modelData.m_vertices[i].position;
		p.Normalize();

		Vector3 n = p;

		// Project onto sphere.
		p = n * radius;

		//XMStoreVector3(&ModelData.m_vertices[i].position, p);
		//XMStoreVector3(&ModelData.m_vertices[i].normal, n);

		modelData.m_vertices[i].position = p;
		modelData.m_vertices[i].normal = n;

		// Derive texture coordinates from spherical coordinates.
		float theta = AngleFromXY(
			modelData.m_vertices[i].position.x,
			modelData.m_vertices[i].position.z);

		float phi = acosf(modelData.m_vertices[i].position.y / radius);

		modelData.m_vertices[i].uv.x = theta / DirectX::XM_2PI;
		modelData.m_vertices[i].uv.y = phi / DirectX::XM_PI;

		// Partial derivative of P with respect to theta
		modelData.m_vertices[i].tangent.x = -radius*sinf(phi)*sinf(theta);
		modelData.m_vertices[i].tangent.y = 0.0f;
		modelData.m_vertices[i].tangent.z = +radius*sinf(phi)*cosf(theta);

		modelData.m_vertices[i].tangent.Normalize();

		//DirectX::XMVECTOR T = XMLoadVector3(&modelData.m_vertices[i].tangent);
		//XMStoreVector3(&modelData.m_vertices[i].tangent, Vector3normalize(T));

		color += colorIncrement;

		modelData.m_vertices[i].color = color;
	}
}

void PrimitiveGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, unsigned sliceCount, unsigned stackCount, ModelData& modelData)
{
	modelData.m_vertices.clear();
	modelData.m_indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	unsigned ringCount = stackCount + 1;

	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4 colorInccrement = (Vector4(0.0f, 0.0f, 0.0f, 1.0f) - Vector4(1.0f, 1.0f, 1.0f, 1.0f)) / static_cast<float>(ringCount);

	Vector4 bottomcolor = color;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (unsigned i = 0; i < ringCount; ++i)
	{
		float y = -0.5f*height + i*stackHeight;
		float r = bottomRadius + i*radiusStep;

		color += colorInccrement;

		// vertices of ring
		float dTheta = 2.0f* DirectX::XM_PI / sliceCount;
		for (unsigned j = 0; j <= sliceCount; ++j)
		{
			Vertex Vertex;



			Vertex.color = color;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			Vertex.position = Vector3(r*c, y, r*s);

			Vertex.uv.x = (float)j / sliceCount;
			Vertex.uv.y = 1.0f - (float)i / stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//   Let r0 be the bottom radius and let r1 be the top radius.
			//   y(v) = h - hv for v in [0,1].
			//   r(v) = r1 + (r0-r1)v
			//
			//   x(t, v) = r(v)*cos(t)
			//   y(t, v) = h - hv
			//   z(t, v) = r(v)*sin(t)
			// 
			//  dx/dt = -r(v)*sin(t)
			//  dy/dt = 0
			//  dz/dt = +r(v)*cos(t)
			//
			//  dx/dv = (r0-r1)*cos(t)
			//  dy/dv = -h
			//  dz/dv = (r0-r1)*sin(t)

			// This is unit length.
			Vertex.tangent = Vector3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			Vector3 bitangent(dr*c, -height, dr*s);

			//DirectX::XMVECTOR T = XMLoadVector3(&Vertex.tangent);

			Vector3 t = Vertex.tangent;
			//DirectX::XMVECTOR B = XMLoadVector3(&bitangent);

			Vector3 b = bitangent;

			//DirectX::XMVECTOR N = Vector3normalize(Vector3Cross(T, B));

			Vector3 n = t.Cross(b);
			n.Normalize();

			//XMStoreVector3(&Vertex.normal, N);
			Vertex.normal = n;

			modelData.m_vertices.emplace_back(Vertex);
		}
	}

	// Add one because we duplicate the first and last Vertex per ring
	// since the texture coordinates are different.
	unsigned ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (unsigned i = 0; i < stackCount; ++i)
	{
		for (unsigned j = 0; j < sliceCount; ++j)
		{
			modelData.m_indices.emplace_back(i*ringVertexCount + j);
			modelData.m_indices.emplace_back((i + 1)*ringVertexCount + j);
			modelData.m_indices.emplace_back((i + 1)*ringVertexCount + j + 1);
								
			modelData.m_indices.emplace_back(i*ringVertexCount + j);
			modelData.m_indices.emplace_back((i + 1)*ringVertexCount + j + 1);
			modelData.m_indices.emplace_back(i*ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, modelData, color);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, modelData, bottomcolor);
}

void PrimitiveGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
	unsigned sliceCount, unsigned stackCount, ModelData& ModelData,
	const Vector4& color)
{
	unsigned baseIndex = (unsigned)ModelData.m_vertices.size();

	float y = 0.5f*height;
	float dTheta = 2.0f* DirectX::XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (unsigned i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius*cosf(i*dTheta);
		float z = topRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		ModelData.m_vertices.emplace_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v, color.x, color.y, color.z, color.w));
	}

	// Cap center Vertex.
	ModelData.m_vertices.emplace_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, color.x, color.y, color.z, color.w));

	// Index of center Vertex.
	unsigned centerIndex = (unsigned)ModelData.m_vertices.size() - 1;

	for (unsigned i = 0; i < sliceCount; ++i)
	{
		ModelData.m_indices.emplace_back(centerIndex);
		ModelData.m_indices.emplace_back(baseIndex + i + 1);
		ModelData.m_indices.emplace_back(baseIndex + i);
	}
}

void PrimitiveGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
	unsigned sliceCount, unsigned stackCount, ModelData& ModelData, const Vector4& color)
{
	// 
	// Build bottom cap.
	//

	unsigned baseIndex = (unsigned)ModelData.m_vertices.size();
	float y = -0.5f*height;

	// vertices of ring
	float dTheta = 2.0f* DirectX::XM_PI / sliceCount;
	for (unsigned i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius*cosf(i*dTheta);
		float z = bottomRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		ModelData.m_vertices.emplace_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v, color.x, color.y, color.z, color.w));
	}

	// Cap center Vertex.
	ModelData.m_vertices.emplace_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, color.x, color.y, color.z, color.w));

	// Cache the index of center Vertex.
	unsigned centerIndex = (unsigned)ModelData.m_vertices.size() - 1;

	for (unsigned i = 0; i < sliceCount; ++i)
	{
		ModelData.m_indices.emplace_back(centerIndex);
		ModelData.m_indices.emplace_back(baseIndex + i);
		ModelData.m_indices.emplace_back(baseIndex + i + 1);
	}
}

void PrimitiveGenerator::CreateGrid(float width, float depth, unsigned m, unsigned n, ModelData& ModelData)
{
	const unsigned VertexCount = m*n;
	const unsigned faceCount = (m - 1)*(n - 1) * 2;

	//
	// Create the vertices.
	//

	const float halfWidth = 0.5f*width;
	const float halfDepth = 0.5f*depth;

	const float dx = width / (n - 1);
	const float dz = depth / (m - 1);
	
	const float du = 1.0f / (n - 1);
	const float dv = 1.0f / (m - 1);

	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4 colorInccrement = (Vector4(0.0f, 0.0f, 0.0f, 1.0f) - Vector4(1.0f, 1.0f, 1.0f, 1.0f)) / static_cast<float>(m);

	ModelData.m_vertices.resize(VertexCount);
	for (unsigned i = 0; i < m; ++i)
	{
		color += colorInccrement;

		float z = halfDepth - i*dz;
		for (unsigned j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			ModelData.m_vertices[i*n + j].position = Vector3(x, 0.0f, z);
			ModelData.m_vertices[i*n + j].normal = Vector3(0.0f, 1.0f, 0.0f);
			ModelData.m_vertices[i*n + j].tangent = Vector3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			ModelData.m_vertices[i*n + j].uv.x = j*du;
			ModelData.m_vertices[i*n + j].uv.y = i*dv;

			ModelData.m_vertices[i*n + j].color = color;
		}
	}

	//
	// Create the indices.
	//

	ModelData.m_indices.resize(faceCount * 3); // 3 indices per face

											 // Iterate over each quad and compute indices.
	unsigned k = 0;
	for (unsigned i = 0; i < m - 1; ++i)
	{
		for (unsigned j = 0; j < n - 1; ++j)
		{
			ModelData.m_indices[k] = i*n + j;
			ModelData.m_indices[k + 1] = i*n + j + 1;
			ModelData.m_indices[k + 2] = (i + 1)*n + j;

			ModelData.m_indices[k + 3] = (i + 1)*n + j;
			ModelData.m_indices[k + 4] = i*n + j + 1;
			ModelData.m_indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}
}

//void PrimitiveGenerator::CreateSamplingPlane(std::vector<Vector3>& Vertexpositions, Matrix4 view, ModelData& modelData)
//{
//
//	std::vector<Vector3> viewSpacePoints;
//	viewSpacePoints.resize(Vertexpositions.size());
//	float viewSpaceZ = 0.0f;
//
//	//transforms the vertices to view space
//	for (unsigned int i = 0; i < viewSpacePoints.size(); ++i)
//	{
//		viewSpacePoints[i] = view * Vertexpositions[i];
//		viewSpaceZ += viewSpacePoints[i].z;
//	}
//
//	viewSpaceZ /= Vertexpositions.size();
//
//	//Calculates the hull in view space
//	std::vector<float2> hull = FindConvexHull(viewSpacePoints);
//
//	std::vector<Vector3> worldSpaceVerts;
//	worldSpaceVerts.resize(hull.size());
//
//	//Transforms the vertices back to world space
//	for (unsigned int i = 0; i < hull.size(); ++i)
//	{
//		worldSpaceVerts[i] = Inverse(view) * Vector3(hull[i].x, hull[i].y, viewSpaceZ);
//	}
//
//	modelData.m_indices.resize((hull.size() - 2) * 3);
//	for (unsigned int i = 0, x = 1; i < modelData.m_indices.size(); ++x, i += 3)
//	{
//		modelData.m_indices[i] = 0;
//		modelData.m_indices[i + 1] = x;
//		modelData.m_indices[i + 2] = x + 1;
//	}
//
//	modelData.m_vertices.resize(hull.size());
//	for (unsigned int i = 0; i < modelData.m_vertices.size(); ++i)
//	{
//		Vertex v;
//		v.position = worldSpaceVerts[i];
//		v.color = Vector4(1, 1, 1, 1);
//
//		modelData.m_vertices[i] = v;
//	}
//
//}
//
//std::vector<float2> PrimitiveGenerator::FindConvexHull(const std::vector<Vector3>& Vertexpositions)
//{
//	std::vector<float2> corners2D;
//	corners2D.resize(Vertexpositions.size());
//
//	for (unsigned int i = 0; i < corners2D.size(); ++i)
//	{
//		corners2D[i] = float2(Vertexpositions[i].x, Vertexpositions[i].y);
//	}
//
//	int mostLeftIndex = -1;
//	float minX = 9999999999999999999999999999999999.9f;
//
//	for (unsigned int x = 0; x < corners2D.size(); ++x)
//	{
//		if (corners2D[x].x < minX)
//		{
//			mostLeftIndex = x;
//			minX = corners2D[x].x;
//		}
//	}
//
//
//	std::vector<int> pathIndices;
//
//	float2 srcLine(0, 1);
//	int currentPointIndex = mostLeftIndex;
//	pathIndices.push_back(currentPointIndex);
//
//	bool keepLooking = true;
//
//	while (true)
//	{
//		float maxDot = -99999999999999999999999999999999.9f;
//		int maxDotIndex = -1;
//
//		for (unsigned int x = 0; x < corners2D.size(); ++x)
//		{
//
//			float2 testLine = normalize(corners2D[x] - corners2D[currentPointIndex]);
//
//			if (testLine.x == 0.0f && testLine.y == 0.0f)
//				continue;
//
//			float dot = Dot(srcLine, testLine);
//
//			if (dot > maxDot)
//			{
//				maxDot = dot;
//				maxDotIndex = x;
//			}
//		}
//
//		if (maxDotIndex == pathIndices[0])
//		{
//			break;
//		}
//
//
//		else
//		{
//			pathIndices.push_back(maxDotIndex);
//			srcLine = normalize(corners2D[maxDotIndex] - corners2D[currentPointIndex]);
//			currentPointIndex = maxDotIndex;
//		}
//
//
//	}
//
//	std::vector<float2> hullPoints;
//	hullPoints.resize(pathIndices.size());
//	for (unsigned int i = 0; i < hullPoints.size(); ++i)
//	{
//		hullPoints[i] = corners2D[pathIndices[i]];
//	}
//
//	return hullPoints;
//}

void PrimitiveGenerator::CreateFullscreenQuad(ModelData& ModelData)
{
	ModelData.m_vertices.resize(4);
	ModelData.m_indices.resize(6);

	// position coordinates specified in NDC space.
	ModelData.m_vertices[0] = Vertex(
		-1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f);

	ModelData.m_vertices[1] = Vertex(
		-1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f);

	ModelData.m_vertices[2] = Vertex(
		+1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f);

	ModelData.m_vertices[3] = Vertex(
		+1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ModelData.m_indices[0] = 0;
	ModelData.m_indices[1] = 1;
	ModelData.m_indices[2] = 2;

	ModelData.m_indices[3] = 0;
	ModelData.m_indices[4] = 2;
	ModelData.m_indices[5] = 3;
}

void PrimitiveGenerator::CreateTeapot(const unsigned n, ModelData& ModelData)
{
	CreateTeapot(1, n, ModelData);
}

// Tessellates the specified bezier patch.
void _fastcall PrimitiveGenerator::TessellatePatch(ModelData& ModelData, TeapotPatch const& patch, size_t tessellation, DirectX::FXMVECTOR scale, bool isMirrored, Vector4 color)
{
	// Look up the 16 control points for this patch.
	//	DirectX::XMVECTOR controlPoints[16];
	//
	//	for (int i = 0; i < 16; i++)
	//	{
	//		controlPoints[i] = TeapotControlPoints[patch.m_indices[i]] * scale;
	//	}
	//
	//	// Create the index data.
	//	size_t vbase = ModelData.m_vertices.size();
	//	Bezier::CreatePatchIndices(tessellation, isMirrored, [&](size_t index)
	//	{
	//		ModelData.m_indices.push_back(vbase + index);
	//	});
	//
	//	// Create the Vertex data.
	//	Bezier::CreatePatchVertices(controlPoints, tessellation, isMirrored, [&](DirectX::FXMVECTOR position, DirectX::FXMVECTOR normal, DirectX::FXMVECTOR textureCoordinate)
	//	{
	//		ModelData.m_vertices.push_back(Vertex(position.m128_f32[2], position.m128_f32[1], position.m128_f32[0],
	//			normal.m128_f32[2], normal.m128_f32[1], normal.m128_f32[0],
	//			0.0f, 1.0f, 0.0f,
	//			textureCoordinate.m128_f32[0], textureCoordinate.m128_f32[1],
	//			color.x, color.y, color.z, color.w));
	//	});
}


// Creates a teapot primitive.
void PrimitiveGenerator::CreateTeapot(float size, size_t tessellation, ModelData& ModelData)
{
	ModelData.m_vertices.clear();
	ModelData.m_indices.clear();

	if (tessellation < 1)
		throw std::out_of_range("tesselation parameter out of range");

	DirectX::XMVECTOR scaleVector = DirectX::XMVectorReplicate(size);

	DirectX::XMVECTOR scaleNegateX = scaleVector * DirectX::g_XMNegateX;
	DirectX::XMVECTOR scaleNegateZ = scaleVector * DirectX::g_XMNegateZ;
	DirectX::XMVECTOR scaleNegateXZ = scaleVector * DirectX::g_XMNegateX * DirectX::g_XMNegateZ;

	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4 colorIncrement = (Vector4(0.0f, 0.0f, 0.0f, 1.0f) - color) / (sizeof(TeapotPatches) / sizeof(TeapotPatches[0]));

	for (int i = 0; i < sizeof(TeapotPatches) / sizeof(TeapotPatches[0]); i++)
	{
		TeapotPatch const& patch = TeapotPatches[i];

		color += colorIncrement;

		// Because the teapot is symmetrical from left to right, we only store
		// data for one side, then tessellate each patch twice, mirroring in X.
		TessellatePatch(ModelData, patch, tessellation, scaleVector, false, color);
		TessellatePatch(ModelData, patch, tessellation, scaleNegateX, true, color);

		if (patch.mirrorZ)
		{
			color += colorIncrement;
			// Some parts of the teapot (the body, lid, and rim, but not the
			// handle or spout) are also symmetrical from front to back, so
			// we tessellate them four times, mirroring in Z as well as X.
			TessellatePatch(ModelData, patch, tessellation, scaleNegateZ, true, color);
			TessellatePatch(ModelData, patch, tessellation, scaleNegateXZ, false, color);
		}
	}

	int i = ModelData.m_vertices.size();
	int j = ModelData.m_indices.size();
}

*/