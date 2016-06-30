//-------------------------------------------------------------------------------------------------
// Descriere: fisier main
//
// Autor: student
// Data: today
//-------------------------------------------------------------------------------------------------

//incarcator de meshe
#include "lab_mesh_loader.hpp"
//geometrie: drawSolidCube, drawWireTeapot...
#include "lab_geometry.hpp"
//incarcator de shadere
#include "lab_shader_loader.hpp"
//interfata cu glut, ne ofera fereastra, input, context opengl
#include "lab_glut.hpp"
//texturi
#include "lab_texture_loader.hpp"
//time
#include <ctime>

#include "lab_camera.hpp"



class Laborator : public lab::glut::WindowListener
{
	//variabile
	private:
		glm::mat4 model_matrix_lit, model_matrix_cube, model_matrix_menu;				//matrici modelare pentru cele 3 obiecte
		glm::mat4 view_matrix, projection_matrix, projection_matrix2, view_matrix2;			//matrici 4x4 pt modelare vizualizare proiectie
		unsigned int gl_program_shader;														//id-ul de opengl al obiectului de tip program shader
		unsigned int screen_width, screen_height;
		int ind_offset, animation, discardability,font;
		float offsetx, offsetz;
		std::vector<float>last_offx, last_offz;
		std::vector<int>phase;
		//containere opengl pentru vertecsi, indecsi si stare
		unsigned int cube_vbo, cube_ibo, cube_vao, cube_num_indices, lit_vbo, lit_ibo, lit_vao, lit_num_indices, menu_vbo, menu_ibo, menu_vao, menu_num_indices;		
		std::vector<lab::VertexFormat> vertices, menuVer;
		std::vector<unsigned int> indices, menuInd;
		//texturi
		unsigned int mesh_vbo_ground, mesh_ibo_ground, mesh_vao_ground, mesh_num_indices_ground;
		unsigned int cube_texture, lit_texture_alpha, lit_texture_color, menu_texture_alpha, menu_texture_color;
		bool edit_enable;
		// Camera
		lab::Camera camera;
		glm::mat4 projectionMatrix;
		float FoV;
		float zNear, zFar;
		float aspectRatio;
		// another camera
		lab::Camera camera2;
		glm::mat4 projectionMatrix2;


		glm::vec3 light_position;
		glm::vec3 eye_position;
		unsigned int material_shininess;
		float material_kd;
		float material_ks;

		// Buffer for input
		char keyState[256];
		char specialKeyState[256];

		unsigned int frameTime;
		unsigned int previousTime;

	//metode
	public:
	
