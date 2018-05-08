#include <Utilities/precompiled.h>
#include <Engine/Applications/TestApp/TestApp.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/GraphicsSystem.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/LightComponent/LightComponent.h>
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
		testObj->AddComponent(new Transform(testObj.get()));
		ModelComponent* test3DComp = new ModelComponent(testObj.get());
		auto graphicsSystem = static_cast<GraphicsSystem*>(GetSystem(SystemType::ST_GRAPHICS));

		//test3DComp->SetModel(loadedModels.at("dragon.obj").get());
		//test3DComp->SetModel(loadedModels.at("bunny.obj").get());
		//test3DComp->SetModel(loadedModels.at("tiny_4anim.x").get());
		//test3DComp->SetModel(graphicsSystem->GetModel("gh_sample_animation.fbx"));
		//auto model = graphicsSystem->GetModel("boblampclean.md5mesh");
		auto model = graphicsSystem->GetModel("bunny.obj");
		//auto model = graphicsSystem->GetModel("boblampclean.md5mesh");
		model->SetDiffTextureFileName("iron_grill.tga", 0);
		test3DComp->SetModel(model);

		testObj->AddComponent(test3DComp);
		graphicsSystem->AddComponent(test3DComp);

		//auto* testPathComp = new CurvePathComponent(testObj.get());
		//testPathComp->GenerateVertexBuffer(m_graphicsSystem->m_dx11Renderer.get());
		//testObj->AddComponent(testPathComp);
		//m_graphicsSystem->AddComponent(testPathComp);

		//auto* testSimpleCloth = new SimpleClothComponent(testObj.get(), 12, 12, 33, 33);
		//testObj->AddComponent(testSimpleCloth);
		//GetSystem(SystemType::ST_GRAPHICS)->AddComponent(testSimpleCloth);
		//testSimpleCloth->generateVertexBuffers(graphicsSystem->GetRenderer());

		testLight = std::make_unique<GameObject>();
		auto transform = new Transform(testLight.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, 3.f, 0.f, 1.f));
		testLight->AddComponent(transform);

		auto lightComp = new LightComponent(testLight.get());
		//lightComp->GetLight()->m_lightType = LightType::LT_DIRECTIONAL;
		testLight->AddComponent(lightComp);
		graphicsSystem->AddComponent(lightComp);


		//////////////////////////////////////////////////////////////////////////
		//floor game object
		testFloor = std::make_unique<GameObject>();
		transform = new Transform(testFloor.get());
		transform->SetPositionn(DirectX::XMVectorSet(0.f, -2.6f, 0.f, 1.f));
		transform->SetScale(DirectX::XMVectorSet(14.f, 1.f, 14.f, 1.f));
		testFloor->AddComponent(transform);
		test3DComp = new ModelComponent(testFloor.get());
		testFloor->AddComponent(test3DComp);
		m_graphicsSystem->AddComponent(test3DComp);
		model = graphicsSystem->GetModel("box.obj");
		model->SetDiffTextureFileName("iron_grill.tga", 0);
		test3DComp->SetModel(model);


		return true;
	}

	return false;
}

void TestApp::Shutdown(void)
{
	IApplication::Shutdown();
}