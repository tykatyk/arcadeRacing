#include <stdio.h>
#include <SFML/Graphics.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define COLOR_DEPTH 32
#define PLAYER_CAR_Y_POS 500
#define GRASS_STRIP_HEIGHT 25//height (in pixels) of a strip of grass colored in the same color
#define LIMIT_LINE_STRIP_HEIGHT 30//height (in pixels) of road's limiting line colored in the same color
#define LIMIT_LINE_CHANGE_SPEED 2
#define MARKING_LINE_HEIGHT 35//height (in pixels) of road's marking line colored in the same color
#define MARKING_LINE_WIDTH 0.025
#define MARKING_LINE_CHANGE_SPEED 4
#define EDGE_WIDTH 1.0625

sfColor lightGrey = {191, 191, 191, 255};
sfColor mediumGrey = {66, 61, 66, 255};
sfColor darkGrey = {26,24,26,255};
sfColor darkGreen = {17, 107, 53, 255};
sfColor roadColor = {103, 106, 110, 255};

typedef enum splashScreenTypes {
	restartGame = -1,
	beforeGamePlayBtnActive = 1,
	beforeGameExitBtnActive,
	duringGameContinueBtnActive,
	duringGameExitBtnActive,
	afterGamePlayAgainBtnActive,
	afterGameExitBtnActive
} splashScreenTypes;

void showCurrentScore(sfRenderWindow* window, sfFont* font, int score){
	
	int x = 10;
	int y = 30;
	int fontSize = 14;
	
	sfText* text = sfText_create();
	sfText_setFont(text, font);
	sfText_setCharacterSize(text, fontSize);
	sfText_setFillColor(text, mediumGrey);

	char string[50] = "Score: ";
	char scoreString[50];
	sprintf(scoreString, "%d", score);
	
	strcat(string, scoreString);
	sfText_setString(text, string);
	sfText_setPosition(text, (sfVector2f){x, y});

	sfRenderWindow_drawText(window, text, NULL);
}

void showFinalScore(sfRenderWindow* window, sfFont* font, int score){
	
	int fontSize = 18;
	sfText* text = sfText_create();
	sfText_setFont(text, font);
	sfText_setCharacterSize(text, fontSize);
	sfText_setFillColor(text, lightGrey);

	char string[50] = "You've got ";
	char scoreString[50];
	sprintf(scoreString, "%d", score);
	
	strcat(string, scoreString);
	if(score == 1){
		strcat(string, " point");
	} else {
		strcat(string, " points");
	}
	
	sfText_setString(text, string);
	int x = WINDOW_WIDTH / 2  - sfText_getGlobalBounds(text).width / 2;
	int y = 400;
	sfText_setPosition(text, (sfVector2f){x, y});

	sfRenderWindow_drawText(window, text, NULL);
}

