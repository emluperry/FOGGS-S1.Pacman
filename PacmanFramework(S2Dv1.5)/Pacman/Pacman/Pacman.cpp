#include "Pacman.h"

#include <iostream>
#include <sstream>
#include <time.h>
#include <fstream>
#include <string>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	srand(time(NULL));

	//pacman
	_pacman = new Player();
	_pacman->dead = false;
	_pacman->cPacmanSpeed = 0.1f;
	_pacman->speedMultiplier = 1.0f;
	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;
	_pacman->invertAnim = false;
	_pacman->availableBoosts = 3;
	_pacman->boostTime = 3000;

	//munchies
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameCount = rand() % 1;
		_munchies[i]->pointWorth = 100;
	}
	numMunchies = MUNCHIECOUNT;

	//cherry
	_cherry = new Enemy();
	_cherry->currentFrameTime = 0;
	_cherry->frameCount = rand() % 1;
	_cherry->isEaten = false;
	_cherry->pointWorth = 1000;

	//ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.1f;
		_ghosts[i]->frame = 0;
		_ghosts[i]->currentFrameTime = 0;
		_ghosts[i]->target = new Vector2(-1,-1);
	}

	//can be toggled by TAB
	_hasCollision = false;
	score = 0;

	_pauseMenu = new Menu();
	_pauseMenu->active = false;
	_pauseMenu->keyDown = false;

	_startMenu = new Menu();
	_startMenu->active = true;

	_winMenu = new Menu();
	_winMenu->active = false;

	_loseMenu = new Menu();
	_loseMenu->active = false;

	_pop = new SoundEffect();
	_music = new SoundEffect();
	_death = new SoundEffect();
	_warp = new SoundEffect();
	_win = new SoundEffect();
	_collision = new SoundEffect();

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
	if (!Audio::IsInitialised())
	{
		cout << "Audio is not initialised." << endl;
	}
	if (!_pop->IsLoaded())
	{
		cout << "_pop member sfx has not loaded." << endl;
	}
}

Pacman::~Pacman()
{
	delete (*_walls)[0][0]->texture;
	int width = _walls->size();
	int height = _walls->at(0).size();
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			delete (*_walls)[x][y]->sourceRect;
			delete (*_walls)[x][y]->position;
		}
	}
	_walls->clear();
	delete _walls;

	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman->position;
	delete _pacman;

	delete _munchies[0]->texture;
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (_munchies[i] == NULL)
		{
			continue;
		}
		delete _munchies[i]->sourceRect;
		delete _munchies[i]->position;
		delete _munchies[i];
	}
	delete[] _munchies;

	delete _cherry->texture;
	delete _cherry->sourceRect;
	delete _cherry;

	delete _ghosts[0]->texture;
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->sourceRect;
		delete _ghosts[i]->position;
		delete _ghosts[i]->target;
		delete _ghosts[i];
	}
	delete[] _ghosts;

	delete _stringPosition;

	delete _startMenu->_menuBackground;
	delete _startMenu->_menuRectangle;
	delete _startMenu->_menuStringPosition;

	delete _startMenu;
	delete _pauseMenu;
	delete _winMenu;
	delete _loseMenu;

	delete _pop;
	delete _death;
	delete _music;
	delete _warp;
	delete _win;
	delete _collision;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.png", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchies - now loaded in LoadLevel
	//Texture2D* munchieTex = new Texture2D();
	//munchieTex->Load("Textures/Munchie.tga", true);
	//for (int i = 0; i < MUNCHIECOUNT; i++)
	//{
	//	_munchies[i]->texture = munchieTex;
	//	_munchies[i]->sourceRect = new Rect(0.0f, 0.0f, 12, 12);
	//	_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	//}

	//Load Cherry
	_cherry->texture = new Texture2D;
	_cherry->texture->Load("Textures/Cherry.png", false);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherry->position = new Vector2(500.0f, 472.0f);

	//load Ghosts
	_ghosts[0]->texture = new Texture2D();
	_ghosts[0]->texture->Load("Textures/GhostGreen.png", false);
	_ghosts[1]->texture = new Texture2D();
	_ghosts[1]->texture->Load("Textures/GhostRed.png", false);
	_ghosts[2]->texture = new Texture2D();
	_ghosts[2]->texture->Load("Textures/GhostPink.png", false);
	_ghosts[3]->texture = new Texture2D();
	_ghosts[3]->texture->Load("Textures/GhostOrange.png", false);
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}
	_ghosts[1]->direction = 1;
	//_ghosts[1]->target = _pacman->position;
	_ghosts[3]->target = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));

	LoadLevel();

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// set menu parameters
	Texture2D* _menuBg = new Texture2D();
	_menuBg->Load("Textures/Transparency.png", false);
	Rect* _menuRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	Vector2* _menuStringPos = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_startMenu->_menuBackground = _menuBg;
	_startMenu->_menuRectangle = _menuRect;
	_startMenu->_menuStringPosition = _menuStringPos;

	_pauseMenu->_menuBackground = _menuBg;
	_pauseMenu->_menuRectangle = _menuRect;
	_pauseMenu->_menuStringPosition = _menuStringPos;

	_loseMenu->_menuBackground = _menuBg;
	_loseMenu->_menuRectangle = _menuRect;
	_loseMenu->_menuStringPosition = _menuStringPos;

	_winMenu->_menuBackground = _menuBg;
	_winMenu->_menuRectangle = _menuRect;
	_winMenu->_menuStringPosition = _menuStringPos;

	_pop->Load("Sounds/pop.wav");
	_death->Load("Sounds/death.wav");
	_warp->Load("Sounds/warp.wav");
	// source: https://freesound.org/people/LittleRobotSoundFactory/sounds/270319/
	_win->Load("Sounds/win.wav");
	_collision->Load("Sounds/collision.wav");
	_collision->SetPitch(0.1);
	_music->Load("Sounds/music.wav");
	_music->SetLooping(true);
	Audio::Play(_music);
}

