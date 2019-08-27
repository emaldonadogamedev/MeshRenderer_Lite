#include <DirectXMath.h>
//DirectX Texture loading
#include "DirectXTex\DirectXTex.h"
#include <fstream>
#include<vector>

using namespace DirectX;
using namespace std;

int main()
{

	std::vector<float> floatData = std::vector<float>();

	std::ifstream F_512("F_512_2.csv");
	if (F_512.is_open())
	{
		float w, h;
		F_512 >> w >> h;

		Image img;
		img.width = w;
		img.height = h;
		img.format = DXGI_FORMAT_R32_FLOAT;
		img.rowPitch = sizeof(float) * 512;/*<number of bytes in a scanline of the source data>*/;
		img.slicePitch = sizeof(float) * img.width * img.height/*<number of bytes in the entire 2D image>*/;

		float xStart, xSend;
		F_512 >> xStart >> xSend;

		float yStart, ySend;
		F_512 >> yStart >> ySend;

		///meat
		while (!F_512.eof())
		{
			float temp;
			F_512 >> temp;
			floatData.push_back(temp);
		}

		F_512.close();

		img.pixels = reinterpret_cast<uint8_t*>(floatData.data());
		HRESULT hr = DirectX::SaveToDDSFile(img, 0, L"F_512.DDS");

		if (hr == S_OK)
		{
			int jk = 90;
			jk += 90;
		}
	}
}