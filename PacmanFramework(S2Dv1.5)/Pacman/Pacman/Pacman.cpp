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

	//munchies
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameCount = rand() % 1;
	}

	//cherry
	_cherry = new Enemy();
	_cherry->currentFrameTime = 0;
	_cherry->frameCount = rand() % 1;
	_cherry->isEaten = false;

	//ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.2f;
		_ghosts[i]->frame = 0;
		_ghosts[i]->currentFrameTime = 0;
		_ghosts[i]->target = new Vector2(-1,-1);
	}
	_ghosts[1]->direction = 1;
	_ghosts[3]->target = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));

	_hasCollision = true;

	_pauseMenu = new Menu();
	_pauseMenu->active = false;
	_pauseMenu->keyDown = false;

	_startMenu = new Menu();
	_startMenu->active = true;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
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
	delete _menuBackground;
	delete _menuRectangle;
	delete _menuStringPosition;
	delete _startMenu;
	delete _pauseMenu;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.png", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchies
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.tga", true);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->texture = munchieTex;
		_munchies[i]->sourceRect = new Rect(0.0f, 0.0f, 12, 12);
		_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	//Load Cherry
	_cherry->texture = new Texture2D;
	_cherry->texture->Load("Textures/Cherry.png", false);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherry->position = new Vector2(500.0f, 450.0f);

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

	LoadLevel();

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// set menu parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
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
				wall->position = new Vector2(x * 32, y * 32);
				wall->texture = wallTex;
				wall->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
				(*_walls)[x][y] = wall;
			}
			else
			{
				(*_walls)[x][y] = nullptr;
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
	else
	{
		CheckPaused(keyboardState, Input::Keys::P);

		if (!_pauseMenu->active) {

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
			CheckGhostCollisions();

			CheckViewportCollision();

			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				if (_munchies[i] == NULL)
				{
					continue;
				}
				if (_munchies[i]->position->X < _pacman->position->X + _pacman->sourceRect->Width &&
					_munchies[i]->position->X + _munchies[i]->sourceRect->Width > _pacman->position->X &&
					_munchies[i]->position->Y < _pacman->position->Y + _pacman->sourceRect->Height &&
					_munchies[i]->position->Y + _munchies[i]->sourceRect->Height > _pacman->position->Y)
				{
					delete _munchies[i]->sourceRect;
					delete _munchies[i]->position;
					delete _munchies[i];
					_munchies[i] = NULL;
				}
			}
			if (_cherry->position->X < _pacman->position->X + _pacman->sourceRect->Width &&
				_cherry->position->X + _cherry->sourceRect->Width > _pacman->position->X &&
				_cherry->position->Y < _pacman->position->Y + _pacman->sourceRect->Height &&
				_cherry->position->Y + _cherry->sourceRect->Height > _pacman->position->Y)
			{
				_cherry->isEaten = true;
			}
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;

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

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (_startMenu->active) {
		std::stringstream menuStream;
		menuStream << "PACMAN!\nPress SPACE to start.";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Yellow);
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

	//mouse input - PACMAN MOVEMENT OPTIONAL IMPLEMENTATION: MOVEMENT WITH MOUSE
	//NOTE: if keeping this code, can put directional movement into functions for code reusability?
	//NOTE 2: pacman can move diagonally using this method, if the mouse stays still & clicked for long enough. Solution?
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		float xDist = _pacman->position->X - mouseState->X;
		float yDist = _pacman->position->Y - mouseState->Y;

		float xSquare = xDist * xDist;
		float ySquare = yDist * yDist;

		if (xSquare > ySquare)
		{
			//move in x direction
			if (xDist >= 0)
			{
				_pacman->position->X -= pacmanSpeed; //Moves Pacman across X axis
				_pacman->direction = 2;
			}
			else
			{
				_pacman->position->X += pacmanSpeed; //Moves Pacman across X axis
				_pacman->direction = 0;
			}
		}
		else
		{
			// move in y direction
			if (yDist >= 0)
			{
				_pacman->position->Y -= pacmanSpeed; //Moves Pacman across X axis
				_pacman->direction = 3;
			}
			else
			{
				_pacman->position->Y += pacmanSpeed; //Moves Pacman across X axis
				_pacman->direction = 1;
			}
		}
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

	// Check if SHIFT key pressed
	//if (keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT))
	//{
		//apply multiplier
		//_pacman->speedMultiplier = 2.0f;
	//}
	//else
	//{
	//	_pacman->speedMultiplier = 1.0f;
	//}

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

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pauseMenu->keyDown) {
		_pauseMenu->keyDown = true;
		_pauseMenu->active = !_pauseMenu->active;
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

void Pacman::CheckViewportCollision()
{
	if (!_hasCollision)
	{
		//prevents movement off right edge
		if (_pacman->position->X - _pacman->sourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width
		{
			//teleport to left wall
			_pacman->position->X = 0 - _pacman->sourceRect->Width;
		}
		//prevent movement off left edge
		if (_pacman->position->X + _pacman->sourceRect->Width < 0)
		{
			//teleport to right wall
			_pacman->position->X = Graphics::GetViewportWidth();
		}
		// off bottom edge
		if (_pacman->position->Y > Graphics::GetViewportHeight()) //1024 is game width
		{
			//teleport to top wall
			_pacman->position->Y = 0 - _pacman->sourceRect->Height;
		}
		// off top edge
		if (_pacman->position->Y + _pacman->sourceRect->Height < 0)
		{
			//teleport to bottom wall
			_pacman->position->Y = Graphics::GetViewportHeight();
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
	if (ghost->position->X < ghost->target->X + 1 &&
		ghost->position->X + ghost->sourceRect->Width > ghost->target->X &&
		ghost->position->Y < ghost->target->Y + 1 &&
		ghost->position->Y + ghost->sourceRect->Height > ghost->target->Y)
	{
		ghost->target = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
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
		}
	}
}