void Pacman::LoadLevel()
{
	// Load the level
	vector<string>* lines = new vector<string>();
	fstream stream;
	stringstream ss;
	ss << "level.txt";
	stream.open(ss.str(), fstream::in);

	char* line = new char[256];
	stream.getline(line, 256);
	string* sline = new string(line);
	int width = sline->size();
	while (!stream.eof())
	{
		lines->push_back(*sline);
		stream.getline(line, 256);
		delete sline;
		sline = new string(line);
	}

	delete[] line;
	delete sline;

	// Allocate the tile grid.
	_walls = new vector<vector<Wall*>>(width, vector<Wall*>(lines->size()));

	Texture2D* wallTex = new Texture2D();
	wallTex->Load("Textures/wall.png", false);
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.tga", true);
	int index = 0;
	// Loop over every tile position,
	for (int y = 0; y < lines->size(); ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			// to load each tile.
			char tileType = lines->at(y)[x];
			if (tileType == 'x')
			{
				Wall* wall = new Wall();
				wall->position = new Vector2(-96 + x * 32, y * 32);
				wall->texture = wallTex;
				wall->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
				(*_walls)[x][y] = wall;
			}
			else
			{
				(*_walls)[x][y] = nullptr;
			}
			
			if (tileType == 'o')
			{
				_munchies[index]->texture = munchieTex;
				_munchies[index]->sourceRect = new Rect(0.0f, 0.0f, 12, 12);
				_munchies[index]->position = new Vector2(-96 + x * 32, y * 32);
				index++;
			}
		}
	}

	delete lines;
	stream.close();
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	//Gets current state of mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();

	_pacman->boostTime += elapsedTime;

	if (_startMenu->active)
	{
		CheckStart(keyboardState, Input::Keys::SPACE);
	}
	else if (_winMenu->active || _loseMenu->active)
	{
		return;
	}
	else
	{
		CheckPaused(keyboardState, Input::Keys::P);

		if (!_pauseMenu->active && !_loseMenu->active && !_winMenu->active) {

			Input(elapsedTime, keyboardState, mouseState);

			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				if (_munchies[i] == NULL)
				{
					continue;
				}
				UpdateMunchie(elapsedTime, i);
			}

			UpdatePacman(elapsedTime);

			UpdateGreen(_ghosts[0], elapsedTime);
			UpdateRed(_ghosts[1], elapsedTime);
			UpdatePink(_ghosts[2], elapsedTime);
			UpdateOrange(_ghosts[3], elapsedTime);
			for (int i = 0; i < GHOSTCOUNT; i++)
			{
				UpdateGhost(_ghosts[i], elapsedTime);
			}

			CheckCollisions(elapsedTime);
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << (int)_pacman->position->X << " Y: " << (int)_pacman->position->Y << " Boosts: " << _pacman->availableBoosts << " Score: " << score;

	SpriteBatch::BeginDraw(); // Starts Drawing

	int width = _walls->size();
	int height = _walls->at(0).size();
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if ((*_walls)[x][y] != nullptr)
			{
				SpriteBatch::Draw((*_walls)[x][y]->texture, (*_walls)[x][y]->position, (*_walls)[x][y]->sourceRect);
			}
		}
	}

	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	}

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (_munchies[i] == NULL)
		{
			continue;
		}
		SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect); // Draws munchie
	}

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect); // Draws ghosts
	}

	if (!_cherry->isEaten)
	{
		SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect); //Draws cherry
	}
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	if (_pauseMenu->active) {
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_pauseMenu->_menuBackground, _pauseMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _pauseMenu->_menuStringPosition, Color::Red);
	}

	if (_startMenu->active) {
		std::stringstream menuStream;
		menuStream << "PACMAN!\nPress SPACE to start.";

		SpriteBatch::Draw(_startMenu->_menuBackground, _startMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _startMenu->_menuStringPosition, Color::Yellow);
	}

	if (_winMenu->active) {
		std::stringstream menuStream;
		menuStream << "YOU WIN!\nFinal score: " << score;

		SpriteBatch::Draw(_winMenu->_menuBackground, _winMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _winMenu->_menuStringPosition, Color::Yellow);
	}

	if (_loseMenu->active) {
		std::stringstream menuStream;
		menuStream << "YOU LOSE...\nFinal score: " << score;

		SpriteBatch::Draw(_winMenu->_menuBackground, _winMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _winMenu->_menuStringPosition, Color::Yellow);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState, Input::MouseState* mouseState)
{
	float pacmanSpeed = _pacman->cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	//mouse input - REPOSITIONS CHERRY
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherry->position->X = mouseState->X;
		_cherry->position->Y = mouseState->Y;
	}

	//mouse input - SPEED BOOST OPTIONAL IMPLEMENTATION: LIMITED TIME/USE
	if (mouseState->RightButton == Input::ButtonState::PRESSED && _pacman->boostTime >= 3000 && _pacman->availableBoosts > 0)
	{
		_pacman->speedMultiplier = 2.0f;
		_pacman->availableBoosts--;
		_pacman->boostTime = 0;
	}

	if (_pacman->boostTime >= 3000)
	{
		_pacman->speedMultiplier = 1.0f;
	}

	// Checks if D key is pressed
	if (keyboardState->IsKeyDown(Input::Keys::D)) {
		_pacman->position->X += pacmanSpeed; //Moves Pacman across X axis
		_pacman->direction = 0;
	}
	// Checks if A key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::A)) {
		_pacman->position->X -= pacmanSpeed; //Moves Pacman across X axis
		_pacman->direction = 2;
	}
	// Checks if W key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::W)) {
		_pacman->position->Y -= pacmanSpeed; //Moves Pacman across Y axis
		_pacman->direction = 3;
	}
	// Checks if S key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::S)) {
		_pacman->position->Y += pacmanSpeed; //Moves Pacman across Y axis
		_pacman->direction = 1;
	}

	//Checks if R key is pressed. Randomly moves cherry based on this.
	if (keyboardState->IsKeyDown(Input::Keys::R))
	{
		_cherry->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	//Toggles wall collision and wall wrapping.
	if (keyboardState->IsKeyDown(Input::Keys::TAB))
				_hasCollision = !_hasCollision;
}

