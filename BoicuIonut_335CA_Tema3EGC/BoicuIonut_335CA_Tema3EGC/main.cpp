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

// camera
#include "lab_camera.hpp"

//time
#include <ctime>


class Laborator : public lab::glut::WindowListener{

//variabile
private:
	glm::mat4 model_matrix, view_matrix, projection_matrix;										//matrici 4x4 pt modelare vizualizare proiectie
	unsigned int gl_program_shader_gouraud;														//id obiect shader gouraud

	unsigned int mesh_vbo_model, mesh_ibo_model, mesh_vao_model, mesh_num_indices_model;		//containere opengl pentru vertecsi, indecsi si stare pentru modelul incarcat

	unsigned int mesh_vbo_sphere, mesh_ibo_sphere, mesh_vao_sphere, mesh_num_indices_sphere;	//containere opengl pentru vertecsi, indecsi si stare pentru sfera ce va fi pusa la pozitia luminii

	unsigned int mesh_vbo_ground, mesh_ibo_ground, mesh_vao_ground, mesh_num_indices_ground;	//containere opengl pentru vertecsi, indecsi si stare pentru sol

	glm::vec3 light_position;
	unsigned int material_shininess;
	float material_kd;
	float material_ks;
	float timp, faza, amplitudine, frecventa;
	glm::vec3 directie, eye_position;
	glm::vec3 centru;
	int indecsi, directional, circular, drawable;
	std::vector<lab::VertexFormat> ver;
	std::vector<glm::uvec3> ind;
	
	lab::Camera FPS;


//metode
public:
	
