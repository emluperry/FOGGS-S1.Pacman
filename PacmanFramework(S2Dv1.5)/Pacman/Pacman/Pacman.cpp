#include "Pacman.h"

#include <iostream>
#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250)
{
	_frameCount = 0;
	_paused = false;
	_pKeyDown = false;
	_spacePressed = false;
	_pacmanDirection = 0;
	_pacmanCurrentFrameTime = 0;
	_pacmanFrame = 0;

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
	delete _munchieInvertedTexture;
	delete _munchieRect;
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
	_munchieInvertedTexture = new Texture2D();
	_munchieInvertedTexture->Load("Textures/MunchieInverted.tga", true);
	_munchieRect = new Rect(0.0f, 0.0f, 12, 12);
	_munchiePosition = new Vector2(100.0f, 450.0f);

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

	if (keyboardState->IsKeyDown(Input::Keys::SPACE)) {
		_spacePressed = true;
	}

	if (_spacePressed) {
		if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown) {
			_pKeyDown = true;
			_paused = !_paused;
		}
		if (keyboardState->IsKeyUp(Input::Keys::P)) {
			_pKeyDown = false;
		}

		if (!_paused) {
			_frameCount++;
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

			_pacmanCurrentFrameTime += elapsedTime;
			if (_pacmanCurrentFrameTime > _cPacmanFrameTime) {
				_pacmanFrame++; //increases animation frame
				if (_pacmanFrame >= 4)
					_pacmanFrame = 0; //four frames starting from 0, reset to 0 if the frame equals 4.

				_pacmanCurrentFrameTime -= _cPacmanFrameTime;
			}

			_pacmanSourceRect->X = _pacmanSourceRect->Width * _pacmanFrame;
			_pacmanSourceRect->Y = _pacmanSourceRect->Height * _pacmanDirection; // change source rect based on direction and frame.

			if (keyboardState->IsKeyDown(Input::Keys::TAB))
				hasCollision = !hasCollision;

			if (!hasCollision)
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
			else if (hasCollision)
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

			//if overlaps dot, eat dot
			if (_munchiePosition->X < _pacmanPosition->X + _pacmanSourceRect->Width &&
				_munchiePosition->X + _munchieRect->X > _pacmanPosition->X &&
				_munchiePosition->Y < _pacmanPosition->Y + _pacmanSourceRect->Height &&
				_munchiePosition->Y + _munchieRect->Y > _pacmanSourceRect->Y)
			{
				isEaten = true;
			}
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

	if (!isEaten)
	{
		if (_frameCount < 30)
		{
			// Draws Red Munchie
			SpriteBatch::Draw(_munchieInvertedTexture, _munchiePosition, _munchieRect);
		}
		else
		{
			// Draw Blue Munchie
			SpriteBatch::Draw(_munchieBlueTexture, _munchiePosition, _munchieRect);
		}
	}

	if (_frameCount >= 60)
		_frameCount = 0;
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);

	if (_paused) {
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (!_spacePressed) {
		std::stringstream menuStream;
		menuStream << "PACMAN!\nPress SPACE to start.";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Yellow);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}