void PacmanDash()
{
	// Check if SHIFT key pressed
    //if (keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT))
    //{
	    //apply multiplier
	    //_pacman->speedMultiplier = 2.0f;
    //}
    //else
    //{
    	//_pacman->speedMultiplier = 1.0f;
    //}
}

void PacmanFollowMouse()
{
	//mouse input - PACMAN MOVEMENT OPTIONAL IMPLEMENTATION: MOVEMENT WITH MOUSE
	//NOTE: if keeping this code, can put directional movement into functions for code reusability?
	//NOTE 2: pacman can move diagonally using this method, if the mouse stays still & clicked for long enough. Solution?
	//if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	//{
	//	float xDist = _pacman->position->X - mouseState->X;
	//	float yDist = _pacman->position->Y - mouseState->Y;

	//	float xSquare = xDist * xDist;
	//	float ySquare = yDist * yDist;

	//	if (xSquare > ySquare)
	//	{
	//		//move in x direction
	//		if (xDist >= 0)
	//		{
	//			_pacman->position->X -= pacmanSpeed; //Moves Pacman across X axis
	//			_pacman->direction = 2;
	//		}
	//		else
	//		{
	//			_pacman->position->X += pacmanSpeed; //Moves Pacman across X axis
	//			_pacman->direction = 0;
	//		}
	//	}
	//	else
	//	{
	//		// move in y direction
	//		if (yDist >= 0)
	//		{
	//			_pacman->position->Y -= pacmanSpeed; //Moves Pacman across X axis
	//			_pacman->direction = 3;
	//		}
	//		else
	//		{
	//			_pacman->position->Y += pacmanSpeed; //Moves Pacman across X axis
	//			_pacman->direction = 1;
	//		}
	//	}
	//}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pauseMenu->keyDown) {
		_pauseMenu->keyDown = true;
		_pauseMenu->active = !_pauseMenu->active;

		if (_pauseMenu->active == true)
		{
			Audio::Pause(_music);
		}
		else
		{
			Audio::Resume(_music);
		}
	}
	if (state->IsKeyUp(pauseKey)) {
		_pauseMenu->keyDown = false;
	}
}

