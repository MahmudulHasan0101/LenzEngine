#if 0
#include<Lenz.h>
#include<Impl/Uniforms/Skybox.h>
#include<Impl/Uniforms/Camera.h>
#include<Impl/Components/RendererComponents.h>


class TexturedSquare : public lenz::Entity
{
public:
	TexturedSquare()
	{
	}

	TexturedSquare(lenz::Scene* scene)
	{
		Init<Shape3D::TexturedCubeShape>(scene);
		AddComponent<RenderComponent::Position>(0.1f, 0.2f, 0.3f);
		AddComponent<RenderComponent::TextureSlot>(6);
	}
};



class ExampleLayer : public lenz::Layer
{
public:
	ExampleLayer(std::string name = "layer")
	{
		shader.Init("Shaders/texturecube.shader");
		texture = lenz::Cubemap({
			"Images/skybox/right.jpg",
			"Images/skybox/left.jpg",
			"Images/skybox/top.jpg",
			"Images/skybox/bottom.jpg",
			"Images/skybox/front.jpg",
			"Images/skybox/back.jpg"
			}, 6);

		texture.UploadToShader(shader, "u_texture");

		auto& squares = scene.CreateDrawCall<Shape3D::TexturedCubeShape>();
		squares.AddAttributes<RenderComponent::Position, RenderComponent::TextureSlot>();
		auto sq = TexturedSquare(&scene);
		squares.Update<RenderComponent::Position, RenderComponent::TextureSlot>();

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
		camera.UploadTo(shader);
		//scene.Update();
		scene.Draw(shader);
	}

private:
	lenz::Camera camera;
	lenz::Scene scene;
	lenz::Shader shader;
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