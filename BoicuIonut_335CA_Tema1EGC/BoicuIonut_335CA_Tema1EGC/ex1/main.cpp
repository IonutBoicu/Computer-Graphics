//-------------------------------------------------------------------------------------------------
// Descriere: fisier main
//
// Autor: student
// Data: today
//-------------------------------------------------------------------------------------------------

//librarie matematica 
#include "..\\dependente\glew\glew.h"
#include "..\\dependente\glm\glm.hpp"
#include "..\\dependente\glm\gtc\type_ptr.hpp"
#include "..\\dependente\glm\gtc\matrix_transform.hpp"

//neimportant pentru acest laborator, va fi explicat in viitor
#include "..\\lab_blackbox.hpp"

//interfata cu glut, ne ofera fereastra, input, context opengl
#include "..\\lab_glut.hpp"

//time
#include <ctime>
#include <cmath>
#include <iostream>
#include <math.h>
#include <algorithm>

//clasa lab, mosteneste din WindowListener
class Lab : public lab::glut::WindowListener{

//variabile
private:
	//un obiect ce ascunde functii pe care vom ajunge sa le scriem de mana.
	lab::BlackBox BLACKBOX;
	//un obiect ce va fi desenat (nu ne intereseaza propietatile sale momentan)
	//lab::Polyline
	lab::Mesh	 *cadru, *lumina, *camera, *obiecte[10], *raza, *scena, *Reflexii[30];
	float xCentru, yCentru, xc1, xc2, yRaza, dirX, dirY, cul[10][3], culReflexii[30][3], coord[10][2], dim[10][2], yUltim;
	int razeTotale = -1, idx1, idx2, idx3, idx4;
	unsigned int width, height;
	bool cameraHit;

	struct MyVertexFormat{
		glm::vec2 pozitie;
		MyVertexFormat(){
			pozitie = glm::vec2(0, 0);
		}
		MyVertexFormat(float px, float py){
			pozitie = glm::vec2(px, py);
		}
		MyVertexFormat operator=(const MyVertexFormat &rhs){
			pozitie = rhs.pozitie;
			return (*this);
		}
	};

