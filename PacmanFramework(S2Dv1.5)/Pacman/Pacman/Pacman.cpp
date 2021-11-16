#include "Pacman.h"

#include <iostream>
#include <sstream>
#include <time.h>

// NOTE TO SELF: Pacman is dressed as a ghost. When creating ghost assets, make the ghosts try to dress as pacman! (Change colour of blue ghost to yellow)

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	srand(time(NULL));

	//Player pacman
	_pacman = new Player();
	_pacman->cPacmanSpeed = 0.1f;
	_pacman->speedMultiplier = 1.0f;
	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;
	_pacman->invertAnim = false;

	//Player ghosts
	_ghostA = new Player();
	_ghostA->cPacmanSpeed = 0.1f;
	_ghostA->speedMultiplier = 1.0f;
	_ghostA->direction = 0;
	_ghostA->currentFrameTime = 0;
	_ghostA->frame = 0;
	_ghostA->invertAnim = false;

	_ghostB = new Player();
	_ghostB->cPacmanSpeed = 0.1f;
	_ghostB->speedMultiplier = 1.0f;
	_ghostB->direction = 0;
	_ghostB->currentFrameTime = 0;
	_ghostB->frame = 0;
	_ghostB->invertAnim = false;

	//Munchies & collectables
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameCount = rand() % 1;
		_munchies[i]->isEaten = false;
	}

	_cherry = new Enemy();
	_cherry->currentFrameTime = 0;
	_cherry->frameCount = rand() % 1;
	_cherry->isEaten = false;

	_hasCollision = true; //walls do not wrap

	//menus
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
	delete _pacman->texture;
	delete _pacman->sourceRect;

	delete _ghostA->texture;
	delete _ghostA->sourceRect;
	delete _ghostB->texture;
	delete _ghostB->sourceRect;

	delete _munchies[0]->texture;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchies[i]->sourceRect;
		delete _munchies[i]->position;
		delete _munchies[i];
	}
	delete[] _munchies;

	delete _cherry->texture;
	delete _cherry->sourceRect;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.png", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Ghosts
	Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/Ghost.png", false);
	_ghostA->texture = ghostTex;
	_ghostA->position = new Vector2(100.0f, 100.0f);
	_ghostA->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	_ghostB->texture = ghostTex;
	_ghostB->position = new Vector2(100.0f, 300.0f);
	_ghostB->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

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

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// set menu parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
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
				UpdateMunchie(elapsedTime, i);
			}
			UpdatePacman(elapsedTime);

			CheckViewportCollision();

			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				if (_munchies[i]->position->X < _pacman->position->X + _pacman->sourceRect->Width &&
					_munchies[i]->position->X + _munchies[i]->sourceRect->Width > _pacman->position->X &&
					_munchies[i]->position->Y < _pacman->position->Y + _pacman->sourceRect->Height &&
					_munchies[i]->position->Y + _munchies[i]->sourceRect->Height > _pacman->position->Y)
				{
					_munchies[i]->isEaten = true;
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

	SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	SpriteBatch::Draw(_ghostA->texture, _ghostA->position, _ghostA->sourceRect);
	SpriteBatch::Draw(_ghostB->texture, _ghostB->position, _ghostB->sourceRect);

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		if (!_munchies[i]->isEaten)
		{
			SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect); // Draws munchie
		}
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
	// pacman use mouse, ghostA use WASD, ghostB use arrows
	float pacmanSpeed = _pacman->cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	//mouse input - REPOSITIONS CHERRY
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherry->position->X = mouseState->X;
		_cherry->position->Y = mouseState->Y;
	}

	//mouse input - PACMAN MOVEMENT OPTIONAL IMPLEMENTATION: MOVEMENT WITH MOUSE
	//NOTE: pacman can move diagonally using this method, if the mouse stays still & clicked for long enough. Solution?
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
				MoveLeft(_pacman, pacmanSpeed);
			}
			else
			{
				MoveRight(_pacman, pacmanSpeed);
			}
		}
		else
		{
			// move in y direction
			if (yDist >= 0)
			{
				MoveUp(_pacman, pacmanSpeed);
			}
			else
			{
				MoveDown(_pacman, pacmanSpeed);
			}
		}
	}

	//mouse input - SPEED BOOST OPTIONAL IMPLEMENTATION: LIMITED TIME/USE, speeds up pacman AND player ghosts
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
		MoveRight(_ghostA, pacmanSpeed);
		//_pacman->position->X += pacmanSpeed; //Moves Pacman across X axis
		//_pacman->direction = 0;
	}
	// Checks if A key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::A)) {
		MoveLeft(_ghostA, pacmanSpeed);
		//_pacman->position->X -= pacmanSpeed; //Moves Pacman across X axis
		//_pacman->direction = 2;
	}
	// Checks if W key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::W)) {
		MoveUp(_ghostA, pacmanSpeed);
		//_pacman->position->Y -= pacmanSpeed; //Moves Pacman across Y axis
		//_pacman->direction = 3;
	}
	// Checks if S key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::S)) {
		MoveDown(_ghostA, pacmanSpeed);
		//_pacman->position->Y += pacmanSpeed; //Moves Pacman across Y axis
		//_pacman->direction = 1;
	}


	// Checks if -> key is pressed
	if (keyboardState->IsKeyDown(Input::Keys::RIGHT)) {
		MoveRight(_ghostB, pacmanSpeed);
	}
	// Checks if <- key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::LEFT)) {
		MoveLeft(_ghostB, pacmanSpeed);
	}
	// Checks if -^ key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::UP)) {
		MoveUp(_ghostB, pacmanSpeed);
	}
	// Checks if -v key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::DOWN)) {
		MoveDown(_ghostB, pacmanSpeed);
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

void Pacman::MoveRight(Player* object, float speed)
{
	object->position->X += speed; //Moves Pacman across X axis
	object->direction = 0;
}

void Pacman::MoveLeft(Player* object, float speed)
{
	object->position->X -= speed; //Moves Pacman across X axis
	object->direction = 2;
}

void Pacman::MoveUp(Player* object, float speed)
{
	object->position->Y -= speed; //Moves Pacman across X axis
	object->direction = 3;
}

void Pacman::MoveDown(Player* object, float speed)
{
	object->position->Y += speed; //Moves Pacman across X axis
	object->direction = 1;
}