	//constructor .. e apelat cand e instantiata clasa
	Laborator(){
		//setari pentru desenare, clear color seteaza culoarea de clear pentru ecran (format R,G,B,A)
		glClearColor(0.8,0.8,0.8,1);
		glClearDepth(1);			//clear depth si depth test (nu le studiem momentan, dar avem nevoie de ele!)
		glEnable(GL_DEPTH_TEST);	//sunt folosite pentru a determina obiectele cele mai apropiate de camera (la curs: algoritmul pictorului, algoritmul zbuffer)
		
		//initializari
		timp = 0.0f;
		amplitudine = 1.5f;
		frecventa = glm::pi<float>() /15;  // 2pi/lungime   ---    lungime e 30
		faza = frecventa /2 ;// viteza * frecventa ---- viteza = 0.5
		directional = 0; circular = 0; // unde
		directie = glm::vec3(1, 0, 1);
		centru.x = 150;
		centru.z =  10;
		drawable = 0;
		//incarca un shader din fisiere si gaseste locatiile matricilor relativ la programul creat
		gl_program_shader_gouraud = lab::loadShader("shadere\\shader_gouraud_vertex.glsl", "shadere\\shader_gouraud_fragment.glsl");

		//incarca o sfera
		lab::loadObj("resurse\\sphere.obj",mesh_vao_sphere, mesh_vbo_sphere, mesh_ibo_sphere, mesh_num_indices_sphere);	

		//incarca un plan
		//lab::loadObj("resurse\\plane.obj",mesh_vao_ground, mesh_vbo_ground, mesh_ibo_ground, mesh_num_indices_ground);	
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				ver.push_back(lab::VertexFormat(i, 0, -j, 0, 1, 0));
			}
		}
		for (int i = 0; i < 99; i++) {
			for (int j = 0; j < 99; j++) {
				ind.push_back(glm::uvec3(i * 100 + j, (i + 1) * 100 + j, (i + 1) * 100 + j + 1));
				ind.push_back(glm::uvec3(i * 100 + j, (i + 1) * 100 + j + 1, i * 100 + j + 1));
			}
		}
		// create grid
		createGrid();

		//lumina & material
		eye_position = glm::vec3(50, 25, 0);
		FPS.set(glm::vec3(50, 25, 0), glm::vec3(50, 0, -15), glm::vec3(0, 1, 0));
		
		light_position = glm::vec3(50,10,-50);
		material_shininess = 30;
		material_kd = 0.5;
		material_ks = 0.5;

		//matrici de modelare si vizualizare
		model_matrix = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

		//desenare wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//destructor .. e apelat cand e distrusa clasa
	~Laborator(){
		//distruge shader
		glDeleteProgram(gl_program_shader_gouraud);

		//distruge mesh incarcat
		glDeleteBuffers(1,&mesh_vbo_model);
		glDeleteBuffers(1,&mesh_ibo_model);
		glDeleteVertexArrays(1,&mesh_vao_model);

		//distruge sfera
		glDeleteBuffers(1,&mesh_vbo_sphere);
		glDeleteBuffers(1,&mesh_ibo_sphere);
		glDeleteVertexArrays(1,&mesh_vao_sphere);

		//distruge solul
		glDeleteBuffers(1,&mesh_vbo_ground);
		glDeleteBuffers(1,&mesh_ibo_ground);
		glDeleteVertexArrays(1,&mesh_vao_ground);
	}


	//--------------------------------------------------------------------------------------------
	//functii de cadru ---------------------------------------------------------------------------

	//creare plan
	void createGrid() {
		//creaza vao
		glGenVertexArrays(1, &mesh_vao_ground);
		glBindVertexArray(mesh_vao_ground);

		//creeaza vbo
		glGenBuffers(1, &mesh_vbo_ground);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_ground);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lab::VertexFormat)*ver.size(), &ver[0], GL_STATIC_DRAW);


		//creeaza ibo
		glGenBuffers(1, &mesh_ibo_ground);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo_ground);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*ind.size() * 3, &ind[0], GL_STATIC_DRAW);
		indecsi = ind.size() * 3;

		//legatura intre atributele vertecsilor si pipeline, datele noastre sunt INTERLEAVED.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)0);						//trimite pozitii pe pipe 0
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)(sizeof(float)* 3));		//trimite normale pe pipe 1
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)(2 * sizeof(float)* 3));	//trimite texcoords pe pipe 2

	}

	//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
	void notifyBeginFrame(){
		//rotatie lumina Oy
		//float angle =0.00005f;
		//light_position.x = lp.x*cos(angle)-lp.z*sin(angle);
		//light_position.z = lp.x*sin(angle)+lp.z*cos(angle);

		timp += 0.0025f;
		
	}
	//functia de afisare (lucram cu banda grafica)
	void notifyDisplayFrame(){
		//bufferele din framebuffer sunt aduse la valorile initiale (setate de clear color si cleardepth)
		//adica se sterge ecranul si se pune culoare (si alte propietati) initiala
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//foloseste shaderul
		glUseProgram(gl_program_shader_gouraud);
				
		//trimite variabile uniforme la shader
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader_gouraud, "model_matrix"),1,false,glm::value_ptr(model_matrix));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader_gouraud, "view_matrix"),1,false,glm::value_ptr(view_matrix));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader_gouraud, "projection_matrix"),1,false,glm::value_ptr(projection_matrix));
		glUniform3f(glGetUniformLocation(gl_program_shader_gouraud, "light_position"),light_position.x, light_position.y, light_position.z);
		glUniform3f(glGetUniformLocation(gl_program_shader_gouraud, "eye_position"),eye_position.x, eye_position.y, eye_position.z);
		glUniform1i(glGetUniformLocation(gl_program_shader_gouraud, "material_shininess"),material_shininess);
		glUniform1f(glGetUniformLocation(gl_program_shader_gouraud, "material_kd"),material_kd);
		glUniform1f(glGetUniformLocation(gl_program_shader_gouraud, "material_ks"),material_ks);
		glUniform1f(glGetUniformLocation(gl_program_shader_gouraud, "timp"), timp);
		glUniform1f(glGetUniformLocation(gl_program_shader_gouraud, "amplitudine"), amplitudine);
		glUniform1f(glGetUniformLocation(gl_program_shader_gouraud, "faza"), faza);
		glUniform1f(glGetUniformLocation(gl_program_shader_gouraud, "frecventa"), frecventa);
		glUniform2f(glGetUniformLocation(gl_program_shader_gouraud, "directie"), directie.x,directie.z);
		glUniform2f(glGetUniformLocation(gl_program_shader_gouraud, "centru"), centru.x, centru.z);
		glUniform1i(glGetUniformLocation(gl_program_shader_gouraud, "directional"), directional);
		glUniform1i(glGetUniformLocation(gl_program_shader_gouraud, "circular"), circular);
		glUniform1i(glGetUniformLocation(gl_program_shader_gouraud, "toDraw"), drawable);
		//pune si un sol
		//matrice_translatie = glm::translate(model_matrix, glm::vec3(0,0,0));
		//glUniformMatrix4fv(glGetUniformLocation(gl_program_shader_gouraud, "model_matrix"),1,false,glm::value_ptr(matrice_translatie));

		glBindVertexArray(mesh_vao_ground);
		glDrawElements(GL_TRIANGLES, indecsi, GL_UNSIGNED_INT, 0);
		drawable = 1;
		glUniform1i(glGetUniformLocation(gl_program_shader_gouraud, "toDraw"), drawable);
		//pune o sfera la pozitia luminii
		glm::mat4 matrice_translatie = glm::translate(model_matrix, glm::vec3(light_position.x,light_position.y,light_position.z));
		glm::mat4 matrice_scalare = glm::scale(model_matrix, glm::vec3(0.1,0.1,0.1));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader_gouraud, "model_matrix"),1,false,glm::value_ptr(matrice_translatie * matrice_scalare));
		
		glBindVertexArray(mesh_vao_sphere);
		glDrawElements(GL_TRIANGLES, mesh_num_indices_sphere, GL_UNSIGNED_INT, 0);
		drawable = 0;
		// camera
		view_matrix = FPS.getViewMatrix();

		
	}
	//functie chemata dupa ce am terminat cadrul de desenare (poate fi folosita pt modelare/simulare)
	void notifyEndFrame(){}
	//functei care e chemata cand se schimba dimensiunea ferestrei initiale
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//reshape
		if(height==0) height=1;
		glViewport(0,0,width,height);
		projection_matrix = glm::perspective(90.0f, (float)width/(float)height,0.1f, 10000.0f);
	}


	//--------------------------------------------------------------------------------------------
	//functii de input output --------------------------------------------------------------------
	
	//tasta apasata
	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
		if(key_pressed == 32) {	//SPACE
			//reincarca shader
			glDeleteProgram(gl_program_shader_gouraud);
			gl_program_shader_gouraud = lab::loadShader("shadere\\shader_gouraud_vertex.glsl", "shadere\\shader_gouraud_fragment.glsl");
		}
		// grid view
		if(key_pressed == 'g'){
			static bool wire =true;
			wire=!wire;
			glPolygonMode(GL_FRONT_AND_BACK, (wire?GL_LINE:GL_FILL));
		}
		// directional
		if (key_pressed == '1') {
			directional ^= 1;
		}
		// circular
		if (key_pressed == '2') {
			circular ^= 1;
		}
		// inainte
		if (key_pressed == 'w') {
			FPS.translateForward(1);
			eye_position = FPS.getPosition();
		}
		// inapoi 
		if (key_pressed == 's') {
			FPS.translateForward(-1);
			eye_position = FPS.getPosition();
		}
		// rotire stanga
		if (key_pressed == 'a') {
			FPS.rotateFPS_OY(5);
			eye_position = FPS.getPosition();
		}
		// rotire dreapta
		if (key_pressed == 'd') {
			FPS.rotateFPS_OY(-5);
			eye_position = FPS.getPosition();
		}
		// rotire sus
		if (key_pressed == 'q') {
			FPS.rotateFPS_OX(-5);
			eye_position = FPS.getPosition();
		}
		// rotire jos
		if (key_pressed == 'e') {
			FPS.rotateFPS_OX(5);
			eye_position = FPS.getPosition();
		}
		// rotire oz
		if (key_pressed == 'z') {
			FPS.rotateFPS_OZ(-5);
			eye_position = FPS.getPosition();
		}
		// rotire oz
		if (key_pressed == 'c') {
			FPS.rotateFPS_OZ(5);
			eye_position = FPS.getPosition();
		}

	}
	//tasta ridicata
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){	}
	//tasta speciala (up/down/F1/F2..) apasata
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
		if (key_pressed == GLUT_KEY_RIGHT) {
			light_position.x += 1;
		}
		if (key_pressed == GLUT_KEY_LEFT) {
			light_position.x -= 1;
		}
		if (key_pressed == GLUT_KEY_UP) {
			light_position.z -= 1;
		}
		if (key_pressed == GLUT_KEY_DOWN) {
			light_position.z += 1;
		}
		if (key_pressed == GLUT_KEY_F5) {
			light_position.y += 1;
		}
		if (key_pressed == GLUT_KEY_F6) {
			light_position.y -= 1;
		}
	}
	//tasta speciala ridicata
	void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){}
	//drag cu mouse-ul
	void notifyMouseDrag(int mouse_x, int mouse_y){ }
	//am miscat mouseul (fara sa apas vreun buton)
	void notifyMouseMove(int mouse_x, int mouse_y){ }
	//am apasat pe un boton
	void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ }
	//scroll cu mouse-ul
	void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ std::cout<<"Mouse scroll"<<std::endl;}

};

int main(){
	//initializeaza GLUT (fereastra + input + context OpenGL)
	lab::glut::WindowInfo window(std::string("EGC - lab 6 - iluminare Gouraud"),800,600,100,100,true);
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