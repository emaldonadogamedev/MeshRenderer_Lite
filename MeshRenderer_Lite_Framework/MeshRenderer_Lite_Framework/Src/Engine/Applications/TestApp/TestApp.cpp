#include <Utilities/precompiled.h>
#include <Engine/Applications/TestApp/TestApp.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Core/GameObjectSystem.h>
#include <Systems/Graphics/GraphicsSystem.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>
#include <Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>

TestApp::TestApp()
	:IApplication()
{
}

TestApp::~TestApp()
{
}

bool TestApp::Initialize(HINSTANCE hInstance)
{
	if (IApplication::Initialize(hInstance)) {
		testObj = std::make_unique<GameObject>();
		auto transform = new Transform(testObj.get());
		testObj->AddComponent(transform);
		transform->SetScale(12, 12, 12);
		ModelComponent* test3DComp = new ModelComponent(testObj.get());
		auto graphicsSystem = static_cast<GraphicsSystem*>(GetSystem(SystemType::ST_GRAPHICS));
		auto gameObjSystem = static_cast<GameObjectSystem*>(GetSystem(SystemType::ST_GAME_OBJECT_SYSTEM));
		
		//test3DComp->SetModel(loadedModels.at("dragon.obj").get());
		//test3DComp->SetModel(loadedModels.at("tiny_4anim.x").get());
		//test3DComp->SetModel(graphicsSystem->GetModel("gh_sample_animation.fbx"));
		//auto model = graphicsSystem->GetModel("bottle.obj");
		//auto model = graphicsSystem->GetModel("gh_sample_animation.fbx");
		//auto model = graphicsSystem->GetModel("walk.fbx");
		//auto model = graphicsSystem->GetModel("dragon.obj");
		//auto model = graphicsSystem->GetModel("box");
		auto model = graphicsSystem->GetModel("sphere");
		//auto model = graphicsSystem->GetModel("bottle.obj");
		//model->SetDiffTextureFileName("AlphaBlendTest.png", 0);
		model->m_meshEntryList[0].meshMaterial = MeshEntryMaterial::GetPresetMaterial(PredefinedMaterials::GreenPlastic);
		test3DComp->SetModel(model);
		test3DComp->SetIsActive(true);

		testObj->AddComponent(test3DComp);
		graphicsSystem->AddComponent(test3DComp);
		gameObjSystem->AddComponent(testObj->GetComponent(ComponentType::TRANSFORM));

		//auto* testPathComp = new CurvePathComponent(testObj.get());
		//testPathComp->GenerateVertexBuffer(m_graphicsSystem->m_dx11Renderer.get());
		//testObj->AddComponent(testPathComp);
		//m_graphicsSystem->AddComponent(testPathComp);

		//auto* testSimpleCloth = new SimpleClothComponent(testObj.get(), 12, 12, 33, 33);
		//testObj->AddComponent(testSimpleCloth);
		//GetSystem(SystemType::ST_GRAPHICS)->AddComponent(testSimpleCloth);
		//testSimpleCloth->generatesVertexBuffers(graphicsSystem->GetRenderer());


		//test light 1
		testLight = std::make_unique<GameObject>();
		transform = new Transform(testLight.get());
		transform->SetPositionn(DirectX::XMVectorSet(2.8f, 48.f, 0.f, 1.f));
		testLight->AddComponent(transform);
		gameObjSystem->AddComponent(transform);

		ShadowLightComponent* lightComp = new ShadowLightComponent(testLight.get(), true, true);
		//lightComp->GetLight()->m_lightType = LightType::LT_DIRECTIONAL;
		testLight->AddComponent(lightComp);
		graphicsSystem->AddComponent(lightComp);
		lightComp->GetLight()->isActive = 1;

		//test light 2
		//testLight2 = std::make_unique<GameObject>();
		//transform = new Transform(testLight2.get());
		//transform->SetPositionn(DirectX::XMVectorSet(-2.8f, 90.0f, 0.f, 1.f));
		//testLight2->AddComponent(transform);
		//gameObjSystem->AddComponent(transform);
		//
		//ShadowLightComponent* lightComp2 = new ShadowLightComponent(testLight2.get(), true, true);
		////lightComp->GetLight()->m_lightType = LightType::LT_DIRECTIONAL;
		//testLight2->AddComponent(lightComp2);
		//graphicsSystem->AddComponent(lightComp2);


		//////////////////////////////////////////////////////////////////////////
		//floor game object
		testFloor = std::make_unique<GameObject>();
		transform = new Transform(testFloor.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, -15.6f, 0.f, 1.f));
		transform->SetScale(DirectX::XMVectorSet(50.f, 1.f, 50.f, 1.f));
		testFloor->AddComponent(transform);
		gameObjSystem->AddComponent(transform);
		test3DComp = new ModelComponent(testFloor.get());
		testFloor->AddComponent(test3DComp);
		m_graphicsSystem->AddComponent(test3DComp);
		auto model2 = graphicsSystem->GetModel("box");
		model2->m_meshEntryList[0].meshMaterial.SetToPresetMaterial(PredefinedMaterials::Emerald);
		model2->SetDiffTextureFileName("crate_1.jpg", 0);
		model2->SetNormalMapFileName("Standard_red_pxr256_normal.png", 0);
		model2->m_meshEntryList[0].meshMaterial.m_materialProperties.specularPowerNs = 300.f;
		test3DComp->SetModel(model2);

		//Many mini lights!
		for (int i = 0; i < MINI_LIGHT_COUNT; i++)
		{
				testMiniLights[i] = std::make_unique<GameObject>();
		
				auto testMiniLightObj = testMiniLights[i].get();
		
				transform = new Transform(testMiniLights[i].get());
				transform->SetPositionn(DirectX::XMVectorSet(RandFloat(-50.f, 50.f), 1.0f, RandFloat(-50.f, 50.f), 1.f));
				gameObjSystem->AddComponent(transform);
				testMiniLightObj->AddComponent(transform);
		
				auto simpleLightComp = new LightComponent(testMiniLights[i].get());
				float range = RandFloat(10, 15);
				simpleLightComp->SetLightRange(range);
				auto light = simpleLightComp->GetLight();
				light->m_Iambient = XMFLOAT3(RandFloat(0.2f, 0.4f), RandFloat(0.2f, 0.4f), RandFloat(0.2f, 0.4f));
				light->m_Idiffuse = XMFLOAT3(RandFloat(10.f, 70.f), RandFloat(10.f, 70.f), RandFloat(10.f, 70.f));
				testMiniLightObj->AddComponent(simpleLightComp);
				graphicsSystem->AddComponent(simpleLightComp);
		}

		return true;
	}

	return false;
}

void TestApp::Shutdown(void)
{
	IApplication::Shutdown();
}