void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
{
	if (state->IsKeyDown(startKey)) {
		_startMenu->active = false;
	}
}

void Pacman::CheckWin()
{
	if (numMunchies == 0)
	{
		_winMenu->active = true;
		Audio::Stop(_music);
		Audio::Play(_win);
	}
}

void Pacman::UpdatePacman(int elapsedTime)
{
	_pacman->currentFrameTime += elapsedTime;
	if (_pacman->currentFrameTime > _pacman->cFrameTime) {
		if (!_pacman->invertAnim) {
			_pacman->frame++; //increases animation frame
			if (_pacman->frame >= 3)
				_pacman->invertAnim = !_pacman->invertAnim; //four frames starting from 0, swap frame change order after 4th frame
		}
		else {
			_pacman->frame--;
			if (_pacman->frame <= 0)
				_pacman->invertAnim = !_pacman->invertAnim;
		}

		_pacman->currentFrameTime -= _pacman->cFrameTime;
	}

	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction; // change source rect based on direction and frame.
}

void Pacman::UpdateMunchie(int elapsedTime, int index)
{
	Enemy* _munchie = _munchies[index];
	_munchie->currentFrameTime += elapsedTime;
	if (_munchie->currentFrameTime > _munchie->cFrameTime) {
		_munchie->frameCount++;
		if (_munchie->frameCount >= 2) {
			_munchie->frameCount = 0;
		}
		_munchie->currentFrameTime -= _munchie->cFrameTime;
	}

	_munchie->sourceRect->X = _munchie->sourceRect->Width * _munchie->frameCount; // change munchie sprite based on time
	_cherry->sourceRect->X = _cherry->sourceRect->Width * _munchie->frameCount; // change cherry sprite based on munchie sprite/time
}

//horizontal patrol
//vertical patrol
//border patrol
//random patrol

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	ghost->currentFrameTime += elapsedTime;
	if (ghost->currentFrameTime > ghost->cFrameTime) {
		ghost->frame++; //increases animation frame
		if (ghost->frame >= 3)
			ghost->frame = 0;

		ghost->currentFrameTime -= ghost->cFrameTime;
	}

	ghost->sourceRect->X = ghost->sourceRect->Width * ghost->frame;
	ghost->sourceRect->Y = ghost->sourceRect->Height * ghost->direction; // change source rect based on direction and frame.
}

void Pacman::UpdateGreen(MovingEnemy* ghost, int elapsedTime)
{
	//horizontal
	if (ghost->direction == 0)
	{
		ghost->position->X += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 2)
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}

	if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth())
	{
		ghost->direction = 2;
	}
	else if (ghost->position->X <= 0)
	{
		ghost->direction = 0;
	}
}

