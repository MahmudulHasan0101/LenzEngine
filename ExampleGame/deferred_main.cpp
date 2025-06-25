#if 0

#include<Lenz.h>
#include<Lenz/Renderer/Cubemap.h>
#include<Lenz/Renderer/Camera.h>
#include<Lenz/Renderer/DeferredShading.h>
#include<Lenz/Scene/Component.h>
#include"Model.h"
#include"BasicShapes.h"

#include<random>


struct Light {
	glm::vec3 position;
	glm::vec3 color;
};


class PlaneShape : public lenz::Shape
{
public:
	static lenz::Texture* texture;

	PlaneShape()
		: lenz::Shape(std::move(*(Shape*)&lenz::Model("./Models/hheli.obj").meshes[0]))
	{
		if (textures.size() != 0)
			texture = textures[0].get();
	}
};

lenz::Texture* PlaneShape::texture = nullptr;


class ExampleLayer : public lenz::Layer
{
public:
	ExampleLayer(std::string name = "layer")
		: 
		planes(&scene, PlaneShape()),
		square(&scene, Square()),
		sky(&scene, TexturedCubeShape()),
		lights(&scene, Cube())
	{

		sky_shader.Init("Shaders/deferred/skybox.shader");
		texture_shader.Init("Shaders/deferred/texture.shader");
		lighting_shader.Init("Shaders/deferred/lighting.shader");
		lightcube_shader.Init("Shaders/coloredcube.shader");


		sky_cubemap = lenz::Cubemap({
			"Images/skybox/right.jpg",
			"Images/skybox/left.jpg",
			"Images/skybox/top.jpg",
			"Images/skybox/bottom.jpg",
			"Images/skybox/front.jpg",
			"Images/skybox/back.jpg"
			}, 0);

		sky_cubemap.BindToSlot(5);
		sky_cubemap.UploadToShader(sky_shader, "u_texture");

		std::vector<Light> vec_lights;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> posDist(-100.0f, 100.0f);
		std::uniform_real_distribution<float> colDist(0.5f, 1.0f);
		std::uniform_real_distribution<float> intensityDist(0.5f, 2.0f);

		for (int i = 0; i < 5; i++) {
			Light light;
			auto position = glm::vec3(posDist(gen), posDist(gen), posDist(gen));
			auto color = glm::vec3(colDist(gen), colDist(gen), colDist(gen));
			light.position = position;
			light.color = color;
			vec_lights.push_back(light);

			auto& entity = lights.CreateEntity();
			entity.GetComponent<RendererComponent::Position>().position = position;
			entity.GetComponent<RendererComponent::Color>().color = glm::vec4(color, 1.0);
		}

		for (int i = 0; i < vec_lights.size(); i++) {
			std::string base = "lights[" + std::to_string(i) + "]";
			lighting_shader.UploadUniformVec3f(base + ".position", vec_lights[i].position);
			lighting_shader.UploadUniformVec3f(base + ".color", vec_lights[i].color);
		}


		deferredTextures.Init(lenz::App::GetWindow().GetWidth(), lenz::App::GetWindow().GetHeight());

		auto& entity = planes.CreateEntity();
		physicsSystem1.AddEntity(entity);
		entity.GetComponent<lenz::PhysicsSpecification>().setColliderBox({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 10);


		square.CreateEntity();
		sky.CreateEntity();

		sky.UpdateAll();
		square.UpdateAll();
		planes.UpdateAll();
		lights.UpdateAll();

		camera.SetSpeed(40.0f);
		lenz::RendererCommands::EnableDepthTest();
	}

	~ExampleLayer()
	{
		sky_cubemap.Delete();
		PlaneShape::texture->Delete();
	}

	bool OnEvent(lenz::Event& e) override
	{
		camera.OnEvent(e);
		lenz::Layer::OnEvent(e);

		return false;
	}

	void PhysicsHander(lenz::TimeStep& ts)
	{
		auto view = planes.GetView<RendererComponent::Transformation, lenz::PhysicsSpecification>();
		for (entt::entity entity : view)
		{
			view.get<lenz::PhysicsSpecification>()
			entity.getWorldTransformation();
		}

	}


	void OnUpdate(lenz::TimeStep& ts) override
	{
		
		
		physicsSystem1.Update(ts.GetDeltaSecond());
		planes.InvalidateWholeAttributeBuffer_ChacheMethod();




		lenz::RendererCommands::Clear();

		camera.OnUpdate(ts);

		camera.UploadOrientationMatrixTo({ &sky_shader, &lightcube_shader }, "u_camera");
		camera.UploadTo({ &texture_shader }, "u_camera");
		camera.UploadPositionTo({ &lighting_shader }, "u_cameraPos");

		// Bind deferred FBO
		deferredTextures.Bind();
		lenz::RendererCommands::Clear();

		// Render scene to deferred FBO
		PlaneShape::texture->BindToSlot(7);
		PlaneShape::texture->UploadToShader(texture_shader, "u_texture");
		//planes.Run(texture_shader);

		// Ensure we only bind the FBO if needed
		GLint currentFBO;
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentFBO);
		if (currentFBO != deferredTextures.getID()) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredTextures.getID());
		}

		// Unbind deferred FBO properly
		deferredTextures.Unbind();

		lenz::RendererCommands::Clear();

		// Bind G-buffer textures for lighting pass
		deferredTextures.BindTextures(0, 1, 2);
		lighting_shader.UploadUniformInt("gPosition", 0);
		lighting_shader.UploadUniformInt("gNormal", 1);
		lighting_shader.UploadUniformInt("gAlbedoSpec", 2);

		// Perform lighting pass
		square.Draw(lighting_shader);

		// Render lights
		lights.Draw(lightcube_shader);

	}

private:
	lenz::Camera camera;
	lenz::Shader sky_shader;
	lenz::Shader texture_shader;
	lenz::Shader lighting_shader;
	lenz::Shader lightcube_shader;
	lenz::Cubemap sky_cubemap;

	lenz::PhysicsSystem physicsSystem1;

	lenz::Scene scene;
	lenz::DeferredFramebuffer deferredTextures;

	lenz::SceneDrawcall<TexturedCubeShape, RendererComponent::TexturePosition> sky;
	lenz::SceneDrawcall<Cube, RendererComponent::Position, RendererComponent::Color> lights;
	lenz::SceneDrawcall<PlaneShape, RendererComponent::Normal, RendererComponent::TextureSlot, RendererComponent::Transformation> planes;
	lenz::SceneDrawcall<Square, int> square;

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

