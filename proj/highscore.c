#include "highscore.h"
#include "proj.h"
#include "state.h"

void highscore_init(){
	FILE* file;
	file = fopen(highscore_path, "r");

	highscore_size = 0;
	highscores = (score**) malloc(MAX_NUMBER_OF_SCORES*sizeof(score*));
	score* sc;

	highscore_menu = (menu*) malloc(sizeof(menu));
	highscore_menu = create_menu();
	menu_add_button(highscore_menu, create_button(400, 650, 200, 100, &highscore_back_on_click, 2));

	if(file == NULL)
		return;

	fscanf(file, "//MINIX INVADERS HIGHSCORES//\n");

	unsigned char i = 0;

	while((sc = highscore_read(file)) != NULL){
		highscores[i] = sc;
		i++;
	}

	highscore_size = i;
	fclose(file);
}

score* highscore_read(FILE* file){
	if(feof(file))
		return NULL;

	char buf;

	score* sc = (score*) malloc(sizeof(score));
	sc->name = (char*) malloc(20*sizeof(char));
	unsigned char length;

	fgets(sc->name, 20, file);
	length = strlen(sc->name);

	sc->name[length-1] = '\0';
	length--;

	sc->name = (char*) realloc(sc->name, length*sizeof(char));

	fscanf(file, "%u\n", &(sc->points));

	return sc;
}

int highscore_add(char* name, unsigned int points){
	score* sc = (score*) malloc(sizeof(score));
	sc->name = name;
	sc->points = points;

	unsigned char i;
	for(i = 0; i < highscore_size; i++){
		if(points > highscores[i]->points)
			break;
	}

	highscore_size++;
	highscores = (score**) realloc(highscores, highscore_size*sizeof(score*));

	unsigned char j;
	for(j = highscore_size; j > i; j--){
		highscores[j] = highscores[j-1];
	}
	highscores[i] = sc;

	return 0;
}

int highscore_write(FILE* file, score* sc){
	fprintf(file, "\n%s\n%u", sc->name, sc->points);
	return 0;
}

void highscore_tick(){
	menu_draw(highscore_menu);
	vg_draw_frame(100, 150, 800, 450, 2);

	//Vertical lines
	vg_draw_line(400, 150, 400, 600, 2);
	vg_draw_line(700, 150, 700, 600, 2);

	//Horizontal lines
	vg_draw_line(100, 195, 900, 195, 2);
	vg_draw_line(100, 200, 900, 200, 2);

	unsigned char i;
	for(i = 1; i < 10; i++){
		vg_draw_line(100, 200+40*i, 900, 200+40*i, 2);
	}
}

void highscore_back_on_click(){
	change_state(main_menu);
}

void highscore_destruct(){
	FILE* file;
	file = fopen(highscore_path, "w");

	printf("size: %d\n", highscore_size);

	fprintf(file, "//MINIX INVADERS HIGHSCORES//");

	unsigned char i;

	if(file != NULL){
		for(i = 0; i < highscore_size && i < MAX_NUMBER_OF_SCORES; i++){
			printf("Name: %s\tScore: %u\n", highscores[i]->name, highscores[i]->points);
			highscore_write(file, highscores[i]);
		}
		fclose(file);
	}

	for(i = 0; i < highscore_size && i < MAX_NUMBER_OF_SCORES; i++)
		free(highscores[i]);
	free(highscores);
	highscore_size = 0;
	delete_menu(highscore_menu);

	start_menu_init();
}
