#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <SFML/Graphics.h>
#include "./include/arLogic.h"

sfIntRect redCarTextureForward = {0, 0, 70, 83};
sfIntRect redCarTextureLeft = {160, 0, 89, 85};
sfIntRect redCarTextureRight = {72, 0, 88, 85};

sfIntRect violetCarTextureForward = {0, 90, 68, 83};
sfIntRect yellowCarTextureForward = {0, 178, 68, 83};

sfIntRect violetCarTextureLeft = {161, 90, 88, 85};
sfIntRect yellowCarTextureLeft = {161, 178, 88, 85};

sfIntRect violetCarTextureRight = {71, 90, 88, 86};
sfIntRect yellowCarTextureRight = {71, 178, 88, 86};

roadParts* createRoad(){
	
	roadParts* segments = (roadParts*) calloc(NUM_SEGMENTS, sizeof(roadParts));
	
	segments[0].length = 50.0;
	segments[0].curvature = 0;
	
	segments[1].length = 80;
	segments[1].curvature = -0.5f;
	
	segments[2].length = 200;
	segments[2].curvature = 0;
	
	segments[3].length = 100;
	segments[3].curvature = 0.5;
	
	segments[4].length = 100;
	segments[4].curvature = -1;
	
	segments[5].length = 300;
	segments[5].curvature = 0;
	
	segments[6].length = 50;
	segments[6].curvature = -0.7;
	
	segments[7].length = 100;
	segments[7].curvature = 0.5;
	
	return segments;
}

float getRoadLength(roadParts* segments, int numSegments){
	
	float roadLength = 0;
	int i = 0;
	
	for(; i<numSegments; i++){
		roadLength += segments[i].length;
	}
	
	return roadLength;
}

//gets curvature of the current road segment
//curvature defines a radius of road's turn
float getDestCurvature(float playerPosition, roadParts* segments){
		float distance = 0;
		int i = 0;
		
		while(distance < playerPosition){
			distance += segments[i].length;
			if(distance >= playerPosition) break;
			i++;
		}
		
		return segments[i].curvature;
}

opponents* createOpponent(){
	
		opponents* opponent = (opponents*) calloc(1, sizeof(opponents));
	
		float pos = MIN_DIST + rand() % MAX_DIST;//distance from player's car to opponent's car
		int color = rand() % 2;
		float speed = OPPONENT_MIN_SPEED + (rand() % 20)/100;//min speed = 0.4, max speed = 0.6
		
		sfSprite* carSprite = sfSprite_create();
		sfTexture* carTexture = sfTexture_createFromFile("images/cars.png", NULL);
		sfSprite_setTexture(carSprite, carTexture, sfFalse);
		if(color == violet) sfSprite_setTextureRect(carSprite, violetCarTextureForward); 
		if(color == yellow) sfSprite_setTextureRect(carSprite, yellowCarTextureForward);
		
		opponent->isHidden = 1,
		opponent->isCrashed = 0,
		opponent->wasCrashed = 0,
		opponent->initialPosition = pos,
		opponent->currentPosition = pos,
		opponent->perspective = 0,
		opponent->initSpeed = speed,
		opponent->curSpeed = speed,
		opponent->carColor = color,
		opponent->prevY = WINDOW_HEIGHT / 2;
		opponent->sprite = sfSprite_create();
		sfSprite_setTexture(opponent->sprite, carTexture, sfFalse);
		
	return opponent;
}

