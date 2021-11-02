#include "Pacman.h"

#include <iostream>
#include <sstream>

// NOTE TO SELF: Pacman is dressed as a ghost. When creating ghost assets, make the ghosts try to dress as pacman! (Change colour of blue ghost to yellow)

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(150), _cMunchieFrameTime(500)
{
	_munchieFrameCount = 0;
	_paused = false;
	_pKeyDown = false;
	_started = false;
	_pacmanDirection = 0;
	_pacmanCurrentFrameTime = 0;
	_pacmanFrame = 0;
	_munchieCurrentFrameTime = 0;
	_invertAnim = false;
	_hasCollision = true;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacmanTexture;
	delete _pacmanSourceRect;
	delete _munchieBlueTexture;
	delete _munchieRect;
	delete _cherryTexture;
	delete _cherryRect;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacmanTexture = new Texture2D();
	_pacmanTexture->Load("Textures/Pacman.png", false);
	_pacmanPosition = new Vector2(350.0f, 350.0f);
	_pacmanSourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Textures/Munchie.tga", true);
	_munchieRect = new Rect(0.0f, 0.0f, 12, 12);
	//initialise munchies
	for (int i = 0; i < sizeof(munchies) / sizeof(*munchies); i++)
	{
		munchies[i] = new Munchie();
		munchies[i]->SetPosition(_munchiePositions[i]);
	}

	//Load Cherry
	_cherryTexture = new Texture2D;
	_cherryTexture->Load("Textures/Cherry.png", false);
	_cherryRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherryPosition = new Vector2(500.0f, 450.0f);

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

	if (!_started)
	{
		CheckStart(keyboardState, Input::Keys::SPACE);
	}
	else
	{
		CheckPaused(keyboardState, Input::Keys::P);

		if (!_paused) {

			Input(elapsedTime, keyboardState);

			UpdateMunchie(elapsedTime);
			UpdatePacman(elapsedTime);

			CheckViewportCollision();

			CheckDotCollision();
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacmanPosition->X << " Y: " << _pacmanPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing

	SpriteBatch::Draw(_pacmanTexture, _pacmanPosition, _pacmanSourceRect); // Draws Pacman

	for (Munchie* dot : munchies)
	{
		if (!dot->GetState())
		{
			SpriteBatch::Draw(_munchieBlueTexture, dot->GetPosition(), _munchieRect);
		}
	}


	SpriteBatch::Draw(_cherryTexture, _cherryPosition, _cherryRect); //Draws cherry
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	if (_paused) {
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (!_started) {
		std::stringstream menuStream;
		menuStream << "PACMAN!\nPress SPACE to start.";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Yellow);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}


void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState)
{
	// Checks if D key is pressed
	if (keyboardState->IsKeyDown(Input::Keys::D)) {
		_pacmanPosition->X += _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
		_pacmanDirection = 0;
	}
	// Checks if A key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::A)) {
		_pacmanPosition->X -= _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
		_pacmanDirection = 2;
	}
	// Checks if W key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::W)) {
		_pacmanPosition->Y -= _cPacmanSpeed * elapsedTime; //Moves Pacman across Y axis
		_pacmanDirection = 3;
	}
	// Checks if S key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::S)) {
		_pacmanPosition->Y += _cPacmanSpeed * elapsedTime; //Moves Pacman across Y axis
		_pacmanDirection = 1;
	}

	if (keyboardState->IsKeyDown(Input::Keys::TAB))
				_hasCollision = !_hasCollision;
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown) {
		_pKeyDown = true;
		_paused = !_paused;
	}
	if (state->IsKeyUp(pauseKey)) {
		_pKeyDown = false;
	}
}

void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
{
	if (state->IsKeyDown(startKey)) {
		_started = true;
	}
}

void Pacman::CheckViewportCollision()
{
	if (!_hasCollision)
	{
		//prevents movement off right edge
		if (_pacmanPosition->X - _pacmanSourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width
		{
			//teleport to left wall
			_pacmanPosition->X = 0 - _pacmanSourceRect->Width;
		}
		//prevent movement off left edge
		if (_pacmanPosition->X + _pacmanSourceRect->Width < 0)
		{
			//teleport to right wall
			_pacmanPosition->X = Graphics::GetViewportWidth();
		}
		// off bottom edge
		if (_pacmanPosition->Y > Graphics::GetViewportHeight()) //1024 is game width
		{
			//teleport to top wall
			_pacmanPosition->Y = 0 - _pacmanSourceRect->Height;
		}
		// off top edge
		if (_pacmanPosition->Y + _pacmanSourceRect->Height < 0)
		{
			//teleport to bottom wall
			_pacmanPosition->Y = Graphics::GetViewportHeight();
		}
	}
	else if (_hasCollision)
	{
		//prevents movement off right edge
		if (_pacmanPosition->X + _pacmanSourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width
		{
			//block movement
			_pacmanPosition->X = Graphics::GetViewportWidth() - +_pacmanSourceRect->Width;
		}
		//prevent movement off left edge
		if (_pacmanPosition->X < 0)
		{
			//teleport to right wall
			_pacmanPosition->X = 0;
		}
		// off bottom edge
		if (_pacmanPosition->Y + _pacmanSourceRect->Height > Graphics::GetViewportHeight()) //1024 is game width
		{
			//block movement
			_pacmanPosition->Y = Graphics::GetViewportHeight() - _pacmanSourceRect->Height;
		}
		// off top edge
		if (_pacmanPosition->Y < 0)
		{
			//block movement
			_pacmanPosition->Y = 0;
		}
	}
}

void Pacman::CheckDotCollision()
{
	for (Munchie* dot : munchies)
	{
		if (dot->GetPosition()->X < _pacmanPosition->X + _pacmanSourceRect->Width &&
			dot->GetPosition()->X + _munchieRect->Width > _pacmanPosition->X &&
			dot->GetPosition()->Y < _pacmanPosition->Y + _pacmanSourceRect->Height &&
			dot->GetPosition()->Y + _munchieRect->Height > _pacmanPosition->Y)
		{
			dot->SetState(true);
		}
	}
	//if overlaps dot, eat dot

}

void Pacman::UpdatePacman(int elapsedTime)
{
	_pacmanCurrentFrameTime += elapsedTime;
	if (_pacmanCurrentFrameTime > _cPacmanFrameTime) {
		if (!_invertAnim) {
			_pacmanFrame++; //increases animation frame
			if (_pacmanFrame >= 3)
				_invertAnim = !_invertAnim; //four frames starting from 0, swap frame change order after 4th frame
		}
		else {
			_pacmanFrame--;
			if (_pacmanFrame <= 0)
				_invertAnim = !_invertAnim;
		}

		_pacmanCurrentFrameTime -= _cPacmanFrameTime;
	}

	_pacmanSourceRect->X = _pacmanSourceRect->Width * _pacmanFrame;
	_pacmanSourceRect->Y = _pacmanSourceRect->Height * _pacmanDirection; // change source rect based on direction and frame.
}

void Pacman::UpdateMunchie(int elapsedTime)
{
	_munchieCurrentFrameTime += elapsedTime;
	if (_munchieCurrentFrameTime > _cMunchieFrameTime) {
		_munchieFrameCount++;
		if (_munchieFrameCount >= 2) {
			_munchieFrameCount = 0;
		}
		_munchieCurrentFrameTime -= _cMunchieFrameTime;
	}

	_munchieRect->X = _munchieRect->Width * _munchieFrameCount; // change munchie sprite based on time
	_cherryRect->X = _cherryRect->Width * _munchieFrameCount; // change cherry sprite based on munchie sprite/time
}