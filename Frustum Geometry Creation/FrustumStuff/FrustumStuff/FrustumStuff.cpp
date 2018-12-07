// FrustumStuff.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

int main()
{
	const size_t shadowMapDim = 1024;
	const XMVECTOR lightPosition = XMVectorSet(8.f, 48.f, 0.f, 1.f);
	const float farPlaneRange = 500.f;
	const XMVECTOR lookAtPosition = XMVectorSet(0, 0, 0, 1);
	const XMVECTOR upYvector = XMVectorSet(0,1.f,0,0);
	const XMVECTOR fqsPositions[4] = //View Space positions
	{
		XMVectorSet(-1.f, -1.f, 0.f, 1.f),
		XMVectorSet(-1.f,  1.f, 0.f, 1.f),
		XMVectorSet(1.f,  1.f, 0.f, 1.f),
		XMVectorSet(1.f, -1.f, 0.f, 1.f)
	};

	const XMMATRIX viewMtx = XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixLookAtLH(lightPosition, XMVectorSet(0,0,0,1), XMVectorSet(0, 1, 0, 0))));
	const XMMATRIX projMtx = XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1, 0.01f, 1000.0f)));


	ofstream outFile("frustumLight.obj", std::ios::trunc);
	if (outFile.is_open())
	{
		vector<XMFLOAT3> vertices(shadowMapDim * shadowMapDim + 1, XMFLOAT3(0, 0, 0));

		//The first vertex is going to be the light position
		vertices[0] = XMFLOAT3(lightPosition.m128_f32[0], lightPosition.m128_f32[1], lightPosition.m128_f32[2]);

		const XMVECTOR lightToOrigVecNorm = DirectX::XMVector3Normalize(lightPosition - lookAtPosition);
		const XMVECTOR pointAtRange = lightToOrigVecNorm * farPlaneRange;

		for(const auto& vert : vertices)
		{
			outFile <<"v " << vert.x << ' ' << vert.y << ' ' << vert.z << '\n';
		}
		

		outFile.close();

		return 0;
	}

	return -1;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
