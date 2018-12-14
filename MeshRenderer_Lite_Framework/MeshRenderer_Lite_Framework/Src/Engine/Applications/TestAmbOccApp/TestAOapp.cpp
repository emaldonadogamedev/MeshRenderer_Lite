#include <Utilities/precompiled.h>
#include <Engine/Applications/TestAmbOccApp/TestAOapp.h>

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

TestAmbOcclussionApp::TestAmbOcclussionApp()
	:IApplication()
{

}

TestAmbOcclussionApp::~TestAmbOcclussionApp()
{

}

bool TestAmbOcclussionApp::Initialize(HINSTANCE hInstance)
{
	if (IApplication::Initialize(hInstance))
	{
		Transform* transform;
		ModelComponent* test3DComp;

		auto graphicsSystem = static_cast<GraphicsSystem*>(GetSystem(SystemType::ST_GRAPHICS));
		auto gameObjSystem = static_cast<GameObjectSystem*>(GetSystem(SystemType::ST_GAME_OBJECT_SYSTEM));

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

		//////////////////////////////////////////////////////////////////////////
		//floor game object
		testFloor = std::make_unique<GameObject>();
		transform = new Transform(testFloor.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, -15.6f, 0.f, 1.f));
		transform->SetScale(DirectX::XMVectorSet(30.f, 1.f, 30.f, 1.f));
		testFloor->AddComponent(transform);
		gameObjSystem->AddComponent(transform);
		test3DComp = new ModelComponent(testFloor.get());
		testFloor->AddComponent(test3DComp);
		m_graphicsSystem->AddComponent(test3DComp);
		Model* model1 = graphicsSystem->GetModel("box");
		model1->m_meshEntryList[0].meshMaterial.SetToPresetMaterial(PredefinedMaterials::Emerald);
		model1->SetDiffTextureFileName("crate_1.jpg", 0);
		model1->SetNormalMapFileName("Standard_red_pxr256_normal.png", 0, false);
		model1->m_meshEntryList[0].meshMaterial.m_materialProperties.specularPowerNs = 300.f;
		test3DComp->SetModel(model1);

		//////////////////////////////////////////////////////////////////////////
		//Wall #1
		testWall1 = std::make_unique<GameObject>();
		transform = new Transform(testWall1.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, -15.6f, 0.f, 1.f));
		transform->SetScale(DirectX::XMVectorSet(1.f, 30.f, 30.f, 1.f));
		testWall1->AddComponent(transform);
		gameObjSystem->AddComponent(transform);
		test3DComp = new ModelComponent(testWall1.get());
		testWall1->AddComponent(test3DComp);
		m_graphicsSystem->AddComponent(test3DComp);
		Model* model2 = graphicsSystem->GetModel("box");
		model2->m_meshEntryList[0].meshMaterial.SetToPresetMaterial(PredefinedMaterials::Emerald);
		model2->SetDiffTextureFileName("crate_1.jpg", 0);
		model2->SetNormalMapFileName("Standard_red_pxr256_normal.png", 0, false);
		model2->m_meshEntryList[0].meshMaterial.m_materialProperties.specularPowerNs = 300.f;
		test3DComp->SetModel(model2);

		//////////////////////////////////////////////////////////////////////////
		//Wall #2
		testWall2 = std::make_unique<GameObject>();
		transform = new Transform(testWall2.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, -15.6f, 0.f, 1.f));
		transform->SetScale(DirectX::XMVectorSet(30.f, 30.f, 1.f, 1.f));
		testWall2->AddComponent(transform);
		gameObjSystem->AddComponent(transform);
		test3DComp = new ModelComponent(testWall2.get());
		testWall2->AddComponent(test3DComp);
		m_graphicsSystem->AddComponent(test3DComp);
		Model* model3 = graphicsSystem->GetModel("box");
		model3->m_meshEntryList[0].meshMaterial.SetToPresetMaterial(PredefinedMaterials::Emerald);
		model3->SetDiffTextureFileName("crate_1.jpg", 0);
		model3->SetNormalMapFileName("Standard_red_pxr256_normal.png", 0, false);
		model3->m_meshEntryList[0].meshMaterial.m_materialProperties.specularPowerNs = 300.f;
		test3DComp->SetModel(model3);

		//enable skybox
		//Camera* const testCam = graphicsSystem->GetTestCamera();
		//testCam->SetIsUsingSkybox(true);
		//testCam->SetSkyboxTexture("Barce_Rooftop_C_3k");
		//
		////enable ibl!
		//m_graphicsSystem->SetIBLTexture("Barce_Rooftop_C_3k");
		//m_graphicsSystem->SetIsUsingIBL(true);
		//m_graphicsSystem->SetIBLsampleWeightCount(23);

		return true;
	}

	return false;
}

void TestAmbOcclussionApp::Shutdown(void)
{

}
