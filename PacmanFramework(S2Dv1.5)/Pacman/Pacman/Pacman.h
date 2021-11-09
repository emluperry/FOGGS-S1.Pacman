#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#define MUNCHIECOUNT 50

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;


//Structure Definition
struct Player
{
	float speedMultiplier;
	int currentFrameTime;
	int direction;
	int frame;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
	const int cFrameTime = 150;
	bool invertAnim;
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
};

struct Menu
{
	//use of both paused and keydown prevents screen flickering
	bool active;
	bool keyDown;
};


//Class Definition
class Pacman : public Game
{
private:
	//Input methods
	void Input(int elapsedTime, Input::KeyboardState* state);

	//Check methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckStart(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();

	//Update methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(int elapsedTime, int index);

	// Data to represent Pacman
	Player* _pacman;

	// Data to represent Munchie
	Enemy* _munchies[MUNCHIECOUNT];

	// Data to rep. cherry
	Enemy* _cherry;

	// Position for String
	Vector2* _stringPosition;

	bool _hasCollision;

	// Data for menus
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;

	Menu* _pauseMenu;
	Menu* _startMenu;

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