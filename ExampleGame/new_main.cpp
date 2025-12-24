#if 1

#include<Lenz.h>
#include<Lenz/Renderer/Cubemap.h>
#include<Lenz/Renderer/Camera.h>
#include<Lenz/Renderer/DeferredShading.h>
#include<Lenz/Scene/Component.h>
#include"BasicShapes.h"

#include<random>

#define GetInstanceOffset(entity, instanceclass, instance) reinterpret_cast<void*>(reinterpret_cast<char*>(entity._DataPtr) + offsetof(instanceclass, instance))

static float RandFloat(float min, float max)
{
	static std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}


struct CubeData
{
	RendererComponent::Transformation transformation;
	RendererComponent::Color color {0.94, 0.42, 0.12, 1.0};
};


class ExampleLayer : public lenz::Layer
{
public:
	ExampleLayer(std::string name = "layer")
		: cubes(scene.CreateDrawUnite<Cube, CubeData>(Cube()))
	{
		lenz::TimeStep::SleepMilliseconds(200);
		cube_shader.Init("Shaders/coloredcube.shader");

		cubes.Reserve(1000);

		lenz::UnitAttributeStorageManager& manager = cubes.GetAttributeStorageManager();

		for (int i = 0; i < 50; i++)
		{
			CubeData data;

			data.color = {
				RandFloat(0.0f, 1.0f),
				RandFloat(0.0f, 1.0f),
				RandFloat(0.0f, 1.0f),
				1.0f
			};

			lenz::Entity e = cubes.AddEntity(data);
			cubeEntities.push_back(e);

			lenz::PhysicsComponent* pcomp = 
				manager.SetAttributeMove<lenz::PhysicsComponent>(e, std::move(lenz::PhysicsComponent(GetInstanceOffset(e, CubeData, transformation))));
			
			physicsSystem1.AddEntity(*pcomp);

			pcomp->setColliderBox(
				{
				RandFloat(-50.0f, 50.0f),
				RandFloat(0.0f, 20.0f),
				RandFloat(-50.0f, 50.0f)
				},
				{ 0,0,0 }, { 1,1,1 }, 10
			);

			pcomp->setStatus(true);
			pcomp->body->setActivationState(DISABLE_DEACTIVATION);
		}

		camera.SetSpeed(40.0f);
		lenz::RendererCommands::EnableDepthTest();
	}


	~ExampleLayer()
	{
	}

	bool OnEvent(lenz::Event& e) override
	{
		camera.OnEvent(e);
		lenz::Layer::OnEvent(e);

		return false;
	}

	void PhysicsHander(lenz::TimeStep& ts)
	{

	}

	void OnUpdate(lenz::TimeStep& ts) override
	{
		for (lenz::Entity& entity : cubeEntities)
		{
			lenz::UnitAttributeStorageManager& manager = cubes.GetAttributeStorageManager();
			lenz::PhysicsComponent* comp = manager.GetAttribute<lenz::PhysicsComponent>(entity);
			comp->applyImpulse({ RandFloat(-10.0f, 10.0f),RandFloat(-10.0f, 10.0f),RandFloat(-10.0f, 10.0f) });
			comp->setStatus(true);
		}

		physicsSystem1.Update(ts.GetDeltaSecond());

		lenz::RendererCommands::Clear();

		camera.OnUpdate(ts);
		camera.UploadTo({ &cube_shader }, "u_Camera");
		cubes.Draw(cube_shader);
	}

private:
	lenz::Camera camera;
	lenz::Shader cube_shader;

	std::vector<lenz::Entity> cubeEntities;
	lenz::PhysicsSystem physicsSystem1;
	lenz::MappedMotionState* mappedMotionState;

	lenz::Scene scene;
	lenz::DrawUnit<CubeData> cubes;
};




class Simulator : public lenz::App
{
public:
	Simulator()
		:App()
	{
		PushLayer(new ExampleLayer());
	}
};

lenz::Window* GetAppWindow()
{
	return new lenz::Window();
}
lenz::Camera* GetAppCamera()
{
	return new lenz::Camera();
}

lenz::App* CreateApp()
{
	return new Simulator();
}
#endif