void handleOpponent(opponents* opponent, Player player, float elapsedSeconds, float currentCurvature, int moveDirection){
	
	if(opponent->isCrashed) opponent->curSpeed -= OPPONENT_SPEED_DECREESE * elapsedSeconds;
	if(!opponent->isCrashed && opponent->curSpeed < opponent->initSpeed) opponent->curSpeed += OPPONENT_SPEED_INCREASE * elapsedSeconds;
	
	if(opponent->curSpeed > opponent->initSpeed) opponent->curSpeed = opponent->initSpeed;
	if(opponent->curSpeed < 0) opponent->curSpeed = 0;
	
	if(opponent->currentPosition += DISTANCE_CHANGE_SPEED * elapsedSeconds * (opponent->curSpeed - player.speed));
	if(opponent->currentPosition > opponent->initialPosition) opponent->isHidden = 1;
	
	opponent->perspective = 1 - opponent->currentPosition/opponent->initialPosition;
	if(opponent->perspective < 0) opponent->perspective = 0;
	if(opponent->perspective > 1) opponent->perspective = 1;
	if(opponent->currentPosition < 0 || opponent->currentPosition > opponent->initialPosition){
		opponent->perspective = 0;
	}
	
	float scale = MIN_SCALE + opponent->perspective < MAX_SCALE ? MIN_SCALE + opponent->perspective : MAX_SCALE;
	sfSprite_setScale(opponent->sprite, (sfVector2f){scale, scale});
	
	opponent->yPos = WINDOW_HEIGHT / 2  * (1 + opponent->perspective);
	
	float currentRoadWidth = MAX_ROAD_WIDTH * opponent->perspective + MIN_ROAD_WIDTH;
	
	if(opponent->currentPosition <= opponent->initialPosition && opponent->isHidden == 1){//opponent shows on the horizon
		opponent->xPos = (MIDDLE + currentCurvature) * WINDOW_WIDTH - sfSprite_getGlobalBounds(opponent->sprite).width/2 + rand() % 101;
		moveDirection = -1;
		opponent->isHidden = 0;
	} else {
		
		float roadLeft = (MIDDLE + currentCurvature * powf(1 - opponent->perspective, CUBE) - currentRoadWidth / 2) * WINDOW_WIDTH;
		float roadRight = (MIDDLE + currentCurvature * powf(1 - opponent->perspective, CUBE) + currentRoadWidth / 2) * WINDOW_WIDTH;
		
		float opponentLeft = opponent->xPos;
		float opponentRight = opponent->xPos + sfSprite_getGlobalBounds(opponent->sprite).width;
		
		if(moveDirection == left){//opponent decides to change a traffic lane
			if(currentCurvature <= LEFT_TURN_CURVATURE){
				if(opponentLeft > roadLeft){
					opponent->xPos -= X_POS_CHANGE_SPEED*(opponentLeft - roadLeft) * opponent->curSpeed * elapsedSeconds;
				}
			}
		}
		
		if(moveDirection == right){
			if(currentCurvature >= RIGHT_TURN_CURVATURE){
				if(opponentRight < roadRight){
					opponent->xPos += X_POS_CHANGE_SPEED*(roadRight - opponentRight) * opponent->curSpeed * elapsedSeconds;
				}
			}
		}
		//if an opponent isn't crashed he always shhould be on the road
		if(!opponent->isCrashed){
			if(opponentLeft < roadLeft) opponent->xPos = roadLeft;
			if(opponentRight > roadRight) opponent->xPos -= opponentRight - roadRight;
		}
		
		if(//if a crashed opponent is visible and his car is on the road
			opponent->isCrashed && opponent->currentPosition > 0 &&
			opponent->currentPosition < opponent->initialPosition &&
			opponentLeft >= roadLeft && opponentRight <= roadRight
		){//slowly move him out of the screen
			opponent->yPos = WINDOW_HEIGHT / 2  *(1 + opponent->perspective);
			
			if(currentCurvature < 0 && opponent->xPos <= WINDOW_WIDTH / 2 ) opponent->xPos -= opponent->xPos * elapsedSeconds;
			if(currentCurvature < 0 && opponent->xPos >= WINDOW_WIDTH / 2 ) opponent->xPos += opponent->xPos * elapsedSeconds;
			
			if(currentCurvature > 0 && opponent->xPos <= WINDOW_WIDTH / 2 ) opponent->xPos -= opponent->xPos * elapsedSeconds;
			if(currentCurvature > 0 && opponent->xPos >= WINDOW_WIDTH / 2 ) opponent->xPos += opponent->xPos * elapsedSeconds;
		}
		
		//if a crashed opponent is behind player's car and out of the road put him back on the road
		if(opponent->isCrashed && opponent->currentPosition <= 0){
			opponent->isCrashed = 0;
			if(opponentLeft < roadLeft) opponent->xPos = roadLeft;
			if(opponentRight > roadRight) opponent->xPos -= opponentRight - roadRight;
		}
	}
	
	sfSprite_setPosition(opponent->sprite, (sfVector2f){opponent->xPos, opponent->yPos});
	
	//change opponent's car image depending on road's current curvature
	if(currentCurvature >= -SHOULD_CHANGE_CAR && currentCurvature <= SHOULD_CHANGE_CAR){
		if(opponent->carColor == violet) sfSprite_setTextureRect(opponent->sprite, violetCarTextureForward);
		if(opponent->carColor == yellow) sfSprite_setTextureRect(opponent->sprite, yellowCarTextureForward);
	}
	
	if(currentCurvature < -SHOULD_CHANGE_CAR){
		if(opponent->carColor == violet) sfSprite_setTextureRect(opponent->sprite, violetCarTextureLeft);
		if(opponent->carColor == yellow) sfSprite_setTextureRect(opponent->sprite, yellowCarTextureLeft);
	}
	
	if(currentCurvature > SHOULD_CHANGE_CAR){
		if(opponent->carColor == violet) sfSprite_setTextureRect(opponent->sprite, violetCarTextureRight);
		if(opponent->carColor == yellow) sfSprite_setTextureRect(opponent->sprite, yellowCarTextureRight);
	}
}

