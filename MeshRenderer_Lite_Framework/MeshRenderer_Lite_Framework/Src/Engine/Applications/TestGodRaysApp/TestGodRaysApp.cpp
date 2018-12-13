#include <Utilities/precompiled.h>
#include <Engine/Applications/TestGodRaysApp/TestGodRaysApp.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Core/GameObjectSystem.h>
#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/GraphicsSystem.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>

TestGodRaysApp::TestGodRaysApp()
	:IApplication()
{

}

TestGodRaysApp::~TestGodRaysApp()
{

}

bool TestGodRaysApp::Initialize(HINSTANCE hInstance)
{
	if(IApplication::Initialize(hInstance))
	{
		auto graphicsSystem = static_cast<GraphicsSystem*>(GetSystem(SystemType::ST_GRAPHICS));
		auto gameObjSystem = static_cast<GameObjectSystem*>(GetSystem(SystemType::ST_GAME_OBJECT_SYSTEM));

		Transform* transform;
		ModelComponent* test3DComp;
		Model* model = graphicsSystem->GetModel("box");

		//test light 1
		testLight = std::make_unique<GameObject>();
		transform = new Transform(testLight.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, 0.f, 100.f, 1.f));
		transform->SetRotationX(180.2f);
		testLight->AddComponent(transform);
		gameObjSystem->AddComponent(transform);

		ShadowLightComponent* lightComp = new ShadowLightComponent(testLight.get(), true, true, false);
		testLight->AddComponent(lightComp);
		graphicsSystem->AddComponent(lightComp);
		lightComp->GetLight()->isActive = 1;

		//////////////////////////////////////////////////////////////////////////
		// Make the grid of boxes
		float horizontalStart = -10.0f;
		float horirontalEnd = 10.0f;

		for (int i = 0; i < BOX_GRID_DIM; ++i)
		{
			testHorizontalBoxes[i] = std::make_unique<GameObject>();

			//transform data
			transform = new Transform(testLight.get());
			transform->SetPositionn(DirectX::XMVectorSet(Lerp(horizontalStart, horirontalEnd, float(i) / float(BOX_GRID_DIM)), 0, 0.f, 1.f));
			transform->SetScale(3, 22, 3);

			testHorizontalBoxes[i]->AddComponent(transform);
			gameObjSystem->AddComponent(transform);

			//Model comp
			ModelComponent* test3DComp = new ModelComponent(testHorizontalBoxes[i].get());
			model->m_meshEntryList[0].meshMaterial = MeshEntryMaterial::GetPresetMaterial(PredefinedMaterials::GreenPlastic);
			test3DComp->SetModel(model);
			test3DComp->SetIsActive(true);

			testHorizontalBoxes[i]->AddComponent(test3DComp);
			graphicsSystem->AddComponent(test3DComp);
		}

		for (int i = 0; i < BOX_GRID_DIM; ++i)
		{
			testVerticalBoxes[i] = std::make_unique<GameObject>();

			//transform data
			transform = new Transform(testLight.get());
			transform->SetPositionn(DirectX::XMVectorSet(0, Lerp(horizontalStart, horirontalEnd, float(i) / float(BOX_GRID_DIM)), 0.f, 1.f));
			transform->SetScale(22, 3, 3);

			testVerticalBoxes[i]->AddComponent(transform);
			gameObjSystem->AddComponent(transform);

			//Model comp
			ModelComponent* test3DComp = new ModelComponent(testVerticalBoxes[i].get());
			model->m_meshEntryList[0].meshMaterial = MeshEntryMaterial::GetPresetMaterial(PredefinedMaterials::GreenPlastic);
			test3DComp->SetModel(model);
			test3DComp->SetIsActive(true);

			testVerticalBoxes[i]->AddComponent(test3DComp);
			graphicsSystem->AddComponent(test3DComp);
		}

		//////////////////////////////////////////////////////////////////////////
		//floor game object
		//testFloor = std::make_unique<GameObject>();
		//transform = new Transform(testFloor.get());
		//transform->SetPositionn(DirectX::XMVectorSet(0.f, -15.6f, 0.f, 1.f));
		//transform->SetScale(DirectX::XMVectorSet(200.f, 1.f, 200.f, 1.f));
		//testFloor->AddComponent(transform);
		//gameObjSystem->AddComponent(transform);
		//test3DComp = new ModelComponent(testFloor.get());
		//testFloor->AddComponent(test3DComp);
		//m_graphicsSystem->AddComponent(test3DComp);
		//auto model2 = graphicsSystem->GetModel("box");
		//model2->m_meshEntryList[0].meshMaterial.SetToPresetMaterial(PredefinedMaterials::Emerald);
		//model2->SetDiffTextureFileName("F_512.tga", 0);
		//model2->SetNormalMapFileName("Standard_red_pxr256_normal.png", 0, false);
		//model2->m_meshEntryList[0].meshMaterial.m_materialProperties.specularPowerNs = 300.f;
		//test3DComp->SetModel(model2);

		return true;
	}

	return false;
}

void TestGodRaysApp::Shutdown(void)
{
}
