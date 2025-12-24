#include"Lenz/App.h"
#include"Camera.h"


namespace lenz
{
	Camera::Camera()
	{
		Init();
	}

	void Camera::Init(glm::vec3 position, float fovdeg, float nearPlane, float farPlane)
	{
		Position = position;
		FOVdeg = fovdeg;
		NearPlane = nearPlane;
		FarPlane = farPlane;
	}

	void Camera::OnUpdate(TimeStep& ts)
	{
		unsigned int width = lenz::App::GetWindow().GetWidth();
		unsigned int height = lenz::App::GetWindow().GetHeight();

		lastView = glm::lookAt(Position, Position + Orientation, Up);
		lastProjection = glm::perspective(glm::radians(FOVdeg), (float)width / height, NearPlane, FarPlane);

		Position += Velocity * ts.GetDeltaSecond();
	}

	glm::mat4 Camera::Matrix() const
	{
		return lastProjection * lastView;
	}

	glm::mat4 Camera::Matrix(unsigned int width, unsigned int height)
	{
		return glm::perspective(glm::radians(FOVdeg), (float)width / height, NearPlane, FarPlane) * lastView;
	}

	glm::mat4 Camera::MatrixNoOrientation() const
	{
		return lastProjection * glm::lookAt(Position, glm::vec3(0, 0, 0), Up);
	}

	glm::mat4 Camera::MatrixNoPosition() const
	{
		return lastProjection * glm::lookAt(glm::vec3(0, 0, 0), Orientation, Up);
	}

	glm::mat4 Camera::Projection() const
	{
		return lastProjection;
	}

	glm::mat4 Camera::View() const
	{
		return lastView;
	}

	void Camera::UploadProjectionTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		glm::mat4 mat = Projection();

		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformMat4(uniformname, mat);
	}

	void Camera::UploadViewTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		glm::mat4 mat = View();

		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformMat4(uniformname, mat);
	}

	void Camera::UploadTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		glm::mat4 mat = Matrix();

		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformMat4(uniformname, mat);
	}

	void Camera::UploadInverseTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		glm::mat4 mat = glm::inverse(Matrix());

		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformMat4(uniformname, mat);
	}

	void Camera::UploadOrientationMatrixTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		glm::mat4 mat = MatrixNoPosition();

		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformMat4(uniformname, mat);
	}

	void Camera::UploadPositionMatrixTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		glm::mat4 mat = MatrixNoOrientation();

		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformMat4(uniformname, mat);
	}

	void Camera::UploadPositionTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformFloat3(uniformname, Position);
	}

	void Camera::UploadOrientationTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformFloat3(uniformname, Orientation);
	}


	void Camera::UploadVelocityTo(std::initializer_list<Shader*> shaders, const std::string& uniformname) const
	{
		for (size_t s = 0; s < shaders.size(); s++)
			shaders.begin()[s]->UploadUniformFloat3(uniformname, Velocity);
	}

	//void Camera::StoreProjectionMatrix(Shader& shader)
	//{
	//	shader.UploadUniformMat4(m_ProjectionMatrix, Matrix());
	//}




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
			speed += 1;
			break;
		}
		case (GLFW_KEY_LEFT_SHIFT):
		{
			Velocity = speed * -Up;
			break;
		}
		case (GLFW_KEY_RIGHT_SHIFT):
		{
			Velocity = speed * -Up;
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