//shows a splash screen at the beggining of the game, during the game if a user presses Esc key and after the game
int showSplashScreen(sfRenderWindow* window, sfFont* font, sfKeyCode key, int score, splashScreenTypes screenType, int* trigger){
	
	if(
		key == sfKeyEnter && screenType == beforeGamePlayBtnActive ||
		key == sfKeyEnter && screenType == duringGameContinueBtnActive
	){
		*trigger = 0;
	}
	
	if(
		key == sfKeyEnter && screenType == beforeGameExitBtnActive ||
		key == sfKeyEnter && screenType == duringGameExitBtnActive ||
		key == sfKeyEnter && screenType == afterGameExitBtnActive
	){
		 sfRenderWindow_close(window);
	}
	
	if(key == sfKeyEnter && screenType == afterGamePlayAgainBtnActive){
		*trigger = 0;
		return -1;//restarting game
	}
	
	if(key == sfKeyUp && screenType == beforeGameExitBtnActive) screenType = beforeGamePlayBtnActive;
	if(key == sfKeyUp && screenType == duringGameExitBtnActive) screenType = duringGameContinueBtnActive;
	if(key == sfKeyUp && screenType == afterGameExitBtnActive) screenType = afterGamePlayAgainBtnActive;
	
	if(key == sfKeyDown && screenType == beforeGamePlayBtnActive) screenType = beforeGameExitBtnActive;
	if(key == sfKeyDown && screenType == duringGameContinueBtnActive) screenType = duringGameExitBtnActive;
	if(key == sfKeyDown && screenType == afterGamePlayAgainBtnActive) screenType = afterGameExitBtnActive;
	
	sfSprite* buttonsSprite = sfSprite_create();
	sfTexture* buttonsTexture = sfTexture_createFromFile("images/buttons.png", NULL);//this should be refactored for better performance
	sfSprite_setTexture(buttonsSprite, buttonsTexture, sfFalse);
	
	int x, y, width = 262, height = 138;
	
	if(screenType == 1){
		x = 0;
		y = 0;
	}
	
	if(screenType == 2){
		x = 0;
		y = 139;
	}
	
	if(screenType == 3){
		x = 263;
		y = 0;
	}
	
	if(screenType == 4){
		x = 263;
		y = 139;
	}
	
	if(screenType == 5){
		x = 526;
		y = 0;
	}
	
	if(screenType == 6){
		x = 526;
		y = 139;
	}
	
	sfSprite_setTextureRect(buttonsSprite, (sfIntRect){x, y, width, height});
	
	x = WINDOW_WIDTH/2 - sfSprite_getLocalBounds(buttonsSprite).width/2;
	y = 150;
	sfSprite_setPosition(buttonsSprite, (sfVector2f){x, y});
	
	sfVertexArray* rectangle = sfVertexArray_create();
	sfVertexArray_setPrimitiveType(rectangle, sfQuads);
	
	sfVertex v1, v2, v3, v4;
	
	sfVector2f topLeft = {0, 0};
	sfVector2f topRight = {800, 0};
	sfVector2f bottomRight = {800, 600};
	sfVector2f bottomLeft = {0, 600};
	
	v1.position = topLeft;
	v1.color = darkGrey;
	sfVertexArray_append(rectangle, v1);
	
	v2.position = topRight;
	v2.color = darkGrey;
	sfVertexArray_append(rectangle, v2);
	
	v3.position = bottomRight;
	v3.color = darkGrey;
	sfVertexArray_append(rectangle, v3);
	
	v4.position = bottomLeft;
	v4.color = darkGrey;
	sfVertexArray_append(rectangle, v4);
	
	sfRenderWindow_clear(window, sfBlack);
	sfRenderWindow_drawVertexArray(window, rectangle, NULL);
	sfRenderWindow_drawSprite(window, buttonsSprite, NULL);
	
	if(screenType == beforeGamePlayBtnActive || screenType == beforeGameExitBtnActive) showRules(window, font);
	if(screenType == afterGamePlayAgainBtnActive || screenType == afterGameExitBtnActive) showFinalScore(window, font, score);
	sfRenderWindow_display(window);
	
	return screenType;
}

//showing rules of the game
showRules(sfRenderWindow* window, sfFont* font){
	
	int fontSize = 18;
	
	sfText* text = sfText_create();
	sfText_setFont(text, font);
	sfText_setCharacterSize(text, fontSize);
	sfText_setFillColor(text, lightGrey);
	
	char* string = "Rules: try to avoid as many car accidents as you can\n"
					"while driving at the maximum speed\n\n"
					"Use keyboard arrows to control your car.\n"
					"< (turn left), > (turn right),  ^ (speed up), v (slow down).\n\n"
					"Press Esc while playing to exit";
					
	sfText_setString(text, string);
	int x = WINDOW_WIDTH / 2  - sfText_getGlobalBounds(text).width / 2;
	int y = 400;
	sfText_setPosition(text, (sfVector2f){x, y});
	
	sfRenderWindow_drawText(window, text, NULL);
}

