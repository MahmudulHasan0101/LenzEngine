#if 1
#include<Lenz.h>
#include<Impl/Uniforms/Skybox.h>
#include<Impl/Uniforms/Camera.h>
#include<Impl/Components/RendererComponents.h>

#include<random>


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


class ExampleLayer : public lenz::Layer
{
public:
	ExampleLayer(std::string name = "layer")
		//:colored_cubes(Cube()), colored_squares(Square()), sky(TexturedCubeShape())
		: colored_cubes(scene.CreateDrawcall<Cube, Component::Position, Component::Color>()),
		colored_squares(scene.CreateDrawcall<Square, Component::Position, Component::Color>())
	{
		//colored_cubes.AddAttributes< Component::Position, Component::Color>();
		//colored_squares.AddAttributes< Component::Position, Component::Color>();

		// Setting the sky:
		sky_shader.Init("Shaders/skybox.shader");
		coloredcube_shader.Init("Shaders/coloredcube.shader");

		texture = lenz::Cubemap({
			"Images/skybox/right.jpg",
			"Images/skybox/left.jpg",
			"Images/skybox/top.jpg",
			"Images/skybox/bottom.jpg",
			"Images/skybox/front.jpg",
			"Images/skybox/back.jpg"
			}, 6);
		texture.UploadToShader(sky_shader, "u_texture");
				
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis_position(-15.0, 15.0);
		std::uniform_real_distribution<float> dis_color(0.5, 1);

		for (size_t s = 0; s < 15; s++)
		{
			auto& entity = colored_cubes.CreateEntity();
			entity.GetComponent<Component::Position>().position = { dis_position(gen), dis_position(gen), dis_position(gen) };
			entity.GetComponent<Component::Color>().color = { dis_color(gen), dis_color(gen), dis_color(gen), 0.75f};
		}

		for (size_t s = 0; s < 15; s++)
		{
			auto& entity = colored_squares.CreateEntity();
			entity.GetComponent<Component::Position>().position = { dis_position(gen), dis_position(gen), dis_position(gen) };
			entity.GetComponent<Component::Color>().color = { dis_color(gen), dis_color(gen), dis_color(gen), 0.75f };
		}

		camera.Init("u_camera");
		camera.SetSpeed(5.0f);

		colored_cubes.Update<Component::Position, Component::Color>();
		colored_squares.Update<Component::Position, Component::Color>();

		glEnable(GL_BLEND);
		lenz::RendererCommands::EnableDepthTest();
	}


	bool OnEvent(lenz::Event& e) override
	{
		camera.OnEvent(e);
		lenz::Layer::OnEvent(e);

		return false;
	}

	bool OnKeyPressed(lenz::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case (GLFW_KEY_Q):
		{
			colored_cubes.drawcall->PrintAttributes<float>();
			break;
		}

		case (GLFW_KEY_P):
		{
			colored_squares.drawcall->PrintAttributes<float>();
			break;
		}
		}
		return false;
	}

	void OnUpdate(lenz::TimeStep& ts) override
	{
		camera.OnUpdate(ts);

		camera.UploadOrientationMatrixTo({ &sky_shader });
		camera.UploadTo({ &coloredcube_shader });
		
		lenz::RendererCommands::Clear();

		colored_squares.drawcall->Run(coloredcube_shader);
		colored_cubes.drawcall->Run(coloredcube_shader);
	}

private:
	lenz::Camera camera;
	lenz::Shader sky_shader;
	lenz::Shader coloredcube_shader;
	lenz::Cubemap texture;

	lenz::Scene scene;

	lenz::SceneDrawcall<Cube, Component::Position, Component::Color> colored_cubes;
	lenz::SceneDrawcall<Square, Component::Position, Component::Color> colored_squares;

	//lenz::DrawCall colored_cubes;
	//lenz::DrawCall colored_squares;
	//lenz::DrawCall sky;
	//lenz::DrawCall<Cube, Component::Position, Component::Color> colored_cubes;
	//lenz::DrawCall<Square, Component::Position, Component::Color> colored_squares;
	//lenz::DrawCall<TexturedCubeShape> sky;

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


//
//
//#include<iostream>
//#include<typeinfo>
//
//template<typename T>
//void print()
//{
//	std::cout << typeid(T).name() << "\n";
//}
//
//int main()
//{
//	std::type_info const& type = typeid(float);
//	print<decltype(type)>();
//}