	glm::vec2 dirVect;
	glm::mat3 transformation[3];
	std::vector<glm::uvec3> indecsi, indReflexii[30];
	std::vector<MyVertexFormat> vertecsi, verReflexii[30];
	
//metode ale clasei lab
public:
	//constructor .. e apelat cand e instantiata clasa
	Lab(){
		dirX = -1; dirY = -1;
		dirVect = glm::normalize(glm::vec2(dirX, dirY));
		// lumina		
		lumina = myLoadSquare_as_Mesh(400, 200, 50, 50);
		//centrul luminalui
		xCentru = 425; yCentru = 225; 
		// coordonatele initiala pentru raza
		xc1 = xCentru - 5; xc2 = xCentru + 5;
		yRaza = yCentru - 5; yUltim = yRaza - 1;
		idx1 = 0; idx2 = 1; idx3 = 2; idx4 = 3;

		std::vector<glm::uvec3> ind;
		std::vector<MyVertexFormat> ver;
		// Raza
		vertecsi.push_back(MyVertexFormat(xc1, yRaza));
		vertecsi.push_back(MyVertexFormat(xc2, yRaza));
		vertecsi.push_back(MyVertexFormat(xc2, yUltim));
		vertecsi.push_back(MyVertexFormat(xc1, yUltim));
		indecsi.push_back(glm::uvec3(idx1, idx2, idx3));
		indecsi.push_back(glm::uvec3(idx3, idx4, idx1));
		raza = myLoadObjectas_Mesh(vertecsi, indecsi);
		// dimensiuni fereastra
		width = lab::glut::getInitialWindowInformation().width;
		height = lab::glut::getInitialWindowInformation().height;
		// coordonate pentru obiecte
		obiecte[0] = myLoadSquare_as_Mesh(15, 15, 100, 100);
		obiecte[1] = myLoadSquare_as_Mesh(width - 55, 15, 200, 20);
		obiecte[2] = myLoadSquare_as_Mesh(225, height - 100, 45, 150);
		obiecte[3] = myLoadSquare_as_Mesh((width - 50) * 2 / 3, height - 175, 125, 40);
		obiecte[4] = myLoadSquare_as_Mesh(100, height / 2, 50, 75);
		obiecte[5] = myLoadSquare_as_Mesh(height / 2, 35, 25, 120);
		obiecte[6] = myLoadSquare_as_Mesh(35, height - 200, 150, 150);
		obiecte[7] = myLoadSquare_as_Mesh((width - 50) * 1 / 2 + 74, (height - 50) / 7, 100, 20);
		obiecte[8] = myLoadSquare_as_Mesh(width * 2 / 3, height / 10, 50, 150);
		obiecte[9] = myLoadSquare_as_Mesh(width * 2 / 3 + 15, height * 6 / 10, 150, 100);
		// salvam coordonatele si dimensiunea obiectelor pentru a realiza coliziunile mai usor
		coord[0][0] = 15; coord[0][1] = 15; dim[0][0] = 100; dim[0][1] = 100;
		coord[1][0] = width - 55; coord[1][1] = 15; dim[1][0] = 200; dim[1][1] = 20;
		coord[2][0] = 225; coord[2][1] = height - 100; dim[2][0] = 45; dim[2][1] = 150;
		coord[3][0] = (width - 50) * 2 / 3; coord[3][1] = height - 175; dim[3][0] = 125; dim[3][1] = 40;
		coord[4][0] = 100; coord[4][1] = height / 2; dim[4][0] = 50; dim[4][1] = 75;
		coord[5][0] = height / 2; coord[5][1] = 35; dim[5][0] = 25; dim[5][1] = 120;
		coord[6][0] = 35; coord[6][1] = height - 200; dim[6][0] = 150; dim[6][1] = 150;
		coord[7][0] = (width - 50) * 1 / 2 + 74; coord[7][1] = (height - 50) / 7; dim[7][0] = 100; dim[7][1] = 20;
		coord[8][0] = width * 2 / 3; coord[8][1] = height / 10; dim[8][0] = 50; dim[8][1] = 150;
		coord[9][0] = width * 2 / 3 + 15; coord[9][1] = height * 6 / 10; dim[9][0] = 150; dim[9][1] = 100;
		// coloram obiectele random
		srand(time(NULL));
		for (int i = 0; i < 10; i++) {
			cul[i][0] = rand() / (float)RAND_MAX;
			cul[i][1] = rand() / (float)RAND_MAX;
			cul[i][2] = rand() / (float)RAND_MAX;
		}
		//transformari pentru raza si cercuri 
		transformation[0] = glm::mat3(1);
		transformation[1] = glm::mat3(1);
		transformation[2] = glm::mat3(1);
		// camera 
		cameraHit = false;
		ver.push_back(MyVertexFormat(width-35,height-35));
		ver.push_back(MyVertexFormat(width-35,height-175));
		ver.push_back(MyVertexFormat(width - 175, height - 35));
		ind.push_back(glm::uvec3(0, 1, 2));
		camera = myLoadObjectas_Mesh(ver, ind);
		// cadru
		cadru = myLoadSquare_as_Mesh(5, 5, height - 30, width - 30);
		scena = myLoadSquare_as_Mesh(15, 15, height - 50, width - 50);
		printf("SCENA: 15, 15, %d, %d\n", height - 50, width - 50);
	}

	//destructor .. e apelat cand e distrusa clasa
	~Lab(){
		delete cadru;
		delete camera;
		delete lumina;
		delete raza;
		delete scena;
	}

	//gives ownership
	lab::Mesh* myLoadObjectas_Mesh(std::vector<MyVertexFormat> ver, std::vector<glm::uvec3> ind){
		//creaza vao
		unsigned int mesh_vao;
		glGenVertexArrays(1, &mesh_vao);
		glBindVertexArray(mesh_vao);

		//creeaza vbo
		unsigned int mesh_vbo;
		glGenBuffers(1, &mesh_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertexFormat)*ver.size(), &ver[0], GL_STATIC_DRAW);

		//creeaza ibo
		unsigned int mesh_ibo;
		glGenBuffers(1, &mesh_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*ind.size() * 3, &ind[0], GL_STATIC_DRAW);

		//creez obiect de tip mesh
		lab::Mesh* mesh = new lab::Mesh(mesh_vbo, mesh_ibo, mesh_vao, ind.size() * 3);

		// traducem la OpenGL cum sa foloseasca datele noastre
		BLACKBOX.bindMesh(mesh, sizeof(MyVertexFormat));