float showTimer(sfRenderWindow* window, sfFont* font, float timeLimit, float elapsedSeconds){
	
	int x = 10;
	int y = 10;
	int fontSize = 14;
	int numDigits = 3;
	
	sfText* text = sfText_create();
	sfText_setFont(text, font);
	
	sfText_setCharacterSize(text, fontSize);
	sfText_setFillColor(text, mediumGrey);

	float remainedTime = timeLimit - elapsedSeconds;
	char string[50] = "Remained time: ";
	char timeLimitString[50];
	
	if(timeLimit == GAME_TIME){
		sprintf(timeLimitString, "%d", GAME_TIME);
	} else {
		if(timeLimit >= 1 && timeLimit < 10) numDigits = 2;
		if(timeLimit >= 0 && timeLimit < 1) numDigits = 1;
		if(timeLimit < 0.15) remainedTime = 0;
			
		gcvt(remainedTime, numDigits, timeLimitString);
	}

	strcat(string, timeLimitString);
	sfText_setString(text, string);
	sfText_setPosition(text, (sfVector2f){x, y});

	sfRenderWindow_drawText(window, text, NULL);
	return remainedTime;
}

void drawScenery(Player player, opponents* opponent, float currentCurvature, sfRenderWindow* window){
		
		sfColor color;
		int i;
		
		for(i=0; i<WINDOW_HEIGHT/2; i++){
			
			int row = i + WINDOW_HEIGHT/2;
			float perspective = (float)i / (WINDOW_HEIGHT / 2);
			float offset = currentCurvature * powf(1 - perspective, CUBE);//to have turns on the road we can't use linear function of perspective so we use qubic 
			float roadWidth = MAX_ROAD_WIDTH * perspective + MIN_ROAD_WIDTH;
			float markingWidth = roadWidth * MARKING_LINE_WIDTH;
			float edgeWidth = roadWidth * EDGE_WIDTH;
			float left;
			float right;
			
			//grass
			color = sinf(player.position + GRASS_STRIP_HEIGHT * (pow(1-perspective, CUBE))) > 0 ? sfGreen : darkGreen;
			sfVertex v1 = {(sfVector2f){0, row}, color};
			sfVertex v2 = {(sfVector2f){WINDOW_WIDTH, row}, color};
			
			//limit lines
			color = sinf(LIMIT_LINE_CHANGE_SPEED * player.position + LIMIT_LINE_STRIP_HEIGHT * (pow(1-perspective, SQUARE))) > 0 ? sfWhite : sfRed;
			left = (MIDDLE + offset - edgeWidth / 2) * WINDOW_WIDTH;
			right = (MIDDLE + offset  + edgeWidth / 2) * WINDOW_WIDTH;
			sfVertex v3 = {(sfVector2f){left, row}, color};
			sfVertex v4 = {(sfVector2f){right, row}, color};
			
			//road
			left = (MIDDLE + offset  - roadWidth / 2) * WINDOW_WIDTH;
			right = (MIDDLE + offset  + roadWidth / 2) * WINDOW_WIDTH;
			sfVertex v5 = {(sfVector2f){left, row}, roadColor};
			sfVertex v6 = {(sfVector2f){right, row}, roadColor};
			
			//marking lines
			color = sinf(MARKING_LINE_CHANGE_SPEED * player.position + MARKING_LINE_HEIGHT * (pow(1-perspective, CUBE))) > 0 ? sfWhite : roadColor;
			left = (MIDDLE + offset  - markingWidth / 2) * WINDOW_WIDTH;
			right = (MIDDLE + offset  + markingWidth / 2) * WINDOW_WIDTH;
			sfVertex v7 = {(sfVector2f){left, row}, color};
			sfVertex v8 = {(sfVector2f){right, row}, color};
			
			sfVertex lines[] = {v1, v2, v3, v4, v5, v6, v7, v8};
			sfRenderWindow_drawPrimitives(window, lines, 8, sfLines, NULL);
			
			if(opponent->yPos > WINDOW_HEIGHT / 2 && opponent->yPos < WINDOW_HEIGHT){
				sfRenderWindow_drawSprite(window, opponent->sprite, NULL);
			}
		}
}