		//constructor .. e apelat cand e instantiata clasa
		Laborator()
		{
			edit_enable = false;
			ind_offset = 0;
			//setari pentru desenare, clear color seteaza culoarea de clear pentru ecran (format R,G,B,A)
			glClearColor(0.5,0.5,0.5,1);
			glClearDepth(1);			//clear depth si depth test (nu le studiem momentan, dar avem nevoie de ele!)
			glEnable(GL_DEPTH_TEST);	//sunt folosite pentru a determina obiectele cele mai apropiate de camera (la curs: algoritmul pictorului, algoritmul zbuffer)
			offsetx = offsetz = 0; animation = discardability = font = 0;
			//incarca un shader din fisiere si gaseste locatiile matricilor relativ la programul creat
			gl_program_shader = lab::loadShader("shadere\\shader_vertex.glsl", "shadere\\shader_fragment.glsl");
			//cube
			lab::loadObj("resurse\\cube.obj", cube_vao, cube_vbo, cube_ibo, cube_num_indices);
			cube_texture = lab::loadTextureBMP("resurse\\LaminatedOak.bmp");
			model_matrix_cube = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
			// litera
			lit_texture_color = lab::loadTextureBMP("resurse\\lucidagrande.bmp");
			lit_texture_alpha = lab::loadTextureBMP("resurse\\lucidagrande.bmp");
			model_matrix_lit = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
			// meniu
			model_matrix_menu = glm::translate(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), glm::vec3(-18, 10, 0));
			//matrici de modelare si vizualizare
			view_matrix = glm::lookAt(glm::vec3(0,0,20), glm::vec3(0,0,0), glm::vec3(0,1,0));
			// init camera
			camera.set(glm::vec3(0, 0, 30), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			memset(keyState, 0, 256);
			memset(specialKeyState, 0, 256);

			// Initialize default projection values
			zNear = 0.1f;
			zFar = 500.0f;
			FoV = 60.0f;
			aspectRatio = 800 / 600.0f;

			previousTime = 0;

			//menu
			// Add 'Menu'
			//m
			menuVer.push_back(lab::VertexFormat(0, 0, 0, 0, 0, 1, 0.5f, 0.51f));	// stanga jos
			menuVer.push_back(lab::VertexFormat(1, 0, 0, 0, 0, 1, 0.595f, 0.51f));   // drapta jos
			menuVer.push_back(lab::VertexFormat(1, 1, 0, 0, 0, 1, 0.595f, 0.6f));   // drapta sus
			menuVer.push_back(lab::VertexFormat(0, 1, 0, 0, 0, 1, 0.5f, 0.6f));   // stanga sus
			// primu triunghi
			menuInd.push_back(0);
			menuInd.push_back(1);
			menuInd.push_back(2);
			// al 2lea triunghi
			menuInd.push_back(0);
			menuInd.push_back(2);
			menuInd.push_back(3);
			//e
			menuVer.push_back(lab::VertexFormat(0.9 + 0, 0, 0, 0, 0, 1, 0.7f, 0.61f));	// stanga jos
			menuVer.push_back(lab::VertexFormat(0.9 + 1, 0, 0, 0, 0, 1, 0.77f, 0.61f));   // drapta jos
			menuVer.push_back(lab::VertexFormat(0.9 + 1, 1, 0, 0, 0, 1, 0.77f, 0.7f));   // drapta sus
			menuVer.push_back(lab::VertexFormat(0.9 + 0, 1, 0, 0, 0, 1, 0.7f, 0.7f));   // stanga sus
			// primu triunghi
			menuInd.push_back(4 + 0);
			menuInd.push_back(4 + 1);
			menuInd.push_back(4 + 2);
			// al 2lea triunghi
			menuInd.push_back(4 + 0);
			menuInd.push_back(4 + 2);
			menuInd.push_back(4 + 3);
			//n
			menuVer.push_back(lab::VertexFormat(1.8 + 0, 0, 0, 0, 0, 1, 0.6f, 0.51f));	// stanga jos
			menuVer.push_back(lab::VertexFormat(1.8 + 1, 0, 0, 0, 0, 1, 0.69f, 0.51f));   // drapta jos
			menuVer.push_back(lab::VertexFormat(1.8 + 1, 1, 0, 0, 0, 1, 0.69f, 0.6f));   // drapta sus
			menuVer.push_back(lab::VertexFormat(1.8 + 0, 1, 0, 0, 0, 1, 0.6f, 0.6f));   // stanga sus
			// primu triunghi
			menuInd.push_back(8 + 0);
			menuInd.push_back(8 + 1);
			menuInd.push_back(8 + 2);
			// al 2lea triunghi
			menuInd.push_back(8 + 0);
			menuInd.push_back(8 + 2);
			menuInd.push_back(8 + 3);
			//i
			menuVer.push_back(lab::VertexFormat(2.7 + 0, 0, 0, 0, 0, 1, 0.1f, 0.51f));	// stanga jos
			menuVer.push_back(lab::VertexFormat(2.7 + 0.3, 0, 0, 0, 0, 1, 0.13f, 0.51f));   // drapta jos
			menuVer.push_back(lab::VertexFormat(2.7 + 0.3, 1, 0, 0, 0, 1, 0.13f, 0.6f));   // drapta sus
			menuVer.push_back(lab::VertexFormat(2.7 + 0, 1, 0, 0, 0, 1, 0.1f, 0.6f));   // stanga sus
			// primu triunghi
			menuInd.push_back(12 + 0);
			menuInd.push_back(12 + 1);
			menuInd.push_back(12 + 2);
			// al 2lea triunghi
			menuInd.push_back(12 + 0);
			menuInd.push_back(12 + 2);
			menuInd.push_back(12 + 3);
			//u
			menuVer.push_back(lab::VertexFormat(3.2 + 0, 0, 0, 0, 0, 1, 0.3f, 0.41f));	// stanga jos
			menuVer.push_back(lab::VertexFormat(3.2 + 1, 0, 0, 0, 0, 1, 0.39f, 0.41f));   // drapta jos
			menuVer.push_back(lab::VertexFormat(3.2 + 1, 1, 0, 0, 0, 1, 0.39f, 0.5f));   // drapta sus
			menuVer.push_back(lab::VertexFormat(3.2 + 0, 1, 0, 0, 0, 1, 0.3f, 0.5f));   // stanga sus
			// primu triunghi
			menuInd.push_back(16 + 0);
			menuInd.push_back(16 + 1);
			menuInd.push_back(16 + 2);
			// al 2lea triunghi
			menuInd.push_back(16 + 0);
			menuInd.push_back(16 + 2);
			menuInd.push_back(16 + 3);
			createChar(menuVer, menuInd, menu_vao, menu_vbo, menu_ibo, menu_num_indices);

			//desenare wireframe
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


			//lumina & material
			eye_position = glm::vec3(0, 0, 20);
			light_position = camera.getPosition();
			material_shininess = 30;
			material_kd = 0.5;
			material_ks = 0.5;
		}

