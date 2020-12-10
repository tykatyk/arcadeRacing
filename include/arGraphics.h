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

typedef enum {
	restartGame = -1,
	beforeGamePlayBtnActive = 1,
	beforeGameExitBtnActive,
	duringGameContinueBtnActive,
	duringGameExitBtnActive,
	afterGamePlayAgainBtnActive,
	afterGameExitBtnActive
} splashScreenTypes;

void showCurrentScore(sfRenderWindow* window, sfFont* font, int score);

void showFinalScore(sfRenderWindow* window, sfFont* font, int score);

//shows a splash screen at the beggining of the game, during the game if a user presses Esc key and after the game
int showSplashScreen(sfRenderWindow* window, sfFont* font, sfKeyCode key, int score, splashScreenTypes screenType, int* trigger);

//showing rules of the game
void showRules(sfRenderWindow* window, sfFont* font);

float showTimer(sfRenderWindow* window, sfFont* font, float timeLimit, float elapsedSeconds);

void drawScenery(Player player, opponents* opponent, float currentCurvature, sfRenderWindow* window);