		//return
		return mesh;
	}
	lab::Mesh* myLoadSquare_as_Mesh(float x, float y, float l, float L){
		//definim containere pentru date
		std::vector<MyVertexFormat> vertecsi;
		std::vector<glm::uvec3> indecsi;

		//4 vertecsi (doar pozitii fara normale fara texcoords)
		vertecsi.push_back(MyVertexFormat(x, y));
		vertecsi.push_back(MyVertexFormat(x + L, y));
		vertecsi.push_back(MyVertexFormat(x + L, y + l));
		vertecsi.push_back(MyVertexFormat(x, y + l));
		
		//2 triunghiuri pentru 1 fata
		indecsi.push_back(glm::uvec3(0, 1, 2));	indecsi.push_back(glm::uvec3(2, 3, 0));

		//creaza vao
		unsigned int mesh_vao;
		glGenVertexArrays(1, &mesh_vao);
		glBindVertexArray(mesh_vao);

		//creeaza vbo
		unsigned int mesh_vbo;
		glGenBuffers(1, &mesh_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertexFormat)*vertecsi.size(), &vertecsi[0], GL_STATIC_DRAW);

		//creeaza ibo
		unsigned int mesh_ibo;
		glGenBuffers(1, &mesh_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indecsi.size() * 3, &indecsi[0], GL_STATIC_DRAW);

		//creez obiect de tip mesh
		lab::Mesh* mesh = new lab::Mesh(mesh_vbo, mesh_ibo, mesh_vao, indecsi.size() * 3);

		// traducem la OpenGL cum sa foloseasca datele noastre
		BLACKBOX.bindMesh(mesh, sizeof(MyVertexFormat));

		//return
		return mesh;
	}

	//--------------------------------------------------------------------------------------------
	//functii de transformare --------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------
	glm::mat3 myIdentity(){
		return glm::transpose(glm::mat3(	//de ce? pt ca in memorie se scriu coloane dar in IDE le vezi ca randuri
			1, 0, 0,						//asa le vezi ca randuri si le scrii ca randuri!
			0, 1, 0,
			0, 0, 1
			));
	}
	glm::mat3 myTranslate(float tx, float ty){
		return glm::transpose(glm::mat3(
			1, 0, tx,
			0, 1, ty,
			0, 0, 1
			));
	}

	glm::mat3 myRotate(float u){
		return glm::transpose(glm::mat3(
			cos(u), -sin(u), 0,
			sin(u), cos(u), 0,
			0, 0, 1
			));
	}

	glm::mat3 myScale(float sx, float sy) {
		return glm::transpose(glm::mat3(
			sx, 0, 0,
			0, sy, 0,
			0, 0, 1));
	}

	//--------------------------------------------------------------------------------------------
	//functii de cadru, apelate per FIECARE cadru ------------------------------------------------
	//--------------------------------------------------------------------------------------------
	//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
	void notifyBeginFrame(){}
	//functia de afisare
	void notifyDisplayFrame(){
		//per ECRAN
		//bufferele din framebuffer sunt aduse la valorile initiale (setate de clear color)
		//adica se sterge ecranul si se pune culoarea initiala (si alte propietati)
		glClearColor(1, 1, 1, 0);			//la ce culoare sterg
		glClear(GL_COLOR_BUFFER_BIT);		//comanda de stergere
		BLACKBOX.notifyDisplay();

		//viewport 1
		glViewport(0, 0, width, height);
		BLACKBOX.setModelMatrix(transformation[0]);
		// maxim 30 raze
		if (razeTotale < 30){
			//sau pana se atinge camera
			if (cameraHit == false) {
				//indecsii de randare
				idx1 += 4; idx2 += 4; idx3 += 4; idx4 += 4;
				//si punctele ultimului dreptunghi adaugat razei
				glm::vec2 P1(xc1, yUltim);
				glm::vec2 P2(xc2, yUltim);

				//parcurgere obiecte
				for (int i = 0; i < 10; i++) {
					// coordonatele fiecarui colt din obiect
					glm::vec2 A(coord[i][0], coord[i][1]);
					glm::vec2 B(coord[i][0] + dim[i][1], coord[i][1]);
					glm::vec2 C(coord[i][0] + dim[i][1], coord[i][1] + dim[i][0]);
					glm::vec2 D(coord[i][0], coord[i][1] + dim[i][0]);
					// verificare coliziune cu unul din peretii obiectului
					if (seCiocneste(A, B, C, D, P1) || seCiocneste(A, B, C, D, P2)) {
						// raza normala
						glm::vec2 normVect = glm::normalize(vectorvectorNormala(A, B, C, D, P1));
						// adaugare raza si culoarea ei
						razeTotale++;
						if (razeTotale == 0) {
							culReflexii[razeTotale][0] = 0.5 + cul[i][0] / 2;
							culReflexii[razeTotale][1] = 0.5 + cul[i][1] / 2;
							culReflexii[razeTotale][2] = cul[i][2] / 2;
						}
						else {
							culReflexii[razeTotale][0] = (culReflexii[razeTotale - 1][0] + cul[i][0]) / 2;
							culReflexii[razeTotale][1] = (culReflexii[razeTotale - 1][1] + cul[i][1]) / 2;
							culReflexii[razeTotale][2] = (culReflexii[razeTotale - 1][2] + cul[i][2]) / 2;
						}
						// directia razei rezultate
						dirVect = glm::normalize(glm::reflect(dirVect, normVect));
						break;
					}
				}
				// punctele cadrului
				glm::vec2 A(15, 15);
				glm::vec2 B(15 + width - 50, 15);
				glm::vec2 C(15 + width - 50, 15 + height - 50);
				glm::vec2 D(15, 15 + height - 50);
				// coliziune cu cadrul
				if (xc1 <= 15 || xc2 <= 15 || xc1 >= width + 15 - 50 || xc2 >= width + 15 - 50 || yUltim <= 15 || yUltim >= height + 15 - 50) {
					// normala
					glm::vec2 normVect = glm::normalize(vectorvectorNormala(A, B, C, D, P1));
					// adaugare raza si culoarea ei
					razeTotale++;
					if (razeTotale == 0) {
						culReflexii[razeTotale][0] = 0.65;
						culReflexii[razeTotale][1] = 0.65;
						culReflexii[razeTotale][2] = 0.65;
					}
					else {
						culReflexii[razeTotale][0] = (culReflexii[razeTotale - 1][0] + 0.3) / 2;
						culReflexii[razeTotale][1] = (culReflexii[razeTotale - 1][1] + 0.3) / 2;
						culReflexii[razeTotale][2] = (culReflexii[razeTotale - 1][2] + 0.3) / 2;
					}
					// directia razei rezultate
					dirVect = glm::normalize(glm::reflect(dirVect, normVect));
				}
				// punctele camerei
				glm::vec2 C1(width - 35, height - 35);
				glm::vec2 C2(width - 35, height - 175);
				glm::vec2 C3(width - 175, height - 35);
				// coliziune cu camera
				if (((arie(C1, C2, P1) + arie(C1, C3, P1) + arie(C2, C3, P1)) - arie(C1, C2, C3) <= 0) 
					|| ((arie(C1, C2, P2) + arie(C1, C3, P2) + arie(C2, C3, P2)) -arie(C1, C2, C3)  <= 0)){
					// s-a atins camera => nu se mai reflecta raza
					cameraHit = true;
				}
				// formarea razelor reflectate
				if (razeTotale >= 0) {
					if (indReflexii[razeTotale].size() == 0) {
						idx1 = 0; idx2 = 1; idx3 = 2; idx4 = 3;
					}
					verReflexii[razeTotale].push_back(MyVertexFormat(xc1, yUltim));
					verReflexii[razeTotale].push_back(MyVertexFormat(xc2, yUltim));
					verReflexii[razeTotale].push_back(MyVertexFormat(xc2 + dirVect.x, yUltim + dirVect.y));
					verReflexii[razeTotale].push_back(MyVertexFormat(xc1 + dirVect.x, yUltim + dirVect.y));
					indReflexii[razeTotale].push_back(glm::uvec3(idx1, idx2, idx3));
					indReflexii[razeTotale].push_back(glm::uvec3(idx3, idx4, idx1));
				}
				else {
					vertecsi.push_back(MyVertexFormat(xc1, yUltim));
					vertecsi.push_back(MyVertexFormat(xc2, yUltim));
					vertecsi.push_back(MyVertexFormat(xc2 + dirVect.x, yUltim + dirVect.y));
					vertecsi.push_back(MyVertexFormat(xc1 + dirVect.x, yUltim + dirVect.y));
					indecsi.push_back(glm::uvec3(idx1, idx2, idx3));
					indecsi.push_back(glm::uvec3(idx3, idx4, idx1));
				}
				yUltim += dirVect.y;
				xc1 += dirVect.x;
				xc2 += dirVect.x;
			}
		}
		// desenarea razei initiale
		raza = myLoadObjectas_Mesh(vertecsi, indecsi);
		raza->setColor(1, 1, 0);
		BLACKBOX.drawMesh(raza, 1);
		// desenarea tuturor razelor reflectate
		for (int j = 0; j < razeTotale+1; j++){
			Reflexii[j] = myLoadObjectas_Mesh(verReflexii[j], indReflexii[j]);
			Reflexii[j]->setColor(culReflexii[j][0], culReflexii[j][1], culReflexii[j][2]);
			BLACKBOX.drawMesh(Reflexii[j], 1);
		}
		//lumina
		lumina->setColor(1, 1, 0);
		for (int i = 0; i < 100; i++) {
			transformation[1] *= myTranslate(xCentru, yCentru)* myRotate(2 * glm::pi<float>() / 100)* myTranslate(-xCentru, -yCentru);
			BLACKBOX.setModelMatrix(transformation[1]);
			BLACKBOX.drawMesh(lumina, 1);
		}
		// obiecte
		for (int i = 0; i < 10; i++) {
			obiecte[i]->setColor(cul[i][0], cul[i][1], cul[i][2]);
			BLACKBOX.drawMesh(obiecte[i], 1);
		}
		// camera
		camera->setColor(0, 0, 0);
		BLACKBOX.drawMesh(camera, 1);
		// scena
		scena->setColor(0.6, 0.6, 0.6);
		BLACKBOX.drawMesh(scena, 1);
		// cadru
		cadru->setColor(0.3, 0.3, 0.3);
		BLACKBOX.drawMesh(cadru, 1);
	}

	glm::vec2 vectorvectorNormala(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, glm::vec2 T) {
		// distanta de la fiecare  latura a obiectului pana la punctul T
		float ABT = distantaLiniePunct(A, B, T);
		float BCT = distantaLiniePunct(B, C, T);
		float CDT = distantaLiniePunct(C, D, T);
		float DAT = distantaLiniePunct(D, A, T);
		// calcularea distantei minime
		float distMin = std::min(ABT, BCT);
		distMin = std::min(distMin, CDT);
		distMin = std::min(distMin, DAT);

		// rezultatul puncutlui de incidenta
		if (abs(distMin - ABT) < 1.0f) {
			return glm::vec2(B.y - A.y, B.x - A.x);
		}
		else if (abs(distMin - BCT) < 1.0f) {
			return glm::vec2(C.y - B.y, C.x - B.x);
		}
		else if (abs(distMin - CDT) < 1.0f) {
			return glm::vec2(D.y - C.y, D.x - C.x);
		}
		else if (abs(distMin - DAT) < 1.0f) {
			return glm::vec2(A.y - D.y, A.x - D.x);
		}
	}

	//verificare arie formata din varfuri cu punctul T de pe raza mai mica decat aria obiectului
	bool seCiocneste(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, glm::vec2 T) {
		return abs(ariePoligon(A, B, C, D, T) - (arie(A, B, C) + arie(C, D, A))) <= 0.5f;
	}
	// calcularea ariei unui poligon format din varfurile unui obiect si punctul T de pe raza
	float ariePoligon(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, glm::vec2 T) {
		return arie(A, B, T) + arie(B, C, T) + arie(C, D, T) + arie(D, A, T);
	}
	// aria unui dreptunghi cu formula lui Heron
	float arie(glm::vec2 A, glm::vec2 B, glm::vec2 C) {
		float AB = distantaPuncte(A.x, A.y, B.x, B.y);
		float BC = distantaPuncte(B.x, B.y, C.x, C.y);
		float CA = distantaPuncte(C.x, C.y, A.x, A.y);
		// p = semiperimetrul
		float p = (AB + BC + CA) / 2;
		return sqrt(p*(p - AB)*(p - BC)*(p - CA));
	}
	// distanta de la un punct la o dreapta
	float distantaLiniePunct(glm::vec2 A, glm::vec2 B, glm::vec2 P) {
		return abs((B.y - A.y)*P.x - (B.x - A.x)*P.y + B.x*A.y - B.y*A.x) /
			sqrt((B.y - A.y)*(B.y - A.y) + (B.x - A.x)*(B.x - A.x));
	}
	// distanta dintre 2 puncte cu formula manhatten
	float distantaPuncte(int x1, int y1, int x2, int y2) {
		return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
	}

	//functie chemata dupa ce am terminat cadrul de desenare (poate fi folosita pt modelare/simulare)
	void notifyEndFrame(){}
	//functie care e chemata cand se schimba dimensiunea ferestrei initiale
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//blackbox needs to know
		BLACKBOX.notifyReshape(width,height);
	}

	//--------------------------------------------------------------------------------------------
	//functii de input output --------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------
	//tasta apasata
	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
	}
	//tasta ridicata
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){	}
	//tasta speciala (up/down/F1/F2..) apasata
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
		// rotire raza stanga
		if (key_pressed == GLUT_KEY_LEFT) {
			// golim toate reflexiile
			for (int i = 0; i <= razeTotale; i++) {
				verReflexii[i].clear();
				indReflexii[i].clear();
			}
			// modificam coorodnatele initiale
			razeTotale = -1;
			xc1 = xCentru - 5; xc2 = xCentru + 5;
			yRaza = yCentru - 5; yUltim = yRaza - 1;
			transformation[0] *= myTranslate(xCentru, yCentru)*myRotate(-0.1f)*myTranslate(-xCentru, -yCentru);
			// modificare vector directie
			dirX = dirX * cos(-0.1f) - dirY * sin(-0.1f);
			dirY = dirX * sin(-0.1f) + dirY * cos(-0.1f);
			dirVect = glm::normalize(glm::vec2(dirX, dirY));
			idx1 = 0; idx2 = 1;  idx3 = 2; idx4 = 3;
			//camera hit
			cameraHit = false;
			// golim raza initiala	
			vertecsi.clear();
			indecsi.clear();
			// Raza
			vertecsi.push_back(MyVertexFormat(xc1, yRaza));
			vertecsi.push_back(MyVertexFormat(xc2, yRaza));
			vertecsi.push_back(MyVertexFormat(xc2, yUltim));
			vertecsi.push_back(MyVertexFormat(xc1, yUltim));
			indecsi.push_back(glm::uvec3(idx1, idx2, idx3));
			indecsi.push_back(glm::uvec3(idx3, idx4, idx1));
			raza = myLoadObjectas_Mesh(vertecsi, indecsi);
		}
		if (key_pressed == GLUT_KEY_RIGHT) {
			// golim toate reflexiile
			for (int i = 0; i <= razeTotale; i++) {
				verReflexii[i].clear();
				indReflexii[i].clear();
			}
			// modificam coorodnatele initiale
			razeTotale = -1;
			transformation[0] *= myTranslate(xCentru, yCentru)*myRotate(0.1f)*myTranslate(-xCentru, -yCentru);
			xc1 = xCentru - 5; xc2 = xCentru + 5;
			yRaza = yCentru - 5; yUltim = yRaza - 1;
			// modificare vector directie
			dirX = dirX * cos(0.1f) - dirY * sin(0.1f);
			dirY = dirX * sin(0.1f) + dirY * cos(0.1f);
			dirVect = glm::normalize(glm::vec2(dirX, dirY));
			idx1 = 0; idx2 = 1; idx3 = 2; idx4 = 3;
			//camera hit
			cameraHit = false;
			// golim raza intiala
			vertecsi.clear();
			indecsi.clear();
			// Raza
			indecsi.push_back(glm::uvec3(idx1, idx2, idx3));
			indecsi.push_back(glm::uvec3(idx3, idx4, idx1));
			vertecsi.push_back(MyVertexFormat(xc1, yRaza));
			vertecsi.push_back(MyVertexFormat(xc2, yRaza));
			vertecsi.push_back(MyVertexFormat(xc2, yUltim));
			vertecsi.push_back(MyVertexFormat(xc1, yUltim));
			raza = myLoadObjectas_Mesh(vertecsi, indecsi);
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
	lab::glut::WindowInfo window(std::string("Lab EGC 3 - transformari"),1000,600,300,50,true);
	lab::glut::ContextInfo context(3,3,false);
	lab::glut::FramebufferInfo framebuffer(true,true,false,false);
	lab::glut::init(window, context, framebuffer);

	//initializeaza GLEW (ne incarca functiile openGL, altfel ar trebui sa facem asta manual!)
	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	//creem clasa noastra si o punem sa asculte evenimentele de la GLUT
	//_DUPA_ GLEW ca sa avem functiile de OpenGL incarcate inainte sa ii fie apelat constructorul (care creeaza obiecte OpenGL)
	Lab mylab;
	lab::glut::setListener(&mylab);

	//run
	lab::glut::run();

	return 0;
}