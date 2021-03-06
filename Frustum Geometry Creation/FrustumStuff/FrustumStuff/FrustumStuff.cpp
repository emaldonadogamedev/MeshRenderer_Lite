// FrustumStuff.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

struct SimpleVertex
{
    SimpleVertex(const XMFLOAT3& p = XMFLOAT3(0,0,0), const XMFLOAT2& _uv = XMFLOAT2(0,0))
        :position(p)
        ,uv(_uv)
    {
    }

    XMFLOAT3 position;
    XMFLOAT2 uv;
};

int main()
{
	const size_t shadowMapDim = 1024;
	const XMVECTOR lightPosition = XMVectorSet(8.f, 1, 2.f, 1.f);
	const float nearPlane = 0.01f;
	const float farPlane = 300.f;
	const float fov = DirectX::XM_PI / 3.0f;
	const XMVECTOR lookAtPosition = XMVectorSet(0, 0, 0, 1);
	const XMVECTOR upYvector = XMVectorSet(0,1.f,0,0);
	const XMVECTOR lookAtDir = XMVector3Normalize(lookAtPosition - lightPosition);
	const XMVECTOR rightVec = XMVector3Cross(upYvector, lookAtDir);
	
	const XMVECTOR farPlanePos = lightPosition + (lookAtDir * farPlane);
	
	const float heightFar = 2.0f * tan(fov / 2.0f) * farPlane;
	const float widthFar = heightFar;// because aspect ratio is 

	const XMVECTOR fqsPositions[4] = //View Space positions
	{
		XMVectorSet(-1.f, -1.f, 1.0f, 1.f),
		XMVectorSet(-1.f,  1.f, 1.0f, 1.f),
		XMVectorSet(1.f,  1.f,  1.0f, 1.f),
		XMVectorSet(1.f, -1.f,  1.0f, 1.f)
	};

	XMVECTOR matInvDeter;

	const XMMATRIX invViewMtx = XMMatrixInverse(&matInvDeter, XMMatrixLookAtLH(lightPosition, XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0)));
	const XMMATRIX invProjMtx = XMMatrixInverse(&matInvDeter, XMMatrixPerspectiveFovLH(DirectX::XM_PI / 3.0f, 1.0f, 0.01f, 500.0f));


	ofstream outFile("frustumLight.obj", std::ios::trunc);
	if (outFile.is_open())
	{
		vector<SimpleVertex> vertices(5/*shadowMapDim * shadowMapDim + 1*/);

		//The first vertex is going to be the light position
		vertices[0].position = XMFLOAT3(lightPosition.m128_f32[0], lightPosition.m128_f32[1], lightPosition.m128_f32[2]);
		XMVECTOR farBotLeft = farPlanePos - (upYvector * heightFar / 2.0f) - (rightVec * widthFar / 2.0f);
		XMVECTOR farTopLeft = farPlanePos + (upYvector * heightFar / 2.0f) - (rightVec * widthFar / 2.0f);
		XMVECTOR farTopRight = farPlanePos + (upYvector * heightFar / 2.0f) + (rightVec * widthFar / 2.0f);
		XMVECTOR farBotRight = farPlanePos - (upYvector * heightFar / 2.0f) + (rightVec * widthFar / 2.0f);

		vertices[1].position = XMFLOAT3(farBotLeft.m128_f32[0], farBotLeft.m128_f32[1], farBotLeft.m128_f32[2]);
		vertices[2].position = XMFLOAT3(farTopLeft.m128_f32[0], farTopLeft.m128_f32[1], farTopLeft.m128_f32[2]);
		vertices[3].position = XMFLOAT3(farTopRight.m128_f32[0], farTopRight.m128_f32[1], farTopRight.m128_f32[2]);
		vertices[4].position = XMFLOAT3(farBotRight.m128_f32[0], farBotRight.m128_f32[1], farBotRight.m128_f32[2]);


		for(const auto& vert : vertices)
		{
			outFile <<"v " << vert.position.x << ' ' << vert.position.y << ' ' << vert.position.z << '\n';
		}
		outFile << "\n\n";

		outFile << "f  1 2 3\n";
		outFile << "f  1 3 4\n";

		outFile << "f  1 5 2\n";
		outFile << "f  2 5 3\n";
		outFile << "f  3 5 4\n";
		outFile << "f  1 5 4\n";

		for (const auto& vert : vertices)
		{
			outFile << "vt " << vert.uv.x << ' ' << vert.uv.y << '\n';
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