void handlePlayerMovements(sfSprite* carSprite, float elapsedSeconds, Player* player, float roadLength){
	
	if(sfKeyboard_isKeyPressed(sfKeyUp)){
		if(!player->startedGame) player->startedGame = 1;
		player->speed += elapsedSeconds;
		sfSprite_setTextureRect(carSprite, redCarTextureForward);
	} else {
		player->speed -= PLAYER_SPEED_DECREASE * elapsedSeconds;
	}
	
	if(sfKeyboard_isKeyPressed(sfKeyDown)){
		player->speed -= elapsedSeconds;
	}
	
	if(sfKeyboard_isKeyPressed(sfKeyLeft)){
		player->curvature -= PLAYER_CURV_CHANGE_SPEED  * elapsedSeconds;
		sfSprite_setTextureRect(carSprite, redCarTextureLeft);
	}
	
	if(sfKeyboard_isKeyPressed(sfKeyRight)){
		player->curvature += PLAYER_CURV_CHANGE_SPEED * elapsedSeconds;
		sfSprite_setTextureRect(carSprite, redCarTextureRight);
	}
	
	if(fabs(player->curvDifference) > PLAYER_CURV_DIFFERENCE_LIMIT) player->speed -= GRASS_SLOW_DOWN * elapsedSeconds;//slow down player's car because he is out of the road 
	
	if(player->speed > PLAYER_MAX_SPEED) player->speed = PLAYER_MAX_SPEED;//define MAX_SPEED
	if(player->speed < PLAYER_MIN_SPEED) player->speed = PLAYER_MIN_SPEED;
	
	player->position += PLAYER_DISTANCE_CHANGE_SPEED * player->speed * elapsedSeconds;
	if(player->position > roadLength) player->position -= roadLength;
}

//checks if a player crashed into another car
//currently only left and right crashes are correctly handled
//handling top and bottom crashes can be added to better gameplay
void handleCollisions(Player* player, sfSprite* playerCar, sfSprite* opponentCar, float elapsedSeconds, opponents* opponent){
	
	if(sfSprite_getPosition(playerCar).y > opponent->prevY - sfSprite_getGlobalBounds(playerCar).height &&
	   sfSprite_getPosition(playerCar).y < sfSprite_getPosition(opponentCar).y + sfSprite_getGlobalBounds(opponentCar).height)
	{
		
		if(//at the right of player's car
			sfSprite_getPosition(playerCar).x - (sfSprite_getPosition(opponentCar).x + sfSprite_getGlobalBounds(opponentCar).width) <= 0 &&
			sfSprite_getPosition(playerCar).x - sfSprite_getPosition(opponentCar).x <= 0 &&
			sfSprite_getPosition(playerCar).x - sfSprite_getPosition(opponentCar).x >= -sfSprite_getGlobalBounds(playerCar).width + CAR_ROTATION_OFFSET
		)
		{
			opponent->isCrashed = 1;
			opponent->xPos = sfSprite_getPosition(playerCar).x + sfSprite_getGlobalBounds(playerCar).width;
			sfSprite_setPosition(opponent->sprite, (sfVector2f){opponent->xPos, opponent->yPos});
	
		} else if(//at the left of player's car

			sfSprite_getPosition(playerCar).x - (sfSprite_getPosition(opponentCar).x + sfSprite_getGlobalBounds(opponentCar).width) <= 0 &&
			sfSprite_getPosition(playerCar).x - sfSprite_getPosition(opponentCar).x >= 0 &&
			sfSprite_getPosition(playerCar).x - sfSprite_getPosition(opponentCar).x <= sfSprite_getGlobalBounds(opponentCar).width + CAR_ROTATION_OFFSET
		)
		{
			opponent->isCrashed = 1;
			opponent->xPos = sfSprite_getPosition(playerCar).x - sfSprite_getGlobalBounds(opponentCar).width;
			sfSprite_setPosition(opponent->sprite, (sfVector2f){opponent->xPos, opponent->yPos});
		}
	}
	if(opponent->isCrashed && !opponent->wasCrashed) opponent->wasCrashed = 1;
	opponent->prevY = sfSprite_getPosition(opponentCar).y;
}

void reset(Player* player, sfSprite* carSprite, float* timeLimit, float* timer, float* elapsedSeconds, int* score, float* currentCurvature, float* accumulatedCurvature){
	
	(*player).startedGame = 0;
	(*player).curvature = 0;
	(*player).curvDifference = 0;
	(*player).position = 0;
	(*player).speed = 0;
	
	sfSprite_setTextureRect(carSprite, (sfIntRect){0, 0, CAR_WIDTH, CAR_HEIGHT});
	
	*timeLimit = GAME_TIME;
	*timer = 0;
	*elapsedSeconds = 0;
	
	*score = 0;
	
	*currentCurvature = 0;
	*accumulatedCurvature = 0;
}
