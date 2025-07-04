#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#define MUNCHIECOUNT 88
#define GHOSTCOUNT 4

// Just need to include main header file
#include "S2D/S2D.h"
#include <string>

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;


//Structure Definition
struct Player
{
	float cPacmanSpeed;
	float speedMultiplier;
	int currentFrameTime;
	int direction;
	int frame;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
	const int cFrameTime = 150;
	bool invertAnim;
	bool dead;

	//Extra functions
	int availableBoosts;
	int boostTime;
	int powerUpTime;
	int availableLives;
};

struct Enemy
{
	int frameCount;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
	const int cFrameTime = 500;
	int currentFrameTime;
	bool isEaten;
	int pointWorth;
};

struct MovingEnemy
{
	Vector2* position;
	Vector2* scatterTile;
	Texture2D* texture;
	Texture2D* scaredTex;
	Rect* sourceRect;
	int direction;
	float speed;
	const int cFrameTime = 500;
	int currentFrameTime;
	int frame;
	bool isEaten;
	int ressurrectionCooldown;
	int pointWorth;
	Vector2* target;
};

struct Wall
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
};

struct Menu
{
	//use of both paused and keydown prevents screen flickering
	bool keyDown;
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
};

enum State
{
	MainMenu = 0,
	Playing,
	Pause,
	Win,
	Lose,
	HighScore
};

//for high score table
struct ScoreEntry
{
	int order = 10;
	string name;
	int score;
};

//Class Definition
class Pacman : public Game
{
private:

	void LoadLevel();
	void BuildLevel();
	void RestartLevel();
	void ResetPositions();
	//Input methods
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	//Check methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckStart(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckWin();
	void CheckHighScore(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckRestart(Input::KeyboardState* state, Input::Keys pauseKey);

	void CheckCollisions(int elapsedTime);
	void CheckViewportCollision();
	void CheckGhostCollisions();
	bool CheckObjectCollision(Enemy* object);
	void CheckPacWallCollision(int elapsedTime);

	//scoring
	void InputName(vector<ScoreEntry>&);
	stringstream DisplayScores(vector<ScoreEntry>&);
	void LoadScores(vector<ScoreEntry>&);
	void SortScores(vector<ScoreEntry>&, ScoreEntry&);
	void SaveScores(vector<ScoreEntry>&);

	//Update methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(int elapsedTime, int index);
	void UpdatePowMunchie(int elapsedTime, int index);

	//PathFinding
	void UpdateGreen(MovingEnemy*);
	void UpdateRed(MovingEnemy*);
	void UpdatePink(MovingEnemy*);
	void UpdateOrange(MovingEnemy*);
	void UpdateGhost(MovingEnemy*, int elapsedTime);
	void RetreatGhost(MovingEnemy*);
	void PathfindTarget(MovingEnemy*);

	vector<int> GetOptions(MovingEnemy*);
	bool CheckPosition(Vector2, int direction);
	int GetBestMove(MovingEnemy*, vector<int> &options);

	Player* _pacman;
	Enemy** _munchies;
	Enemy** _powMunchies;
	Enemy* _cherry;
	MovingEnemy* _ghosts[GHOSTCOUNT];

	vector<string>* lines;
	vector<vector<Wall*>>* _walls;

	vector<ScoreEntry> scores;
	int score;
	int munchieCount;
	int powMunchieCount;
	int numMunchies;
	int numPowMunchies;
	int invasionCooldown = 0;

	// Position for String
	Vector2* _stringPosition;

	bool _hasCollision;
	bool _powerUpActive;

	// Data for menus
	Menu* _pauseMenu;
	Menu* _startMenu;
	Menu* _winMenu;
	Menu* _loseMenu;
	Menu* _scoreboard;

	State _gameState;

	SoundEffect* _pop;
	SoundEffect* _music;
	SoundEffect* _death;
	SoundEffect* _warp;
	SoundEffect* _win;
	SoundEffect* _collision;
	SoundEffect* _powerUp;

public:

	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};