		//destructor .. e apelat cand e distrusa clasa
		~Laborator(){
			//distruge shader
			glDeleteProgram(gl_program_shader);

			glDeleteBuffers(1,&cube_vbo);	glDeleteBuffers(1,&cube_ibo);	glDeleteVertexArrays(1,&cube_vao);		
			glDeleteBuffers(1, &lit_vbo);	glDeleteBuffers(1, &lit_ibo);	glDeleteVertexArrays(1, &lit_vao);
			glDeleteBuffers(1, &menu_vbo);	glDeleteBuffers(1, &menu_ibo);	glDeleteVertexArrays(1, &menu_vao);
		}


		void computePerspectiveProjection()
		{
			projectionMatrix = glm::perspective(FoV, aspectRatio, zNear, zFar);
			projectionMatrix2 = glm::perspective(FoV, aspectRatio, zNear, zFar);
		}

		void createChar(std::vector<lab::VertexFormat> vertices, std::vector<unsigned int> indices, unsigned int &vao, unsigned int& vbo, unsigned int &ibo, unsigned int &num_indices) {
			//creeaza obiectele OpenGL necesare desenarii
			unsigned int gl_vertex_array_object, gl_vertex_buffer_object, gl_index_buffer_object;

			//vertex array object -> un obiect ce reprezinta un container pentru starea de desenare
			glGenVertexArrays(1, &gl_vertex_array_object);
			glBindVertexArray(gl_vertex_array_object);

			//vertex buffer object -> un obiect in care tinem vertecsii
			glGenBuffers(1, &gl_vertex_buffer_object);
			glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_object);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(lab::VertexFormat), &vertices[0], GL_STATIC_DRAW);

			//index buffer object -> un obiect in care tinem indecsii
			glGenBuffers(1, &gl_index_buffer_object);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_index_buffer_object);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			//legatura intre atributele vertecsilor si pipeline, datele noastre sunt INTERLEAVED.
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)0);						//trimite pozitii pe pipe 0
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)(sizeof(float)* 3));		//trimite normale pe pipe 1
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)(2 * sizeof(float)* 3));	//trimite texcoords pe pipe 2

			vao = gl_vertex_array_object;
			vbo = gl_vertex_buffer_object;
			ibo = gl_index_buffer_object;
			num_indices = indices.size();
		}

		// Called every frame to draw
		void treatInput()
		{
			unsigned int time = glutGet(GLUT_ELAPSED_TIME);
			frameTime = time - previousTime;
			previousTime = time;

			float moveSpeed = frameTime / 20.0f;
			float rotateSpeed = frameTime / 20.0f;

			// Camera Translation
			if (keyState['w']) { camera.translateForward(moveSpeed); }
			if (keyState['a']) { camera.translateRight(-moveSpeed); }
			if (keyState['s']) { camera.translateForward(-moveSpeed); }
			if (keyState['d']) { camera.translateRight(moveSpeed); }
			if (keyState['e']) { camera.translateUpword(moveSpeed); }
			if (keyState['q']) { camera.translateUpword(-moveSpeed); }

			// Camera Rotate FPS
			if (specialKeyState[GLUT_KEY_LEFT]) { camera.rotateFPS_OY(rotateSpeed); }
			if (specialKeyState[GLUT_KEY_RIGHT]) { camera.rotateFPS_OY(-rotateSpeed); }

			if (specialKeyState[GLUT_KEY_UP]) { camera.rotateFPS_OX(rotateSpeed); }
			if (specialKeyState[GLUT_KEY_DOWN]) { camera.rotateFPS_OX(-rotateSpeed); }
		}


		//--------------------------------------------------------------------------------------------
		//functii de cadru ---------------------------------------------------------------------------

		//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
		void notifyBeginFrame(){
			//rotatie
			static float angle=0;
			angle = 0.1f;

			treatInput();
		}
		//functia de afisare (lucram cu banda grafica)
		void notifyDisplayFrame()
		{

			//pe tot ecranul
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	
			//foloseste shaderul
			glUseProgram(gl_program_shader);
			glUniform1i(glGetUniformLocation(gl_program_shader, "do_discard"), 0);
			glUniform1i(glGetUniformLocation(gl_program_shader, "has_alpha"), 0);
			glBindVertexArray(mesh_vao_ground);
			
			//lumina
			light_position = camera.getPosition();
			glUniform3f(glGetUniformLocation(gl_program_shader, "light_position"), light_position.x, light_position.y, light_position.z);
			glUniform3f(glGetUniformLocation(gl_program_shader, "forward_position"), camera.getForward().x, camera.getForward().y, camera.getForward().z);
			glUniform3f(glGetUniformLocation(gl_program_shader, "eye_position"), eye_position.x, eye_position.y, eye_position.z);
			glUniform1i(glGetUniformLocation(gl_program_shader, "material_shininess"), material_shininess);
			glUniform1f(glGetUniformLocation(gl_program_shader, "material_kd"), material_kd);
			glUniform1f(glGetUniformLocation(gl_program_shader, "material_ks"), material_ks);
			//lumina

			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "view_matrix"), 1, false, glm::value_ptr(view_matrix));
			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "projection_matrix"), 1, false, glm::value_ptr(projection_matrix));
			

			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "model_matrix"), 1, false, glm::value_ptr(model_matrix_menu));
			glUniform1i(glGetUniformLocation(gl_program_shader, "has_alpha"), 1);
			glActiveTexture(GL_TEXTURE0 + 2); // pt alpha
			glBindTexture(GL_TEXTURE_2D, lit_texture_alpha);
			glUniform1i(glGetUniformLocation(gl_program_shader, "textura2"), 2); // trimitem la shader unitatea de text
			glActiveTexture(GL_TEXTURE0 + 3);//pt acolor
			glBindTexture(GL_TEXTURE_2D, lit_texture_color);
			glUniform1i(glGetUniformLocation(gl_program_shader, "textura1"), 3); // trimitem la shader unitatea de text
			glBindVertexArray(menu_vao);
			glDrawElements(GL_TRIANGLES, menu_num_indices, GL_UNSIGNED_INT, 0);
			//change viewport
			////////////////////////////////
			glViewport(0, 0, 800, 600);
			//trimite variabile uniforme la shader
			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"view_matrix"),1,false,glm::value_ptr(camera.getViewMatrix()));
			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"projection_matrix"),1,false,glm::value_ptr(projection_matrix));
			glUniform1i(glGetUniformLocation(gl_program_shader, "do_discard"), 0);
			//cube----------------------------
			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"model_matrix"),1,false,glm::value_ptr(model_matrix_cube));

			// TODO
			// Bind generated cube texture
			glUniform1i(glGetUniformLocation(gl_program_shader,"has_alpha"), 0);
			glActiveTexture(GL_TEXTURE0 + 1); // 1 nr unitatii de texturare pt pamant
			glBindTexture(GL_TEXTURE_2D, cube_texture);
			glUniform1i(glGetUniformLocation(gl_program_shader,"textura1"), 1); // trimitem la shader unitatea de text
			glBindVertexArray(cube_vao);
			glDrawElements(GL_TRIANGLES, cube_num_indices, GL_UNSIGNED_INT, 0);
			
			// CHAR--------------------------
			if (!last_offx.empty()){
				glUniformMatrix4fv(glGetUniformLocation(gl_program_shader, "model_matrix"), 1, false, glm::value_ptr(model_matrix_lit));

				glUniform1i(glGetUniformLocation(gl_program_shader, "has_alpha"), 1);
				glActiveTexture(GL_TEXTURE0 + 2); // pt alpha
				glBindTexture(GL_TEXTURE_2D, lit_texture_alpha);
				glUniform1i(glGetUniformLocation(gl_program_shader, "textura2"), 2); // trimitem la shader unitatea de text
				glActiveTexture(GL_TEXTURE0 + 3);//pt acolor
				glBindTexture(GL_TEXTURE_2D, lit_texture_color);
				glUniform1i(glGetUniformLocation(gl_program_shader, "textura1"), 3); // trimitem la shader unitatea de text
			}
			if (animation == 1 && discardability == 0) {
				for (int i = 0; i < vertices.size(); i++) {
					if (phase[i/4] == 0) {
						vertices[i].position_x -= 0.005;
						if (vertices[i].position_x < -11)
							phase[i/4] = 1;
					}
					else if (phase[i/4] == 1) {
						vertices[i].position_z -= 0.005;
						if (vertices[i].position_z < -10.3)
							phase[i / 4] = 2;
					}
					else if (phase[i/4] == 2) {
						vertices[i].position_x += 0.005;
						if (vertices[i].position_x > 11)
							phase[i / 4] = 3;
					}
					else if (phase[i/4] == 3) {
						vertices[i].position_z += 0.005;
						if (vertices[i].position_z > 10.5)
							phase[i / 4] = 0;
					}
				}
				createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
			}
			else if (animation == 1 && discardability == 1) {
				glUniform1i(glGetUniformLocation(gl_program_shader, "do_discard"), 1);
				for (int i = 0; i < vertices.size()/4; i++) {
						vertices[i * 4].position_x -= 0.005;
						vertices[i * 4 + 1].position_x -= 0.005;
						vertices[i * 4 + 2].position_x -= 0.005;
						vertices[i * 4 + 3].position_x -= 0.005;
						if (vertices[i * 4].position_x < (-12.5 - (vertices.size() / 4))){
							vertices[i * 4].position_x += 30 + vertices.size()/4;
							vertices[i * 4 + 1].position_x += 30 + vertices.size() / 4;
							vertices[i * 4 + 2].position_x += 30 + vertices.size() / 4;
							vertices[i * 4 + 3].position_x += 30 + vertices.size() / 4;
						}
							
					}
				createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);		
			}

			glBindVertexArray(lit_vao);
			glDrawElements(GL_TRIANGLES, lit_num_indices, GL_UNSIGNED_INT, 0);


		}

		//functie chemata dupa ce am terminat cadrul de desenare (poate fi folosita pt modelare/simulare)
		void notifyEndFrame(){}

		//functei care e chemata cand se schimba dimensiunea ferestrei initiale
		void notifyReshape(int width, int height, int previos_width, int previous_height){
			//reshape
			if(height==0) height=1;
			float aspect = (float)width / (float)height;
			projection_matrix = glm::perspective(75.0f, aspect, 0.1f, 10000.0f);

		}


		//--------------------------------------------------------------------------------------------
		//functii de input output --------------------------------------------------------------------
	
		//tasta apasata
		void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y)
		{
			if (edit_enable == false) {
				keyState[key_pressed] = 1;
			}
			if(key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
			if(key_pressed == 32) {
				//SPACE reincarca shaderul si recalculeaza locatiile (xi/pointeri)
				glDeleteProgram(gl_program_shader);
				gl_program_shader = lab::loadShader("shadere\\shader_vertex.glsl", "shadere\\shader_fragment.glsl");
			}
			if(key_pressed == '1'){
				static bool wire =true;
				wire=!wire;
				glPolygonMode(GL_FRONT_AND_BACK, (wire?GL_LINE:GL_FILL));
			}

			if (key_pressed == 'a') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.3f, 0.61f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.39f, 0.61f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.39f, 0.7f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.3f, 0.7f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;
					// modifica  vertecsi
					last_offx.push_back(0.9f);
					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'b') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.4f, 0.61f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.47f, 0.61f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.47f, 0.7f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.4f, 0.7f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			
			if (key_pressed == 'c') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.5f, 0.61f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.59f, 0.61f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.59f, 0.7f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.5f, 0.7f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'd') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.6f, 0.61f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.69f, 0.61f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.69f, 0.7f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.6f, 0.7f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'e') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.7f, 0.61f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.77f, 0.61f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.77f, 0.7f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.7f, 0.7f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'f') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.8f, 0.61f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.87f, 0.61f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.87f, 0.7f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.8f, 0.7f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'g') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.9f, 0.61f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.99f, 0.61f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.99f, 0.7f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.9f, 0.7f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'h') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.0f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.09f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.09f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.0f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'i') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.1f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 0.3, 0, offsetz, 0, 0, 1, 0.13f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 0.3, 1, offsetz, 0, 0, 1, 0.13f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.1f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'j') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.2f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 0.6, 0, offsetz, 0, 0, 1, 0.24f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 0.6, 1, offsetz, 0, 0, 1, 0.24f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.2f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'k') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.3f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.39f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.39f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.3f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'l') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.4f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.49f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.49f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.4f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'm') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.5f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.595f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.595f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.5f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'n') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.6f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.69f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.69f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.6f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'o') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.7f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.79f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.79f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.7f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'p') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.8f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.88f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.88f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.8f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'q') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.9f, 0.51f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.99f, 0.51f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.99f, 0.6f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.9f, 0.6f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'r') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.0f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.09f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.09f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.0f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 's') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.1f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.19f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.19f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.1f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 't') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.2f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.29f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.29f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.2f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'u') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.3f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.39f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.39f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.3f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'v') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.4f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.49f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.49f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.4f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'w') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.5f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.59f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.59f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.5f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'x') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.6f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.69f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.69f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.6f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'y') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.7f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.79f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.79f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.7f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 'z') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.8f, 0.41f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.89f, 0.41f));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.89f, 0.5f));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.8f, 0.5f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == ' ') {
				if (edit_enable == true) {
					if (vertices.empty()){
						offsetx = 0;
						offsetz = 10.5;
						phase.push_back(0);
					}
					else {
						phase.push_back(phase.back());
						if (phase.back() == 0)
						{
							offsetx = vertices.back().position_x + 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 1)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z + 0.9f;
						}
						else if (phase.back() == 2)
						{
							offsetx = vertices.back().position_x - 0.9f;
							offsetz = vertices.back().position_z;
						}
						else if (phase.back() == 3)
						{
							offsetx = vertices.back().position_x;
							offsetz = vertices.back().position_z - 0.9f;
						}
					}
					vertices.push_back(lab::VertexFormat(offsetx + 0, 0, offsetz, 0, 0, 1, 0.0f, 0.91f));	// stanga jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 0, offsetz, 0, 0, 1, 0.09f, 1));   // drapta jos
					vertices.push_back(lab::VertexFormat(offsetx + 1, 1, offsetz, 0, 0, 1, 0.09f, 1));   // drapta sus
					vertices.push_back(lab::VertexFormat(offsetx + 0, 1, offsetz, 0, 0, 1, 0.0f, 0.91f));   // stanga sus
					// primu triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 1);
					indices.push_back(ind_offset + 2);
					// al 2lea triunghi
					indices.push_back(ind_offset + 0);
					indices.push_back(ind_offset + 2);
					indices.push_back(ind_offset + 3);
					// modifica  indici
					ind_offset += 4;

					createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
				}
			}
			if (key_pressed == 8) {
				if (edit_enable == true) {
					if (!vertices.empty()) {
						vertices._Pop_back_n(4);
						indices._Pop_back_n(6);
						// modifica  indici
						ind_offset -= 4;
						// modifica  vertecsi
						if (!vertices.empty())
							createChar(vertices, indices, lit_vao, lit_vbo, lit_ibo, lit_num_indices);
					}
				}
			}

		}
		//tasta ridicata
		void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){
			keyState[key_released] = 0;
		}

		//tasta speciala (up/down/F1/F2..) apasata
		void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
			specialKeyState[key_pressed] = 1;
			if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
			if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
			// edit enable
			if (key_pressed == GLUT_KEY_F8) {
				if (edit_enable == false) 
					edit_enable = true;
				else 
					edit_enable = false;
			}
			// animation
			if (key_pressed == GLUT_KEY_F9) {
				animation ^= 1;
			}
			// discard one face only
			if (key_pressed == GLUT_KEY_F10) {
				discardability ^= 1;
			}
		}
		//tasta speciala ridicata
		void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){
			specialKeyState[key_released] = 0;
		}
		//drag cu mouse-ul
		void notifyMouseDrag(int mouse_x, int mouse_y){ }
		//am miscat mouseul (fara sa apas vreun buton)
		void notifyMouseMove(int mouse_x, int mouse_y){ }
		//am apasat pe un boton
		void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ }
		//scroll cu mouse-ul
		void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ }

};

int main()
{
	//initializeaza GLUT (fereastra + input + context OpenGL)
	lab::glut::WindowInfo window(std::string("lab EGC 8 - texturi"),800,600,100,100,true);
	lab::glut::ContextInfo context(3,3,false);
	lab::glut::FramebufferInfo framebuffer(true,true,true,true);
	lab::glut::init(window,context, framebuffer);

	//initializeaza GLEW (ne incarca functiile openGL, altfel ar trebui sa facem asta manual!)
	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	//creem clasa noastra si o punem sa asculte evenimentele de la GLUT
	//DUPA GLEW!!! ca sa avem functiile de OpenGL incarcate inainte sa ii fie apelat constructorul (care creeaza obiecte OpenGL)
	Laborator mylab;
	lab::glut::setListener(&mylab);

	//taste
	std::cout<<"Taste:"<<std::endl<<"\tESC ... iesire"<<std::endl<<"\tSPACE ... reincarca shadere"<<std::endl<<"\tw ... toggle wireframe"<<std::endl;

	//run
	lab::glut::run();

	return 0;
}