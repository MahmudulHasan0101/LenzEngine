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
	class Camera : public EventAdaptor
	{
	public:
		glm::mat4 Matrix(unsigned int width, unsigned int height);
		glm::mat4 Matrix() const;
		glm::mat4 MatrixNoPosition() const;
		glm::mat4 MatrixNoOrientation() const;
		glm::mat4 Projection() const;
		glm::mat4 View() const;

		inline glm::mat4 getLastView() const { return lastView; }
		inline glm::mat4 getLastProjection() const { return lastProjection; }

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

		virtual void OnUpdate(TimeStep& ts);

		void UploadProjectionTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadViewTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadInverseTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadOrientationMatrixTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadPositionMatrixTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadPositionTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadOrientationTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;
		void UploadVelocityTo(std::initializer_list<Shader*> shader, const std::string& uniformname) const;

		Camera();
		void Init(glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f }, float FOVdeg = 80.0f, float nearPlane = 0.01f, float farPlane = 10000.0f);

	public:
		glm::vec3 Position;
		glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

		float FOVdeg = 0;
		float NearPlane = 0;
		float FarPlane = 0;

		float speed = 1.0f;
		float sensitivity = 70.0f;

		glm::mat4 lastView;
		glm::mat4 lastProjection;

	private:
		bool firstClick = true;
		bool mouseClicked = false;
	};
}