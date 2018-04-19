#include <Utilities/precompiled.h>
#include <Engine/Applications/TestApp/TestApp.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/GraphicsSystem.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>
#include <Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>

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
		const auto& loadedModels = graphicsSystem->GetLoadedModels();

		//test3DComp->SetModel(loadedModels.at("dragon.obj").get());
		//test3DComp->SetModel(loadedModels.at("bunny.obj").get());
		test3DComp->SetModel(loadedModels.at("cylinder_skellmesh.fbx").get());
		//test3DComp->SetModel(loadedModels.at("tiny_4anim.x").get());
		//test3DComp->SetModel(loadedModels.at("boblampclean.md5mesh").get());

		testObj->AddComponent(test3DComp);
		graphicsSystem->AddComponent(test3DComp);

		//auto* testPathComp = new CurvePathComponent(testObj.get());
		//testPathComp->GenerateVertexBuffer(m_graphicsSystem->m_dx11Renderer.get());
		//testObj->AddComponent(testPathComp);
		//m_graphicsSystem->AddComponent(testPathComp);

		auto* testSimpleCloth = new SimpleClothComponent(testObj.get(), 12, 12, 33, 33);
		testObj->AddComponent(testSimpleCloth);
		GetSystem(SystemType::ST_GRAPHICS)->AddComponent(testSimpleCloth);
		testSimpleCloth->generateVertexBuffers(graphicsSystem->GetRenderer());

		return true;
	}

	return false;
}

void TestApp::Shutdown(void)
{
	IApplication::Shutdown();
}