void Pacman::UpdateRed(MovingEnemy* ghost, int elapsedTime)
{
	//vertical
	if (ghost->direction == 1)
	{
		ghost->position->Y += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 3)
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
	}

	if (ghost->position->Y + ghost->sourceRect->Height >= Graphics::GetViewportHeight())
	{
		ghost->direction = 3;
	}
	else if (ghost->position->Y <= 0)
	{
		ghost->direction = 1;
	}
}

void Pacman::UpdatePink(MovingEnemy* ghost, int elapsedTime)
{
	//horizontal
	if (ghost->direction == 0)
	{
		ghost->position->X += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 1)
	{
		ghost->position->Y += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 2)
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 3)
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
	}

	if (ghost->position->X + ghost->sourceRect->Width > Graphics::GetViewportWidth())
	{
		ghost->position->X = Graphics::GetViewportWidth() - ghost->sourceRect->Width;
		ghost->direction = 1;
	}
	else if (ghost->position->Y + ghost->sourceRect->Height > Graphics::GetViewportHeight())
	{
		ghost->position->Y = Graphics::GetViewportHeight() - ghost->sourceRect->Height;
		ghost->direction = 2;
	}
	else if (ghost->position->X < 0)
	{
		ghost->position->X = 0;
		ghost->direction = 3;
	}
	else if (ghost->position->Y < 0)
	{
		ghost->position->Y = 0;
		ghost->direction = 0;
	}
}

void Pacman::UpdateOrange(MovingEnemy* ghost, int elapsedTime)
{
	MoveTowardsTarget(ghost, elapsedTime);

	if (ghost->position->X < ghost->target->X + 1 &&
		ghost->position->X + ghost->sourceRect->Width > ghost->target->X &&
		ghost->position->Y < ghost->target->Y + 1 &&
		ghost->position->Y + ghost->sourceRect->Height > ghost->target->Y)
	{
		ghost->target = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}
}

void Pacman::CheckProximity(MovingEnemy* ghost, int elapsedTime)
{
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		if (_ghosts[i]->position->X < _pacman->position->X + _pacman->sourceRect->Width &&
			_ghosts[i]->position->X + _ghosts[i]->sourceRect->Width > _pacman->position->X &&
			_ghosts[i]->position->Y < _pacman->position->Y + _pacman->sourceRect->Height &&
			_ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height > _pacman->position->Y)
		{
			//mode = chase
		}
		else
		{
			//mode = patrol
		}
	}
}

void Pacman::ChasePacman(MovingEnemy* ghost, int elapsedTime)
{
	MoveTowardsTarget(ghost, elapsedTime);
	ghost->target = _pacman->position;
}

void Pacman::MoveTowardsTarget(MovingEnemy* ghost, int elapsedTime)
{
	float xDist = ghost->position->X - ghost->target->X;
	float yDist = ghost->position->Y - ghost->target->Y;

	float xSquare = xDist * xDist;
	float ySquare = yDist * yDist;

	if (xSquare > ySquare)
	{
		//move in x direction
		if (xDist >= 0)
		{
			ghost->position->X -= ghost->speed * elapsedTime;
			ghost->direction = 2;
		}
		else
		{
			ghost->position->X += ghost->speed * elapsedTime;
			ghost->direction = 0;
		}
	}
	else
	{
		// move in y direction
		if (yDist >= 0)
		{
			ghost->position->Y -= ghost->speed * elapsedTime;
			ghost->direction = 3;
		}
		else
		{
			ghost->position->Y += ghost->speed * elapsedTime;
			ghost->direction = 1;
		}
	}
}

void Pacman::CheckCollisions(int elapsedTime)
{
	CheckGhostCollisions();
	CheckViewportCollision();
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (_munchies[i] == NULL)
		{
			continue;
		}
		if (CheckObjectCollision(_munchies[i]))
		{
			Audio::Play(_pop);
			score += _munchies[i]->pointWorth;
			delete _munchies[i]->sourceRect;
			delete _munchies[i]->position;
			delete _munchies[i];
			_munchies[i] = NULL;
			numMunchies--;
			CheckWin();
		}
	}
	if (_cherry->isEaten == false && CheckObjectCollision(_cherry))
	{
		score += _cherry->pointWorth;
		_cherry->isEaten = true;
	}
	CheckWallCollision(elapsedTime);
}

