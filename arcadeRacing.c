#include ".\include\arLogic.h"
#include ".\include\arGraphics.h"

#define TIME_TO_CHANGE_LINE 3//time in seconds to change the line on which opponent's car is currently on

int main(){
	
	srand(time(0));
	
	//creating main window of the program
	sfVideoMode mode = {WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_DEPTH};
	sfRenderWindow* window = sfRenderWindow_create(mode, "Arcade Racing", sfResize | sfClose, NULL);
	
	//loading a font to show rules of the game
	sfFont* font = sfFont_createFromFile("fonts/arial.ttf");
	
	//creating player's car
	sfSprite* carSprite = sfSprite_create();
	sfTexture* carTexture = sfTexture_createFromFile("images/cars.png", NULL);
	sfSprite_setTexture(carSprite, carTexture, sfFalse);
	sfSprite_setTextureRect(carSprite, (sfIntRect){0, 0, CAR_WIDTH, CAR_HEIGHT});
	
	//creating mounatins on background
	sfSprite* bgSprite = sfSprite_create();
	sfTexture* bgTexture = sfTexture_createFromFile("images/bg.jpg", NULL);
	sfSprite_setTexture(bgSprite, bgTexture, sfFalse);
	sfSprite_setTextureRect(bgSprite, (sfIntRect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT/2});
	sfSprite_setPosition(bgSprite, (sfVector2f){0, 0});
	
	sfClock* clock = sfClock_create();
	float timeLimit = GAME_TIME;//time limit to play game
	float timer = 0;//timer to show new opponent
	sfEvent event;
	
	opponents* opponent = createOpponent();

	Player player = {0, 0, 0, 0, 0};
	int score = 0;
	
	//creating road
	roadParts* segments = createRoad();
	
	//calculating road length
	float roadLength = getRoadLength(segments, NUM_SEGMENTS);
	
	float currentCurvature = 0;//current curvature of the road
	float accumulatedCurvature = 0;//accumulated curvature of the road
	
	sfKeyCode key = sfKeyUnknown;//defines which key is pressed when shplash screen is showed
	int trigger = 1;//defines whether to show a splash screen or no
	
	splashScreenTypes screenType = beforeGamePlayBtnActive;//type of splash screen to show 
	
	while(sfRenderWindow_isOpen(window)){
		
		while(sfRenderWindow_pollEvent(window, &event)){
			
			if(event.type == sfEvtClosed) sfRenderWindow_close(window);
			if(event.type == sfEvtKeyPressed) key = event.key.code;
				
			if(!trigger && key == sfKeyEscape){
				trigger = 1;
				screenType = duringGameContinueBtnActive;
			}
		}
		
		sfRenderWindow_clear(window, sfBlack);
		sfTime elapsed = sfClock_restart(clock);
		float elapsedSeconds = sfTime_asSeconds(elapsed);
		
		//if game time ended we need to show the final score
		//and prompt the user if he want to play again
		if(!trigger && timeLimit <= 0){
			trigger = 1;
			screenType = afterGamePlayAgainBtnActive;
		}
		
		if(trigger){

			screenType = showSplashScreen(window, font, key, score, screenType, &trigger);
			
			if(screenType == restartGame){//if the user decided to play again we need to restart the game
				reset(player, carSprite, &timeLimit, &timer, &elapsedSeconds, &score, &currentCurvature, &accumulatedCurvature);
				opponent = createOpponent();
			}
			
			key = sfKeyUnknown;
			continue;
		}
		
		timer += elapsedSeconds;
		if(timer >= TIME_TO_CHANGE_LINE){
			moveDirection = rand() % 2;
			timer = 0;
		}
		
		float destCurvature = getDestCurvature(player.position, segments);//getting curvature of current road segment
		float curvatureDifference = destCurvature - currentCurvature;
		currentCurvature += curvatureDifference * elapsedSeconds * player.speed;// -0.5 <= currCurvature <= 0.5
		accumulatedCurvature += currentCurvature * elapsedSeconds * player.speed;
		
		handlePlayerMovements(carSprite, elapsedSeconds, &player, roadLength);
		
		if(opponent->currentPosition <= -1 * opponent->initialPosition || opponent->currentPosition >= 2 * opponent->initialPosition){
			if(opponent->currentPosition < 0 && !opponent->wasCrashed) score++;
			opponent = createOpponent();
		}
		
		handleOpponent(opponent, player, elapsedSeconds, currentCurvature, moveDirection);
		
		player.curvDifference = player.curvature - accumulatedCurvature;
		
		sfSprite_setPosition(carSprite, (sfVector2f){WINDOW_WIDTH * (MIDDLE + player.curvDifference) - sfSprite_getGlobalBounds(carSprite).width / 2, PLAYER_CAR_Y_POS});
		
		handleCollisions(&player, carSprite, opponent->sprite, elapsedSeconds, opponent);
		
		drawScenery(player, opponent, currentCurvature, window);

		sfRenderWindow_drawSprite(window, bgSprite, NULL);//drawing mountains on background
		sfRenderWindow_drawSprite(window, carSprite, NULL);//drawing player's car
		if(player.startedGame){
			timeLimit = showTimer(window, font, timeLimit, elapsedSeconds);
		} else {
			timeLimit = showTimer(window, font, timeLimit, 0);
		}
		showCurrentScore(window, font, score);
		sfRenderWindow_display(window);
	}

	return 0;
}
