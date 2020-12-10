#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <SFML/Graphics.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MIDDLE 0.5//horisontal center of the screen
#define CAR_WIDTH 69
#define CAR_HEIGHT 83
#define CAR_ROTATION_OFFSET 15
#define SHOULD_CHANGE_CAR 0.09
#define MAX_ROAD_WIDTH 0.7
#define MIN_ROAD_WIDTH 0.1
#define NUM_SEGMENTS 8//number of road segments
#define GAME_TIME 60//if >=100 showTimer function should be edited
#define MIN_DIST 10//min distance from player's car to opponent's car
#define MAX_DIST 41//max distance from player's car to opponent's car
#define OPPONENT_MIN_SPEED 0.4//min speed of opponent's car
#define OPPONENT_SPEED_DECREESE 0.5
#define OPPONENT_SPEED_INCREASE 0.25
#define PLAYER_MAX_SPEED 1
#define PLAYER_MIN_SPEED 0
#define PLAYER_CURV_CHANGE_SPEED 0.4
#define PLAYER_CURV_DIFFERENCE_LIMIT 0.4//if player's curvature difference is > than this number, player is on the grass
#define PLAYER_DISTANCE_CHANGE_SPEED 50
#define PLAYER_SPEED_DECREASE 0.25
#define DISTANCE_CHANGE_SPEED 50
#define MIN_SCALE 0.4
#define MAX_SCALE 1
#define LEFT_TURN_CURVATURE -0.2
#define RIGHT_TURN_CURVATURE 0.2
#define X_POS_CHANGE_SPEED 3
#define GRASS_SLOW_DOWN 3
#define SQUARE 2
#define CUBE 3

/*extern sfIntRect redCarTextureForward;
extern sfIntRect redCarTextureLeft;
extern sfIntRect redCarTextureRight;

extern sfIntRect violetCarTextureForward;
extern sfIntRect yellowCarTextureForward;

extern sfIntRect violetCarTextureLeft;
extern sfIntRect yellowCarTextureLeft;

extern sfIntRect violetCarTextureRight;
extern sfIntRect yellowCarTextureRight;*/

typedef struct {
	int startedGame;
	float speed;
	float curvature;
	float curvDifference;//difference between player's curvature and road curvature
	float position;
} Player;

typedef struct {
	float length;
	float curvature;
} roadParts;

typedef enum {
	violet,
	yellow,
} colors;

//directions to which opponent's car move within a road
typedef enum {
	left,
	right
} moveDirections;

typedef struct {
	int isHidden;
	int isCrashed;
	int wasCrashed;
	float initialPosition;
	float currentPosition;
	float perspective;
	float initSpeed;
	float curSpeed;
	int carColor;
	float yPos;
	float prevY;
	float xPos;//can be replaced by direct calls to SFML API where needed
	sfSprite* sprite;
} opponents;

roadParts* createRoad();

float getRoadLength(roadParts* segments, int numSegments);

//gets curvature of the current road segment
//curvature defines a radius of road's turn
float getDestCurvature(float playerPosition, roadParts* segments);

opponents* createOpponent();

void handleOpponent(opponents* opponent, Player player, float elapsedSeconds, float currentCurvature, int moveDirection);

void handlePlayerMovements(sfSprite* carSprite, float elapsedSeconds, Player* player, float roadLength);

//checks if a player crashed into another car
//currently only left and right crashes are correctly handled
//handling top and bottom crashes can be added to better gameplay
void handleCollisions(Player* player, sfSprite* playerCar, sfSprite* opponentCar, float elapsedSeconds, opponents* opponent);

void reset(Player* player, sfSprite* carSprite, float* timeLimit, float* timer, float* elapsedSeconds, int* score, float* currentCurvature, float* accumulatedCurvature);
