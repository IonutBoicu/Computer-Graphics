
#include "lab_blackbox.hpp"
#include "lab_camera.hpp"


//time
#include <ctime>


#define  KEY_ESCAPE		27


class Laborator5
	: public lab::glut::WindowListener
{
	private:
		// The BlackBox hides some functionality that we'll learn in the next course
		lab::BlackBox BLACKBOX;

		// Buffers used for holding state of keys
		// true - pressed
		// false - not pressed
		bool keyState[256];
		bool specialKeyState[256];
		bool jump = false, collUp, collDown, collRight, collLeft, didMove, pressC1, pressC2;
		int jumps = 0, rotate = 0, rotiri = 0;
		float step = 0.001f;
		char lastMove;
		// Objects
		lab::Mesh *obiect, *target;

		// helper variables for third person camera drawing
		bool thirdPersonCamera;
		bool alwaysDrawTarget;

		// Projection matrix
		bool isPerspectiveProjection;
		glm::mat4 projectionMatrix1, projectionMatrix2, targetMatrix;
		glm::mat4 *objectModelMatrix;
		glm::vec3 dim[39];
		float FoV;
		float zNear, zFar;
		float aspectRatio;
		float orthoLeft, orthoRight, orthoTop, orthoBottom;
		float curx, cury, curz,camx,camy,camz;
		// Camera
		lab::Camera cameraTPS, cameraFPS;

	public:
		Laborator5()
		{
			// Load Objects
			obiect = BLACKBOX.loadMesh("resurse/cube.obj");
			target = BLACKBOX.loadMesh("resurse/cube.obj");
			// init camera
			cameraTPS.set(glm::vec3(0.5f, 1.25f, 5.0f), glm::vec3(0.5f, 1.25f , 0.5), glm::vec3(0, 1, 0));
			cameraFPS.set(glm::vec3(0.5f, 1.75f, 0.5f), glm::vec3(0.5f, 1.75f, 1), glm::vec3(0, 1, 0));
			cameraFPS.rotateFPS_OY(90);
			curx = 0.25f, cury = 1.0f, curz = 0.25f;
			camx = 0.5f, camy = 1.75f, camz = 0.5f;
			// initializa all key states to not pressed
			memset(keyState, 0, 256);
			memset(specialKeyState, 0, 256);
			// scene
			objectModelMatrix = new glm::mat4[40];
			collDown = true, didMove = false, collLeft = false, collUp = false, collRight = false, pressC1 = false, pressC2 = false;
			
			objectModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
			objectModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(1, 0, 0));
			objectModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 0));
			objectModelMatrix[3] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 0, 0));
			objectModelMatrix[4] = glm::translate(glm::mat4(1.0f), glm::vec3(5, 0, 0));
			objectModelMatrix[5] = glm::translate(glm::mat4(1.0f), glm::vec3(5, 0, -1));
			objectModelMatrix[6] = glm::translate(glm::mat4(1.0f), glm::vec3(6, 0, 0));
			objectModelMatrix[7] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 0, 0));
			objectModelMatrix[8] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 0, -1));
			objectModelMatrix[9] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 0, -2));
			objectModelMatrix[10] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 0, -3));
			objectModelMatrix[11] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 1, -2));	
			objectModelMatrix[12] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 1, -3));
			objectModelMatrix[13] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 2, -3));
			objectModelMatrix[14] = glm::translate(glm::mat4(1.0f), glm::vec3(6, 2, -3));
			objectModelMatrix[16] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 0, -3));
			objectModelMatrix[17] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 1, -3));
			objectModelMatrix[15] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 2, -3));
			objectModelMatrix[18] = glm::translate(glm::mat4(1.0f), glm::vec3(3, 2, -3));
			objectModelMatrix[19] = glm::translate(glm::mat4(1.0f), glm::vec3(2, 2, -3));
			objectModelMatrix[20] = glm::translate(glm::mat4(1.0f), glm::vec3(2, 2, -4));
			objectModelMatrix[21] = glm::translate(glm::mat4(1.0f), glm::vec3(2, 2, -5));
			objectModelMatrix[22] = glm::translate(glm::mat4(1.0f), glm::vec3(3, 2, -5));
			objectModelMatrix[23] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 2, -5));
			objectModelMatrix[24] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 2, -4));
			objectModelMatrix[25] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 3, -5));
			objectModelMatrix[26] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 3, -4));
			objectModelMatrix[27] = glm::translate(glm::mat4(1.0f), glm::vec3(4, 4, -4));
			objectModelMatrix[28] = glm::translate(glm::mat4(1.0f), glm::vec3(5, 4, -4));
			objectModelMatrix[29] = glm::translate(glm::mat4(1.0f), glm::vec3(6, 4, -4));
			objectModelMatrix[30] = glm::translate(glm::mat4(1.0f), glm::vec3(7, 4, -4));
			objectModelMatrix[31] = glm::translate(glm::mat4(1.0f), glm::vec3(9, 4, -4));
			objectModelMatrix[32] = glm::translate(glm::mat4(1.0f), glm::vec3(9, 4, -2));
			objectModelMatrix[33] = glm::translate(glm::mat4(1.0f), glm::vec3(11, 4, -2));
			objectModelMatrix[34] = glm::translate(glm::mat4(1.0f), glm::vec3(11, 0, -1));
			objectModelMatrix[35] = glm::translate(glm::mat4(1.0f), glm::vec3(11, 0, 0));
			objectModelMatrix[36] = glm::translate(glm::mat4(1.0f), glm::vec3(-1, 0, 0));
			objectModelMatrix[37] = glm::translate(glm::mat4(1.0f), glm::vec3(-2, 0, 0));
			objectModelMatrix[38] = glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 1));


			dim[0] = glm::vec3(0, 0, 0);
			dim[1] = glm::vec3(1, 0, 0);
			dim[2] = glm::vec3(2, 0, 0);
			dim[3] = glm::vec3(4, 0, 0);
			dim[4] = glm::vec3(5, 0, 0);
			dim[5] = glm::vec3(5, 0, -1);
			dim[6] = glm::vec3(6, 0, 0);
			dim[7] = glm::vec3(7, 0, 0);
			dim[8] = glm::vec3(7, 0, -1);
			dim[9] = glm::vec3(7, 0, -2);
			dim[10] = glm::vec3(7, 0, -3);
			dim[11] = glm::vec3(7, 1, -2);
			dim[12] = glm::vec3(7, 1, -3);
			dim[13] = glm::vec3(7, 2, -3);
			dim[14] = glm::vec3(6, 2, -3);
			dim[16] = glm::vec3(4, 0, -3);
			dim[17] = glm::vec3(4, 1, -3);
			dim[15] = glm::vec3(4, 2, -3);
			dim[18] = glm::vec3(3, 2, -3);
			dim[19] = glm::vec3(2, 2, -3);
			dim[20] = glm::vec3(2, 2, -4);
			dim[21] = glm::vec3(2, 2, -5);
			dim[22] = glm::vec3(3, 2, -5);
			dim[23] = glm::vec3(4, 2, -5);
			dim[24] = glm::vec3(4, 2, -4);
			dim[25] = glm::vec3(4, 3, -5);
			dim[26] = glm::vec3(4, 3, -4);
			dim[27] = glm::vec3(4, 4, -4);
			dim[28] = glm::vec3(5, 4, -4);
			dim[29] = glm::vec3(6, 4, -4);
			dim[30] = glm::vec3(7, 4, -4);
			dim[31] = glm::vec3(9, 4, -4);
			dim[32] = glm::vec3(9, 4, -2);
			dim[33] = glm::vec3(11, 4, -2);
			dim[34] = glm::vec3(11, 0, -1);
			dim[35] = glm::vec3(11, 0, 0);
			dim[36] = glm::vec3(-1, 0, 0);
			dim[37] = glm::vec3(-2, 0, 0);
			dim[38] = glm::vec3(1, 1, 1);
			// helper variables for third person camera
			thirdPersonCamera = false;
			alwaysDrawTarget = false;

			// Initialize default projection values
			zNear = 0.1f;
			zFar = 10.0f;
			FoV = 60.0f;
			aspectRatio = 1200 / 800.0f;
			orthoLeft = -5;
			orthoRight = 5;
			orthoBottom = -5;
			orthoTop = 5;

			// value may be used for updating the projection when reshaping the window
			//isPerspectiveProjection = false;

			projectionMatrix1 = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, zNear, zFar);
			projectionMatrix2 = glm::perspective(FoV, aspectRatio, zNear, zFar);
		}

		~Laborator5()
		{
			delete obiect;
		}

		// Function used to compute perspective projection matrix
		// use glm::perspective and member variables FoV, aspectRation, zNear and zFar
		void computePerspectiveProjection()
		{
			// projectionMatrix = glm::mat4(1);
		}
		void computeOrthograhicProjection()
		{
			//projectionMatrix = glm::mat4(1);
		}

		//---------------------------------------------------------------------
		// Loop Functions - function that are called every single frame

		// Called right before frame update callback (notifyDisplayFrame)
		void notifyBeginFrame() { };




		// Called every frame before we draw
		// Because glut sends only 1 key event every single frame, pressing more than 1 key will have no effect
		// If we treat the input using the 2 special buffers where we hold key states we can treat more than 1
		// key pressed at the same time. Also by not relling on glut to send the continuous pressing event
		// the moving motion will be more smooth because every single frame we apply the changes is contrast to
		// the event based method provided by Glut
		void treatInput()
		{
			// Compute frameRate independent speed for moving the camera
			// We set the speed based on the frameTime between the last 2 frames
			// Speed will remain constant no matter how many frames/second is the GPU cappable to render
			float frameTime = BLACKBOX.getFrameTimeSeconds();
			float moveSpeed = frameTime * 20;

			// If camera rotate counterclockwise animate it
			if (pressC1 == true) {
				if (rotiri < 89) {
					cameraTPS.rotateTPS_OY(-1);
					cameraFPS.rotateFPS_OY(-1);
					rotiri++;
					Sleep(5);
				}
				else {
					pressC1 = false;
					rotiri = 0;
					if (rotate == 0)
						rotate = 3;
					else
						rotate--;
				}
			}
			// if camera rotate clockwise animate it
			if (pressC2 == true) {
				if (rotiri < 89) {
					cameraTPS.rotateTPS_OY(1);
					cameraFPS.rotateFPS_OY(1);
					rotiri++;
					Sleep(5);
				}
				else {
					pressC2 = false;
					rotiri = 0;
					if (rotate == 3)
						rotate = 0;
					else 
						rotate++;
				}
			}

			// COLLISIONS
			//check down
			if (didMove) {
				collDown = false;
				collUp = false;
				collRight = false;
				collLeft = false;
			}

			// check bot
			for (int i = 0; i <= 38 && !collDown; i++) {
				if (((curx >= dim[i][0] && (curx - 0.99) <= dim[i][0]) || (curx + 0.51 >= dim[i][0] && (curx - 0.49) <= dim[i][0])) &&
					(dim[i][1] <= (cury - 0.99) && dim[i][1] >= (cury - 1.01)) && ((curz + 0.01 >= dim[i][2] && (curz - 0.99) <= dim[i][2]) || (curz + 0.51 >= dim[i][2] && (curz - 0.49) <= dim[i][2]))) {
					collDown = true;
					break;
				}	
			}

			//check left
			for (int i = 0; i <= 38 && !collLeft; i++) {
				if (rotate % 2 == 0) {
					if ((curx - 0.99 >= dim[i][0] && dim[i][0] >= curx - 1.01) &&
						(cury + 0.49 >= dim[i][1] && dim[i][1] >= cury - 0.99) &&
						((curz + 0.01 >= dim[i][2] && (curz - 0.99) <= dim[i][2]) || (curz + 0.51 >= dim[i][2] && (curz - 0.49) <= dim[i][2]))) {
						collLeft = true;
						break;
					}
				}
				else {
					if ((curz + 0.49 <= dim[i][2] && dim[i][2] <= curz + 0.51) &&
						(cury + 0.49 >= dim[i][1] && dim[i][1] >= cury - 0.99) &&
						((curx +0.01 >= dim[i][0] && (curx - 0.99) <= dim[i][0]) || (curx + 0.51 >= dim[i][0] && (curx - 0.49) <= dim[i][0]))) {
						collLeft = true;
						break;
					}
				}
			}
			//check right
			for (int i = 0; i <= 38 && !collRight; i++) {
				if (rotate % 2 == 0) {
					if ((curx + 0.51 >= dim[i][0] && dim[i][0] >= curx + 0.49) &&
						(cury + 0.49 >= dim[i][1] && dim[i][1] >= cury - 0.99) &&
						((curz + 0.01 >= dim[i][2] && (curz - 0.99) <= dim[i][2]) || (curz + 0.51 >= dim[i][2] && (curz - 0.49) <= dim[i][2]))) {
						collRight = true;
						break;
					}
				}
				else {
					if ((curz - 1.01 <= dim[i][2] && dim[i][2] <= curz - 0.99) &&
						(cury + 0.49 >= dim[i][1] && dim[i][1] >= cury - 0.99) &&
						((curx +0.01 >= dim[i][0] && (curx - 0.99) <= dim[i][0]) || (curx + 0.51 >= dim[i][0] && (curx - 0.49) <= dim[i][0]))) {
						collRight = true;
						break;
					}
				}
			}
			//check up
			for (int i = 0; i <= 38 && !collUp; i++) {
				if (((curx >= dim[i][0] && (curx - 0.99) <= dim[i][0]) || (curx + 0.51 >= dim[i][0] && (curx - 0.49) <= dim[i][0])) &&
					(dim[i][1] >= cury +0.49 && dim[i][1] <= cury + 0.51) && ((curz >= dim[i][2] && (curz - 0.99) <= dim[i][2]) || (curz + 0.51 >= dim[i][2] && (curz - 0.49) <= dim[i][2]))) {
					collUp = true;
					break;
				}
			}
			didMove = false;

			// no surface -> go down (animated gravity)
			if (!collDown) {
				cury -= step;
				camy -= step;
				cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5 + curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
				cameraTPS.rotateTPS_OY(90 * rotate);
				cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
				cameraFPS.rotateFPS_OY(90 * rotate);
				cameraTPS.translateUpword(-moveSpeed);
			}
			// move Left
			if (keyState['a']) {
				if (!collLeft) {
					// move on x
					if (rotate % 2 == 0) {
						if (rotate < 2) {
							curx -= step;
							camx -= step;
						}
						else {
							curx += step;
							camx += step;
						}
						cameraFPS.set(glm::vec3(camx,camy,camz), glm::vec3(camx,camy,camz + 0.25), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate - 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5 + curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
						lastMove = 's';
						didMove = true;
					}
					// move on z
					else {
						if (rotate < 2) {
							curz += step;
							camz += step;
						}
						else {
							curz -= step;
							camz -= step;
						}
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate - 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5 + curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
						lastMove = 's';
						didMove = true;
					}
				}

			}
			// move right
			if (keyState['d']) {
				if (!collRight) {
					// move on x
					if (rotate % 2 == 0) {
						if (rotate < 2) {
							curx += step;
							camx += step;
						}
						else {
							curx -= step;
							camx -= step;
						}
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate + 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5 + curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
						lastMove = 'd';
						didMove = true;
					}
					// move on z
					else {
						if (rotate < 2) {
							curz -= step;
							camz -= step;
						}
						else {
							curz += step;
							camz += step;
						}
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate + 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5 + curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
						cameraTPS.translateRight(moveSpeed);
						lastMove = 'd';
						didMove = true;
					}
				}
			}

			// Spacebar pressed
			if (keyState[32]) {	
				//jump around
				if (jump == false) {
					jump = true;
					jumps = 0;
				}
			}
			// is jumping
			if (jump == true) {
				didMove = true;
				if (lastMove == 'd'){
					if (jumps <= 130) {
						// jump on x
						if (rotate % 2 == 0){
							if (rotate < 2) {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collRight){
									curx += 0.005;
									camx += 0.005;
								}
							}
							else {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collRight) {
									curx -= 0.005;
									camx -= 0.005;
								}
							}
						}
						// jump on z
						else {
							if (rotate < 2) {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collRight) {
									curz -= 0.005;
									camz -= 0.005;
								}
							}
							else {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collRight) {
									curz += 0.005;
									camz += 0.005;
								}
							}
						}
						
						jumps++;
						Sleep(2);
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate + 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5+curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
					}
					else if (jumps <= 261){
						// jump on x
						if (rotate % 2 == 0) {
							if (rotate < 2) {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collRight) {
									curx += 0.005;
									camx += 0.005;
								}
							}
							else {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collRight) {
									curx -= 0.005;
									camx -= 0.005;
								}
							}
						}
						// jump on z
						else {
							if (rotate < 2) {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collRight) {
									curz -= 0.005;
									camz -= 0.005;
								}
							}
							else {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collRight) {
									curz += 0.005;
									camz += 0.005;
								}
							}
						}
						jumps++;
						Sleep(2);
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate + 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5+curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
					}
					// last step of jumping
					else {
						jumps = 0;
						jump = false;
						cury = glm::trunc(cury);
						camy = glm::trunc(camy);
						camy += 0.75f;
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate + 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5+curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
					}
				}
				else {
					didMove = true;
					if (jumps <= 130) {
						// jump on x
						if (rotate % 2 == 0) {
							if (rotate < 2) {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collLeft) {
									curx -= 0.005;
									camx -= 0.005;
								}
							}
							else {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collLeft) {
									curx += 0.005;
									camx += 0.005;
								}
							}
						}
						// jump on z
						else {
							if (rotate < 2) {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collLeft) {
									curz += 0.005;
									camz += 0.005;
								}
							}
							else {
								if (!collUp) {
									cury += 0.01f;
									camy += 0.01f;
								}
								if (!collLeft) {
									curz -= 0.005;
									camz -= 0.005;
								}
							}
						}
						jumps++;
						Sleep(2);
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate - 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5+curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
					}
					else if (jumps <= 261){
					// jump on x
						if (rotate % 2 == 0) {
							if (rotate < 2) {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collLeft) {
									curx -= 0.005;
									camx -= 0.005;
								}
							}
							else {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collLeft) {
									curx += 0.005;
									camx += 0.005;
								}
							}
						}
						// jump on z
						else {
							if (rotate < 2) {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collLeft) {
									curz += 0.005;
									camz += 0.005;
								}
							}
							else {
								if (!collDown) {
									cury -= 0.01f;
									camy -= 0.01f;
								}
								if (!collLeft) {
									curz -= 0.005;
									camz -= 0.005;
								}
							}
						}
						jumps++;
						Sleep(2);
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate - 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5+curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
					}
					// last jumping step
					else {
						jumps = 0;
						jump = false;
						cury = glm::trunc(cury);
						camy = glm::trunc(camy);
						camy += 0.75f;
						cameraFPS.set(glm::vec3(camx, camy, camz), glm::vec3(camx, camy, 0.25 + camz), glm::vec3(0, 1, 0));
						cameraFPS.rotateFPS_OY(90 * (rotate - 1));
						cameraTPS.set(glm::vec3(0.25 + curx, 0.25 + cury, 5+curz), glm::vec3(0.25 + curx, 0.25f + cury, 0.25 + curz), glm::vec3(0, 1, 0));
						cameraTPS.rotateTPS_OY(90 * rotate);
					}
				}
			}
		}

		// A key was pressed
		void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y)
		{
			keyState[key_pressed] = 1;

			if (key_pressed == KEY_ESCAPE)
			{
				lab::glut::close();
			}
		
			// Set the projection matrix to Orthographic
			if (key_pressed == 'o')
			{
				projectionMatrix2=glm::ortho(orthoLeft,orthoRight,orthoBottom,orthoTop,zNear,zFar);
				projectionMatrix1 = glm::perspective(FoV, aspectRatio, zNear, zFar);
				isPerspectiveProjection = false;
			}

			// Set the projection matrix to Perspective 
			if (key_pressed == 'p')
			{
				projectionMatrix1 = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, zNear, zFar);
				projectionMatrix2=glm::perspective(FoV,aspectRatio,zNear,zFar);
				isPerspectiveProjection = true;
			}

			// Reset Camera
			if (keyState['c'])
			{
				cameraTPS.set(glm::vec3(0.5f, 1.25f, 5.0f), glm::vec3(0.5f, 1.25f, 0.5), glm::vec3(0, 1, 0));
				cameraFPS.set(glm::vec3(0.5f, 1.75f, 0.5f), glm::vec3(0.5f, 1.75f, 1), glm::vec3(0, 1, 0));
				cameraFPS.rotateFPS_OY(90);
				curx = 0.25f, cury = 1.0f, curz = 0.25f;
				camx = 0.5f, camy = 1.75f, camz = 0.5f;
			}

		}

		// When a key was released
		void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y)
		{
			keyState[key_released] = 0;

			// Disable drawing of camera target if no longer moving in ThirdPerson
			if (thirdPersonCamera)
			{
				char sum = 0;
				for (unsigned int i = 0; i <= 9; i++) {
					sum += keyState[i + '0'];
				}
				if (!sum) thirdPersonCamera = false;
			}

		}

		// Special key pressed like the navigation arrows or function keys F1, F2, ...
		void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y)
		{
			specialKeyState[key_pressed] = 1;

			switch (key_pressed)
			{
			case GLUT_KEY_F1: {
				lab::glut::enterFullscreen();
				break;
			}

			case GLUT_KEY_F2: {
				lab::glut::exitFullscreen();
				break;
			}

			case GLUT_KEY_F5: {
				BLACKBOX.LoadShader();
				break;
			}
			case GLUT_KEY_LEFT: {pressC1 = true; cameraTPS.rotateTPS_OY(-1); cameraFPS.rotateFPS_OY(-1); break; }
			case GLUT_KEY_RIGHT: {pressC2 = true; cameraTPS.rotateTPS_OY(1); cameraFPS.rotateFPS_OY(1); break; }

			default:
				break;
			}
		}

		// Called when a special key was released
		void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y)
		{
			specialKeyState[key_released] = 0;
		}

		// Called every frame to draw
		void notifyDisplayFrame()
		{
			// Treat continuous input
			treatInput();

			// Clear Color Buffer with the specified color
			glClearColor(1, 1, 1, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			BLACKBOX.notifyDisplay();

			// ----------------------------------
			// Set the viewport and view and projection matrices

			unsigned int width = lab::glut::getInitialWindowInformation().width;
			unsigned int height = lab::glut::getInitialWindowInformation().height;

			// VIIEW PORT 2 FPS CAMERA//////
			///////////////////////////////
			////////////////////////////////
			glViewport(0, height*2/3, width / 3, height / 3);
			// Send view matrix to the GPU
			BLACKBOX.setViewMatrix(cameraFPS.getViewMatrix());

			// Send projection matrix to the GPU
			BLACKBOX.setProjectionMatrix(projectionMatrix2);

			// ----------SCENA-------------
			// 3 in line
			BLACKBOX.setModelMatrix(objectModelMatrix[0]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[1]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[2]);
			BLACKBOX.drawMesh(obiect);
			// pause1, 3 in line
			BLACKBOX.setModelMatrix(objectModelMatrix[3]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[4]);
			BLACKBOX.drawMesh(obiect);
			//lat
			BLACKBOX.setModelMatrix(objectModelMatrix[5]);
			BLACKBOX.drawMesh(obiect);
			//endlat
			BLACKBOX.setModelMatrix(objectModelMatrix[6]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[7]);
			BLACKBOX.drawMesh(obiect);
			//corner with 3. 1,2 levels upper
			BLACKBOX.setModelMatrix(objectModelMatrix[8]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[9]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[10]);
			BLACKBOX.drawMesh(obiect);
			// up 1 level
			BLACKBOX.setModelMatrix(objectModelMatrix[11]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[12]);
			BLACKBOX.drawMesh(obiect);
			//up 2 levels
			BLACKBOX.setModelMatrix(objectModelMatrix[13]);
			BLACKBOX.drawMesh(obiect);
			// next corner
			BLACKBOX.setModelMatrix(objectModelMatrix[14]);
			BLACKBOX.drawMesh(obiect);
			//pause 1
			//lv 0
			BLACKBOX.setModelMatrix(objectModelMatrix[16]);
			BLACKBOX.drawMesh(obiect);
			//lv 1
			BLACKBOX.setModelMatrix(objectModelMatrix[17]);
			BLACKBOX.drawMesh(obiect);
			//lv 2 circle with empty mid
			BLACKBOX.setModelMatrix(objectModelMatrix[15]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[18]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[19]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[20]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[21]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[22]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[23]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[24]);
			BLACKBOX.drawMesh(obiect);
			//3 lv up
			BLACKBOX.setModelMatrix(objectModelMatrix[25]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[26]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[27]);
			BLACKBOX.drawMesh(obiect);
			// line at lv4
			BLACKBOX.setModelMatrix(objectModelMatrix[28]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[29]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[30]);
			BLACKBOX.drawMesh(obiect);
			//1 pause 
			BLACKBOX.setModelMatrix(objectModelMatrix[31]);
			BLACKBOX.drawMesh(obiect);
			//1 pause
			BLACKBOX.setModelMatrix(objectModelMatrix[32]);
			BLACKBOX.drawMesh(obiect);
			//1 pause
			BLACKBOX.setModelMatrix(objectModelMatrix[33]);
			BLACKBOX.drawMesh(obiect);
			//Finish ground level
			BLACKBOX.setModelMatrix(objectModelMatrix[34]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[35]);
			BLACKBOX.drawMesh(obiect);

			BLACKBOX.setModelMatrix(objectModelMatrix[36]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[37]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[38]);
			BLACKBOX.drawMesh(obiect);

			//target object
			targetMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(curx, cury, curz));
			targetMatrix = glm::scale(targetMatrix, glm::vec3(0.5f));
			BLACKBOX.setModelMatrix(targetMatrix);
			BLACKBOX.drawMesh(target);



			glViewport(0, 0, width, height);

			// Send view matrix to the GPU
			BLACKBOX.setViewMatrix(cameraTPS.getViewMatrix());
			
			// Send projection matrix to the GPU
			BLACKBOX.setProjectionMatrix(projectionMatrix1);

			// ----------SCENA-------------
			// 3 in line
			BLACKBOX.setModelMatrix(objectModelMatrix[0]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[1]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[2]);
			BLACKBOX.drawMesh(obiect);
			// pause1, 3 in line
			BLACKBOX.setModelMatrix(objectModelMatrix[3]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[4]);
			BLACKBOX.drawMesh(obiect);
				//lat
			BLACKBOX.setModelMatrix(objectModelMatrix[5]);
			BLACKBOX.drawMesh(obiect);
				//endlat
			BLACKBOX.setModelMatrix(objectModelMatrix[6]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[7]);
			BLACKBOX.drawMesh(obiect);
			//corner with 3. 1,2 levels upper
			BLACKBOX.setModelMatrix(objectModelMatrix[8]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[9]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[10]);
			BLACKBOX.drawMesh(obiect);
				// up 1 level
			BLACKBOX.setModelMatrix(objectModelMatrix[11]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[12]);
			BLACKBOX.drawMesh(obiect);
				//up 2 levels
			BLACKBOX.setModelMatrix(objectModelMatrix[13]);
			BLACKBOX.drawMesh(obiect);
			// next corner
			BLACKBOX.setModelMatrix(objectModelMatrix[14]);
			BLACKBOX.drawMesh(obiect);
			//pause 1
				//lv 0
			BLACKBOX.setModelMatrix(objectModelMatrix[16]);
			BLACKBOX.drawMesh(obiect);
				//lv 1
			BLACKBOX.setModelMatrix(objectModelMatrix[17]);
			BLACKBOX.drawMesh(obiect);
			//lv 2 circle with empty mid
			BLACKBOX.setModelMatrix(objectModelMatrix[15]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[18]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[19]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[20]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[21]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[22]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[23]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[24]);
			BLACKBOX.drawMesh(obiect);
				//3 lv up
			BLACKBOX.setModelMatrix(objectModelMatrix[25]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[26]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[27]);
			BLACKBOX.drawMesh(obiect);
			// line at lv4
			BLACKBOX.setModelMatrix(objectModelMatrix[28]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[29]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[30]);
			BLACKBOX.drawMesh(obiect);
			//1 pause 
			BLACKBOX.setModelMatrix(objectModelMatrix[31]);
			BLACKBOX.drawMesh(obiect);
			//1 pause
			BLACKBOX.setModelMatrix(objectModelMatrix[32]);
			BLACKBOX.drawMesh(obiect);
			//1 pause
			BLACKBOX.setModelMatrix(objectModelMatrix[33]);
			BLACKBOX.drawMesh(obiect);
			//Finish ground level
			BLACKBOX.setModelMatrix(objectModelMatrix[34]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[35]);
			BLACKBOX.drawMesh(obiect);
			
			BLACKBOX.setModelMatrix(objectModelMatrix[36]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[37]);
			BLACKBOX.drawMesh(obiect);
			BLACKBOX.setModelMatrix(objectModelMatrix[38]);
			BLACKBOX.drawMesh(obiect);

			//target object
			targetMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(curx, cury, curz));
			targetMatrix = glm::scale(targetMatrix, glm::vec3(0.5f));
			BLACKBOX.setModelMatrix(targetMatrix);
			BLACKBOX.drawMesh(target);
			///////////////////////////////
			
		}

		// Called when the frame ended
		void notifyEndFrame() { }

		//---------------------------------------------------------------------
		// Function called when the windows was resized
		void notifyReshape(int width, int height, int previos_width, int previous_height)
		{
			//blackbox needs to know
			BLACKBOX.notifyReshape(width, height);
			aspectRatio = (float)width / height;

			// TODO - [OPTIONAL]
			// Recompute the orthographic or perspective matrices

		}

		//---------------------------------------------------------------------
		// Input function

		// Mouse drag, mouse button pressed 
		void notifyMouseDrag(int mouse_x, int mouse_y) { }

		// Mouse move without pressing any button
		void notifyMouseMove(int mouse_x, int mouse_y) { }

		// Mouse button click
		void notifyMouseClick(int button, int state, int mouse_x, int mouse_y) { }

		// Mouse scrolling
		void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y) { }
};

int main()
{
	// Initialize GLUT: window + input + OpenGL context
	lab::glut::WindowInfo window(std::string("BOICU IONUT 335CA - TEMA2 EGC"), 800, 600, 600, 100, true);
	lab::glut::ContextInfo context(3, 3, false);
	lab::glut::FramebufferInfo framebuffer(true, true, false, false);
	lab::glut::init(window, context, framebuffer);

	// Initialize GLEW + load OpenGL extensions 
	glewExperimental = true;
	glewInit();
	std::cout << "[GLEW] : initializare" << std::endl;

	// Create a new instance of Lab and listen for OpenGL callback
	// Must be created after GLEW because we need OpenGL extensions to be loaded

	Laborator5 *lab5 = new Laborator5();
	lab::glut::setListener(lab5);

	// Enter loop
	lab::glut::run();

	return 0;
}