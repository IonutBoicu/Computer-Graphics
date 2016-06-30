//-------------------------------------------------------------------------------------------------
// Descriere: header camera
//
// Autor: Lucian Petrescu
// Data: 14 oct 2013
// Update: 10 nov 2015 Gabriel Ivanica
//-------------------------------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <math.h>

#pragma once
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"


glm::vec3 WORLD_UP = glm::vec3(0, 1, 0);

// Rotate a Point/Vector around world OY (0, 1, 0) with a specific angle(radians)
inline glm::vec3 RotateOY(const glm::vec3 P, float radians)
{
	glm::vec3 R;
	R.x = P.x * cos(radians) - P.z * sin(radians);
	R.y = P.y;
	R.z = P.x * sin(radians) + P.z * cos(radians);
	return R;
}

namespace lab
{
	class Camera
	{
	public:
		Camera()
		{
			position = glm::vec3(0, 0, 50);
			forward = glm::vec3(0, 0, -1);
			up		= glm::vec3(0, 1, 0);
			right	= glm::vec3(1, 0, 0);
			distanceToTarget = 40;
		}

		Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
		{
			set(position, center,up);
		}

		~Camera()
		{ }

		// Update camera
		void set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
		{
			this->position = position;
			forward = glm::normalize(center-position);
			right	= glm::cross(forward, up);
			this->up = glm::cross(right,forward);
		}

		void translateForward(float distance)
		{
			glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
			position = position + dir * distance;
		}

		void translateTarget(float distance)
		{
			position = position + forward * distance;
		}


		void translateUpword(float distance)
		{
			position = position + up * distance;
		}

		void translateRight(float distance)
		{
			glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
			position = position + dir * distance;
		}

		void rotateFPS_OX(float angle)
		{
			forward = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1))));
			up = glm::normalize(glm::cross(right, forward));
		}

		void rotateFPS_OY(float angle)
		{
			// Use GLM
			forward = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f), angle, WORLD_UP) * glm::vec4(forward, 1))));
			right = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f), angle, WORLD_UP) * glm::vec4(right, 1))));

			// Use simple rotation matrix
			//forward = RotateOY(forward, glm::radians(angle));
			//right	= RotateOY(right, glm::radians(angle));

			// or use cross product... whatever!
			//up = RotateOY(up, glm::radians(angle));
			up = glm::normalize(glm::cross(right, forward));
		}

		void rotateFPS_OZ(float angle)
		{
			right = glm::normalize(glm::vec3( (glm::rotate(glm::mat4(1.0f) ,angle, forward) * glm::vec4(right,1)) ));
			up = glm::normalize(glm::cross(right, forward));
		}

		void rotateTPS_OX(float angle)
		{
			translateTarget(distanceToTarget);
			rotateFPS_OX(angle);
			translateTarget(-distanceToTarget);
		}

		void rotateTPS_OY(float angle)
		{
			translateTarget(distanceToTarget);
			rotateFPS_OY(angle);
			translateTarget(-distanceToTarget);
		}

		void rotateTPS_OZ(float angle)
		{
			translateTarget(distanceToTarget);
			rotateFPS_OZ(angle);
			translateTarget(-distanceToTarget);
		}

		glm::mat4 getViewMatrix()
		{
			return glm::lookAt(position, position + forward, up);
		}

		glm::vec3 getTargetPosition()
		{
			return position + forward * distanceToTarget;
		}

		glm::vec3 getPosition()
		{
			return position;
		}

		glm::vec3 getForward()
		{
			return forward;
		}

	private:
		float distanceToTarget;
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
	};
}