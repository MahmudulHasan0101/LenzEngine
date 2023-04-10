#include"Lenz/App.h"
#include"Camera.h"


namespace lenz
{
	Camera::Camera()
	{
	}

	void Camera::Init(const char* name, glm::vec3 position, float fovdeg, float nearPlane, float farPlane)
	{
		ChangeName(name);
		Position = position;
		FOVdeg = fovdeg;
		NearPlane = nearPlane;
		FarPlane = farPlane;
	}

	void Camera::Matrix()
	{
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		unsigned int width = lenz::App::GetWindow().GetWidth();
		unsigned int height = lenz::App::GetWindow().GetHeight();

		view = glm::lookAt(Position, Position + Orientation, Up);
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, NearPlane, FarPlane);

		Data = projection * view;
	}

	void Camera::MatrixNoOrientation()
	{
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		unsigned int width = lenz::App::GetWindow().GetWidth();
		unsigned int height = lenz::App::GetWindow().GetHeight();

		view = glm::lookAt(Position, glm::vec3(0, 0, 0), Up);
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, NearPlane, FarPlane);

		Data = projection * view;
	}

	void Camera::MatrixNoPosition()
	{
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		unsigned int width = lenz::App::GetWindow().GetWidth();
		unsigned int height = lenz::App::GetWindow().GetHeight();

		view = glm::lookAt(glm::vec3(0, 0, 0), Orientation, Up);
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, NearPlane, FarPlane);

		Data = projection * view;
	}

	void Camera::UploadTo(std::initializer_list<Shader*> shaders)
	{
		Matrix();

		for (size_t s = 0; s < shaders.size(); s++)
			UniformMat4::UploadTo(std::ref(*shaders.begin()[s]));
	}
	void Camera::UploadOrientationMatrixTo(std::initializer_list<Shader*> shaders)
	{
		MatrixNoPosition();

		for (size_t s = 0; s < shaders.size(); s++)
			UniformMat4::UploadTo(std::ref(*shaders.begin()[s]));
	}

	void Camera::UploadPositionMatrixTo(std::initializer_list<Shader*> shaders)
	{
		MatrixNoOrientation();

		for (size_t s = 0; s < shaders.size(); s++)
			UniformMat4::UploadTo(std::ref(*shaders.begin()[s]));
	}

	//void Camera::StoreProjectionMatrix(Shader& shader)
	//{
	//	shader.UploadUniformMat4(m_ProjectionMatrix, Matrix());
	//}

	void Camera::OnUpdate(TimeStep& ts)
	{
		Position += Velocity * ts.GetDeltaSecond();
	}


	void Camera::SetSpeed(float speed)
	{
		this->speed = speed;
	}
	void Camera::SetSensitivity(float sensitivity)
	{
		this->sensitivity = sensitivity;
	}

	void Camera::MoveForward()
	{
		Velocity = speed * Orientation;
	}
	void Camera::MoveBack()
	{
		Velocity = speed * -Up;

	}
	void Camera::MoveRight()
	{
		Velocity = speed * glm::normalize(glm::cross(Orientation, Up));
	}
	void Camera::MoveLeft()
	{
		Velocity = speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	void Camera::MoveUp()
	{
		Velocity = speed * Up;
	}
	void Camera::MoveDown()
	{
		Velocity = speed * -Up;
	}

	void Camera::RotateX(float rad)
	{
		Orientation = glm::rotate(Orientation, glm::radians(rad), Up);
	}

	void Camera::RotateY(float rad)
	{
		Orientation = glm::rotate(Orientation, glm::radians(rad), glm::normalize(glm::cross(Orientation, Up)));
	}

	
	bool Camera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(LZ_BIND_EVENT_FN(Camera::OnKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(LZ_BIND_EVENT_FN(Camera::OnKeyReleased));
		dispatcher.Dispatch<MouseButtonPressedEvent>(LZ_BIND_EVENT_FN(Camera::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(LZ_BIND_EVENT_FN(Camera::OnMouseButtonReleased));
		dispatcher.Dispatch<MouseMovedEvent>(LZ_BIND_EVENT_FN(Camera::OnMouseMoved));
		return false;
	};


	
	bool Camera::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case (GLFW_KEY_W):
		{
			Velocity = speed * Orientation;
			break;
		}
		case (GLFW_KEY_A):
		{

			Velocity = speed * -glm::normalize(glm::cross(Orientation, Up));
			break;
		}
		case (GLFW_KEY_S):
		{
			Velocity = speed * -Orientation;
			break;
		}
		case (GLFW_KEY_D):
		{
			Velocity = speed * glm::normalize(glm::cross(Orientation, Up));
			break;
		}
		//case (GLFW_KEY_RIGHT):
		//{
		//	RotateX(sensitivity * 0.2f);
		//}
		//case (GLFW_KEY_LEFT):
		//{
		//	RotateX(-sensitivity * 0.2f);
		//}
		case (GLFW_KEY_SPACE):
		{
			Velocity = speed * Up;
			break;
		}
		case (GLFW_KEY_LEFT_CONTROL):
		{
			Velocity = speed * -Up;
			break;
		}
		case (GLFW_KEY_LEFT_SHIFT):
		{
			speed = 0.4f;
			break;
		}
		case (GLFW_KEY_RIGHT_SHIFT):
		{
			speed = 0.1f;
		}
		default:
			break;
		}
		return false;
	}
	bool Camera::OnKeyReleased(KeyReleasedEvent& e)
	{
		Velocity = glm::vec3{ 0.0f, 0.0f, 0.0f };
		return false;
	}
	bool Camera::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		mouseClicked = true;
		return false;
	}
	bool Camera::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		mouseClicked = false;
		return false;
	}
	bool Camera::OnMouseMoved(MouseMovedEvent& e)
	{
		if (mouseClicked)
		{
			unsigned int width = lenz::App::GetWindow().GetWidth();
			unsigned int height = lenz::App::GetWindow().GetHeight();
			RotateX(e.GetVelX() / width * sensitivity);
			RotateY(e.GetVelY() / height * sensitivity);
		}
		return false;
	}

}