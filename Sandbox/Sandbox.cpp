#if 1
#include<Lenz.h>
#include<Impl/Uniforms/Skybox.h>
#include<Impl/Uniforms/Camera.h>
#include<Impl/Components/RendererComponents.h>



class FPSUpdater
{
public:
	FPSUpdater(std::function<void()> fn, float FPS)
		:Fn(fn), fps(FPS), delta(0), tick(1 / fps)
	{
	}

	void Run(lenz::TimeStep& ts)
	{
		if (delta >= tick)
		{
			Fn();
			delta = 0.0f;
		}
		delta += ts;
	}


	std::function<void()> Fn;

	float fps,
		delta,
		tick;
};




class Sky : public lenz::Entity
{
public:
	Sky(lenz::Scene* scene)
	{
		Init<Shape3D::TexturedCubeShape>(scene);
	}
};



class ExampleLayer : public lenz::Layer
{
public:
	ExampleLayer(std::string name = "layer")
	{
		// Setting the sky:
		sky_shader.Init("Shaders/skybox.shader");
		texture = lenz::Cubemap({
			"Images/skybox/right.jpg",
			"Images/skybox/left.jpg",
			"Images/skybox/top.jpg",
			"Images/skybox/bottom.jpg",
			"Images/skybox/front.jpg",
			"Images/skybox/back.jpg"
			}, 6);
		texture.UploadToShader(sky_shader, "u_texture");


		scene.CreateDrawCall<Shape3D::TexturedCubeShape>().Compile();
		auto sky = Sky(&scene);

		camera.Init("u_camera");
		camera.SetSpeed(5.0f);

		lenz::RendererCommands::EnableDepthTest();
	}


	bool OnEvent(lenz::Event& e) override
	{
		camera.OnEvent(e);

		return false;
	}
	void OnUpdate(lenz::TimeStep& ts) override
	{
		lenz::RendererCommands::Clear();
		camera.OnUpdate(ts);
		camera.UpdoadOrientationMatrixTo({ &sky_shader });

		lenz::RendererCommands::DepthTestType(lenz::DepthTestTypes::LEQUAL);
		scene.Draw(sky_shader);
		lenz::RendererCommands::DepthTestType(lenz::DepthTestTypes::LESS);
	}

private:
	lenz::Camera camera;
	lenz::Scene scene;
	lenz::Shader sky_shader;
	lenz::Cubemap texture;

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