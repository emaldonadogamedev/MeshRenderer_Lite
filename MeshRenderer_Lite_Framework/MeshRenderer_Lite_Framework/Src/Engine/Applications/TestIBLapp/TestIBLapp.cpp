#include <Utilities/precompiled.h>
#include <Engine/Applications/TestIBLapp/TestIBLapp.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Core/GameObjectSystem.h>
#include <Systems/Graphics/GraphicsSystem.h>
#include <Systems/Graphics/CameraClasses/Camera.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>
#include <Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>

TestIBLapp::TestIBLapp()
	:IApplication()
{

}

TestIBLapp::~TestIBLapp()
{

}

bool TestIBLapp::Initialize(HINSTANCE hInstance)
{	
	if (IApplication::Initialize(hInstance)) 
	{
		m_graphicsSystem->GetTestCamera()->m_Position = XMVectorSet(0,0, -7, 1);
		
		testObj = std::make_unique<GameObject>();
		auto transform = new Transform(testObj.get());
		testObj->AddComponent(transform);
		transform->SetScale(3, 3, 3);
		ModelComponent* test3DComp = new ModelComponent(testObj.get());
		auto graphicsSystem = static_cast<GraphicsSystem*>(GetSystem(SystemType::ST_GRAPHICS));
		auto gameObjSystem = static_cast<GameObjectSystem*>(GetSystem(SystemType::ST_GAME_OBJECT_SYSTEM));

		auto model = graphicsSystem->GetModel("sphere");

		model->m_meshEntryList[0].meshMaterial.m_materialProperties.diffuseKd = XMFLOAT4(1, 1, 1, 1);
		model->m_meshEntryList[0].meshMaterial.m_materialProperties.specularKs = XMFLOAT3(12,12,12);
		test3DComp->SetModel(model);
		test3DComp->SetIsActive(true);

		testObj->AddComponent(test3DComp);
		graphicsSystem->AddComponent(test3DComp);
		gameObjSystem->AddComponent(testObj->GetComponent(ComponentType::TRANSFORM));

		//////////////////////////////////////////////////////////////////////////
		//floor game object
		testFloor = std::make_unique<GameObject>();
		transform = new Transform(testFloor.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, -15.6f, 0.f, 1.f));
		transform->SetScale(DirectX::XMVectorSet(2.f, 2.f, 2.f, 1.f));
		testFloor->AddComponent(transform);
		gameObjSystem->AddComponent(transform);
		test3DComp = new ModelComponent(testFloor.get());
		testFloor->AddComponent(test3DComp);
		m_graphicsSystem->AddComponent(test3DComp);
		auto model2 = graphicsSystem->GetModel("box");
		model2->m_meshEntryList[0].meshMaterial.SetToPresetMaterial(PredefinedMaterials::Emerald);
		model2->SetDiffTextureFileName("crate_1.jpg", 0);
		model2->SetNormalMapFileName("Standard_red_pxr256_normal.png", 0, false);
		model2->m_meshEntryList[0].meshMaterial.m_materialProperties.specularPowerNs = 300.f;
		test3DComp->SetModel(model2);

		//////////////////////////////////////////////////////////////////////////
		//test light 1
		testLight = std::make_unique<GameObject>();
		transform = new Transform(testLight.get());
		transform->SetPositionn(DirectX::XMVectorSet(-22.8f, 48.f, 0.f, 1.f));
		testLight->AddComponent(transform);
		gameObjSystem->AddComponent(transform);

		ShadowLightComponent* lightComp = new ShadowLightComponent(testLight.get(), true, false, false);
		//lightComp->GetLight()->m_lightType = LightType::LT_DIRECTIONAL;
		testLight->AddComponent(lightComp);
		graphicsSystem->AddComponent(lightComp);
		lightComp->GetLight()->isActive = 1;

		//enable skybox
		Camera* const testCam = graphicsSystem->GetTestCamera();
		testCam->SetIsUsingSkybox(true);
		testCam->SetSkyboxTexture("Barce_Rooftop_C_3k");

		//enable ibl!
		m_graphicsSystem->SetIBLTexture("Barce_Rooftop_C_3k");
		m_graphicsSystem->SetIsUsingIBL(true);
		m_graphicsSystem->SetIBLsampleWeightCount(23);

		return true;
	}

	return false;
}

void TestIBLapp::Shutdown(void)
{
	IApplication::Shutdown();
}
