BOICU IONUT 335CA - TEMA 1 EGC

Pentru realizarea temei am folosit scheletul de laborator3.
	Pentru inceput am hardcodat coordonatele scenei, obiectelor din scena, razei initiale si a camerei. Am salvat coordonatele si dimensiunea obiectelor intr-o matrice, la fel si culorile acestora pe care le-am randomizat.
	Reflectarea razei se deseneaza timp de 30 reflexii sau pana la intalnirea camerei.
	Pentru randarea razei am folosit 4 indici prin care imi salvez ultimele 4 puncte din indecsi pentru mesh. folosesc 2 puncte P1,P2 (punctul cel mai din stanga al razei si cel mai din dreapta), pe care le verific de coliziuni cu peretii, cele 10 obiecte, camera.
	1)Coliziunea cu obiectele:
	- identific 4 puncte(patrulaterelor - obiectelor) apoi calculez aria celor 4 puncte si
aria pe care o formeaza cele 4 puncte cu punctele P1 sau P2. Daca diferenta ariilor este mai mica
sau egala cu 0 (0.5) inseamna ca s-a produs o coliziune cu un obiect. Odata produsa coliziunea luam normalaVectorului cu functia vectorNormalizat, care calculeaza cu ce muchie s-a produs coliziunea, prin aflarea distantei minime cu toate celelalate muchii. Apoi salvam noua culoare a razei reflectate intr-o matrice de de culori, si calculam vectorul directiei cu functia reflect din glm.
	2)Coliziunea cu peretii:
	-identific cele 4 colturi ale cadrului, si daca coordonatele razei (xc1,xc2,yUltim) depasesc una din coordonate  atunci s-a produs o coliziune cu peretii cadrului. Ca si la coliziunea cu obiectele calculez normala vectorului incident, preiau culoarea razei reflectate si calculez vectorul director cu functia reflect.
	3)Coliziunea cu camera:
	-identific varfurile camerei si calculez aria camerei, si suma ariilor varfurilor camerei impreuna cu punctele P1 si P2 de pe raza incidenta. daca diferenta este mai mica sau egala cu 0 atunci raza a atins camera si se opreste cautarea unor noi reflexii.
	
	Pentru fiecare reflexie gasita se adauga in vectorul de vertecsi si cel de indecsi valorile noilor puncte in functie de vectorul director si indecsi.
	
	Pentru desenarea sursei de lumina rotit un patrat 360 de grade si am simulat un cerc.

	Functii ajutatoare:
	1)vetorNormala: calculeaza distanta de la punctul "T" pana la fiecare din laturile unui obiect gaseste distanta minima pana la cea mai apropiata latura si returneaza coordonatele punctului de incidenta.
	2)seCiocneste: verifica incidenta cu unul dintre obiecte prin metoda descrisa la coliziunea cu obiectele.
	3)ariePoligon: calculeaza suma ariilor laturilor impreuna cu punctul "T"
	4)arie: calculeaza aria unui triunghi cu formula lui Heron
	5)distantaLiniePunct: calculeaza distanta de la un punct la o dreapta
	6)distantaPuncte: calculeaza distanta dintre 2 puncte
	