void Pacman::CheckViewportCollision()
{
	if (!_hasCollision)
	{
		//prevents movement off right edge
		if (_pacman->position->X - _pacman->sourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width
		{
			//teleport to left wall
			_pacman->position->X = 0 - _pacman->sourceRect->Width;
			Audio::Play(_warp);
		}
		//prevent movement off left edge
		if (_pacman->position->X + _pacman->sourceRect->Width < 0)
		{
			//teleport to right wall
			_pacman->position->X = Graphics::GetViewportWidth();
			Audio::Play(_warp);
		}
		// off bottom edge
		if (_pacman->position->Y > Graphics::GetViewportHeight()) //1024 is game width
		{
			//teleport to top wall
			_pacman->position->Y = 0 - _pacman->sourceRect->Height;
			Audio::Play(_warp);
		}
		// off top edge
		if (_pacman->position->Y + _pacman->sourceRect->Height < 0)
		{
			//teleport to bottom wall
			_pacman->position->Y = Graphics::GetViewportHeight();
			Audio::Play(_warp);
		}
	}
	else if (_hasCollision)
	{
		//prevents movement off right edge
		if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width
		{
			//block movement
			_pacman->position->X = Graphics::GetViewportWidth() - +_pacman->sourceRect->Width;
		}
		//prevent movement off left edge
		if (_pacman->position->X < 0)
		{
			//teleport to right wall
			_pacman->position->X = 0;
		}
		// off bottom edge
		if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight()) //1024 is game width
		{
			//block movement
			_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
		}
		// off top edge
		if (_pacman->position->Y < 0)
		{
			//block movement
			_pacman->position->Y = 0;
		}
	}
}

void Pacman::CheckGhostCollisions()
{
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		if (_ghosts[i]->position->X < _pacman->position->X + _pacman->sourceRect->Width &&
			_ghosts[i]->position->X + _ghosts[i]->sourceRect->Width > _pacman->position->X &&
			_ghosts[i]->position->Y < _pacman->position->Y + _pacman->sourceRect->Height &&
			_ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height > _pacman->position->Y)
		{
			_pacman->dead = true;
			i = GHOSTCOUNT;
			_loseMenu->active = true;
			Audio::Play(_death);
			Audio::Stop(_music);
		}
	}
}

bool Pacman::CheckObjectCollision(Enemy* object)
{
	if (object->position->X < _pacman->position->X + _pacman->sourceRect->Width &&
		object->position->X + object->sourceRect->Width > _pacman->position->X &&
		object->position->Y < _pacman->position->Y + _pacman->sourceRect->Height &&
		object->position->Y + object->sourceRect->Height > _pacman->position->Y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Pacman::CheckWallCollision(int elapsedTime)
{
	int maxWalls = _walls->size();
	int width = _walls->size();
	int height = _walls->at(0).size();
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if ((*_walls)[x][y] == NULL)
			{
				continue;
			}
			if ((*_walls)[x][y]->position->X < _pacman->position->X + _pacman->sourceRect->Width &&
				(*_walls)[x][y]->position->X + (*_walls)[x][y]->sourceRect->Width > _pacman->position->X &&
				(*_walls)[x][y]->position->Y < _pacman->position->Y + _pacman->sourceRect->Height &&
				(*_walls)[x][y]->position->Y + (*_walls)[x][y]->sourceRect->Height > _pacman->position->Y)
			{
				if (_collision->GetState() != SoundEffectState::PLAYING)
				{
					Audio::Play(_collision);
				}
				if (_pacman->direction == 0)
				{
					_pacman->position->X -= _pacman->cPacmanSpeed * _pacman->speedMultiplier * elapsedTime;
				}
				else if (_pacman->direction == 2)
				{
					_pacman->position->X += _pacman->cPacmanSpeed * _pacman->speedMultiplier * elapsedTime;
				}
				else if (_pacman->direction == 1)
				{
					_pacman->position->Y -= _pacman->cPacmanSpeed * _pacman->speedMultiplier * elapsedTime;
				}
				else if (_pacman->direction == 3)
				{
					_pacman->position->Y += _pacman->cPacmanSpeed * _pacman->speedMultiplier * elapsedTime;
				}
			}
		}
	}
}