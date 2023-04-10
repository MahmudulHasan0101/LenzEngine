#pragma once
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"
#include"glm/gtx/rotate_vector.hpp"
#include"glm/gtx/vector_angle.hpp"

#include"Lenz/Event/EventAdaptor.h"
#include"Lenz/Utils/TimeStep.h"
#include"Lenz/Renderer/Uniform.h"
#include"Lenz/Renderer/Shader.h"

namespace lenz
{
	class Camera : public EventAdaptor, public UniformMat4
	{
	public:
		void Matrix();
		void MatrixNoPosition();
		void MatrixNoOrientation();

		void SetSpeed(float speed);
		void SetSensitivity(float sensitivity);

		void MoveForward();
		void MoveBack();
		void MoveRight();
		void MoveLeft();
		void MoveUp();
		void MoveDown();
		
		void RotateX(float rad);
		void RotateY(float rad);

		virtual bool OnEvent(Event& e);

		virtual bool OnKeyPressed(KeyPressedEvent& e) override;
		virtual bool OnKeyReleased(KeyReleasedEvent& e) override;
		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& e) override;
		virtual bool OnMouseButtonReleased(MouseButtonReleasedEvent& e) override;
		virtual bool OnMouseMoved(MouseMovedEvent& e) override;

		void StoreProjectionMatrix(Shader& shader);
		void OnUpdate(TimeStep& ts);

		void UploadTo(std::initializer_list<Shader*> shaders);
		void UploadOrientationMatrixTo(std::initializer_list<Shader*> shader);
		void UploadPositionMatrixTo(std::initializer_list<Shader*> shader);

		Camera();
		void Init(const char* name, glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f }, float FOVdeg = 60.0f, float nearPlane = 0.01f, float farPlane = 100.0f);

	public:
		glm::vec3 Position;
		glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	protected:
		void UploadTo(Shader& shader) override {};

		float FOVdeg = 0;
		float NearPlane = 0;
		float FarPlane = 0;

		float speed = 1.0f;
		float sensitivity = 70.0f;

	private:
		bool firstClick = true;
		bool mouseClicked = false;
	};
}