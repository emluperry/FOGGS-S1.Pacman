#include "Pacman.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <fstream>
#include <string>
#include <algorithm>

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
	_pacman->powerUpTime = 99999;
	_pacman->availableLives = 3;

	LoadLevel();

	_munchies = new Enemy * [munchieCount];
	//munchies
	for (int i = 0; i < munchieCount; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameCount = rand() % 1;
		_munchies[i]->pointWorth = 100;
	}
	numMunchies = munchieCount;

	_powMunchies = new Enemy * [powMunchieCount];
	for (int i = 0; i < powMunchieCount; i++)
	{
		_powMunchies[i] = new Enemy();
		_powMunchies[i]->currentFrameTime = 0;
		_powMunchies[i]->frameCount = rand() % 1;
		_powMunchies[i]->pointWorth = 500;
	}
	numPowMunchies = powMunchieCount;

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.1f;
		_ghosts[i]->frame = 0;
		_ghosts[i]->currentFrameTime = 0;
		_ghosts[i]->target = new Vector2(-1, -1);
		_ghosts[i]->pointWorth = 1000;
	}

	_cherry = new Enemy();
	_cherry->currentFrameTime = 0;
	_cherry->frameCount = rand() % 1;
	_cherry->isEaten = false;
	_cherry->pointWorth = 1000;

	//can be toggled by TAB
	_hasCollision = false;
	_powerUpActive = false;
	score = 0;

	_gameState = State::MainMenu;

	_pauseMenu = new Menu();
	_pauseMenu->keyDown = false;
	_startMenu = new Menu();
	_winMenu = new Menu();
	_loseMenu = new Menu();
	_scoreboard = new Menu();
	_scoreboard->keyDown = false;

	_pop = new SoundEffect();
	_music = new SoundEffect();
	_death = new SoundEffect();
	_warp = new SoundEffect();
	_win = new SoundEffect();
	_collision = new SoundEffect();
	_powerUp = new SoundEffect();

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
			if ((*_walls)[x][y] != nullptr)
			{
				delete (*_walls)[x][y]->sourceRect;
				delete (*_walls)[x][y]->position;
			}
		}
	}
	_walls->clear();
	delete _walls;

	delete _munchies[0]->texture;
	for (int i = 0; i < munchieCount; i++)
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

	delete _powMunchies[0]->texture;
	for (int i = 0; i < powMunchieCount; i++)
	{
		if (_powMunchies[i] == NULL)
		{
			continue;
		}
		delete _powMunchies[i]->sourceRect;
		delete _powMunchies[i]->position;
		delete _powMunchies[i];
	}
	delete[] _munchies;

	delete _cherry->texture;
	delete _cherry->sourceRect;
	delete _cherry;

	delete _ghosts[0]->texture;
	delete _ghosts[0]->scaredTex;
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->sourceRect;
		delete _ghosts[i]->position;
		delete _ghosts[i]->target;
		delete _ghosts[i]->scatterTile;
		delete _ghosts[i];
	}

	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman->position;
	delete _pacman;

	delete _stringPosition;

	delete _startMenu->_menuBackground;
	delete _startMenu->_menuRectangle;
	delete _startMenu->_menuStringPosition;

	delete _startMenu;
	delete _pauseMenu;
	delete _winMenu;
	delete _loseMenu;
	delete _scoreboard->_menuStringPosition;
	delete _scoreboard;

	delete _pop;
	delete _death;
	delete _music;
	delete _warp;
	delete _win;
	delete _collision;
	delete _powerUp;

	delete lines;
}

void Pacman::LoadContent()
{
	BuildLevel();
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.png", false);
	_pacman->position = new Vector2(350.0f, 335.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchies - now loaded in LoadLevel
	//Load Cherry
	_cherry->texture = new Texture2D;
	_cherry->texture->Load("Textures/Cherry.png", false);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherry->position = new Vector2(500.0f, 494.0f);

	//load Ghosts
	_ghosts[0]->texture = new Texture2D();
	_ghosts[0]->texture->Load("Textures/GhostGreen.png", false);
	_ghosts[1]->texture = new Texture2D();
	_ghosts[1]->texture->Load("Textures/GhostRed.png", false);
	_ghosts[2]->texture = new Texture2D();
	_ghosts[2]->texture->Load("Textures/GhostPink.png", false);
	_ghosts[3]->texture = new Texture2D();
	_ghosts[3]->texture->Load("Textures/GhostOrange.png", false);
	Texture2D* scaredTex = new Texture2D();
	scaredTex->Load("Textures/GhostScared.png", false);
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->scaredTex = scaredTex;
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}
	_ghosts[0]->position = new Vector2(976.0f, 600.0f);
	_ghosts[0]->scatterTile = new Vector2(976.0f, 600.0f);
	_ghosts[0]->ressurrectionCooldown = 10000;
	_ghosts[0]->isEaten = true;
	_ghosts[1]->position = new Vector2(16.0f, 100.0f);
	_ghosts[1]->scatterTile = new Vector2(16.0f, 100.0f);
	_ghosts[1]->ressurrectionCooldown = 0;
	_ghosts[1]->isEaten = false;
	_ghosts[2]->position = new Vector2(16.0f, 600.0f);
	_ghosts[2]->scatterTile = new Vector2(16.0f, 600.0f);
	_ghosts[2]->ressurrectionCooldown = 5000;
	_ghosts[2]->isEaten = true;
	_ghosts[3]->position = new Vector2(976.0f, 100.0f);
	_ghosts[3]->scatterTile = new Vector2(976.0f, 100.0f);
	_ghosts[3]->ressurrectionCooldown = 15000;
	_ghosts[3]->isEaten = true;

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

	_scoreboard->_menuBackground = _menuBg;
	_scoreboard->_menuRectangle = _menuRect;
	_scoreboard->_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 12.0f, Graphics::GetViewportHeight() / 14.0f);

	_pop->Load("Sounds/pop.wav");
	_death->Load("Sounds/death.wav");
	_powerUp->Load("Sounds/powerup.wav");
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
	lines = new vector<string>();
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

	stream.close();

	munchieCount = 0;
	powMunchieCount = 0;

	for (int y = 0; y < lines->size(); ++y)
	{
		for (int x = 0; x < lines->at(0).size(); ++x)
		{
			if (lines->at(y)[x] == 'o')
			{
				munchieCount++;
			}
			if (lines->at(y)[x] == 'O')
			{
				powMunchieCount++;
			}
		}
	}
}

void Pacman::BuildLevel()
{
	// Allocate the tile grid.
	int width = lines->at(0).size();
	_walls = new vector<vector<Wall*>>(width, vector<Wall*>(lines->size()));

	Texture2D* wallTex = new Texture2D();
	wallTex->Load("Textures/wall.png", false);
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.tga", true);
	Texture2D* powMunchieTex = new Texture2D();
	powMunchieTex->Load("Textures/PowerMunchie.png", false);

	int index = 0;
	int powindex = 0;
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
				wall->position = new Vector2(-112 + x * 32, -16 + y * 32);
				wall->texture = wallTex;
				wall->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
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
				_munchies[index]->position = new Vector2(-104 + x * 32, -8 + y * 32);
				index++;
			}
			else if (tileType == 'O')
			{
				_powMunchies[powindex]->texture = powMunchieTex;
				_powMunchies[powindex]->sourceRect = new Rect(0.0f, 0.0f, 16, 16);
				_powMunchies[powindex]->position = new Vector2(-104 + x * 32, -8 + y * 32);
				powindex++;
			}
		}
	}
}

void Pacman::RestartLevel()
{
	_powerUpActive = false;
	//reset pacman pos, boosts, lives
	_pacman->dead = false;
	_pacman->direction = 0;
	_pacman->availableBoosts = 3;
	_pacman->availableLives = 3;
	_pacman->boostTime = 3000;
	_pacman->position = new Vector2(350.0f, 335.0f);

	//reset munchies
	//delete values in old array
	for (int i = 0; i < munchieCount; i++)
	{
		if (_munchies[i] == NULL)
		{
			continue;
		}
		delete _munchies[i]->sourceRect;
		delete _munchies[i]->position;
		delete _munchies[i];
	}
	for (int i = 0; i < powMunchieCount; i++)
	{
		if (_powMunchies[i] == NULL)
		{
			continue;
		}
		delete _powMunchies[i]->sourceRect;
		delete _powMunchies[i]->position;
		delete _powMunchies[i];
	}
	//create new array values
	LoadLevel();
	for (int i = 0; i < munchieCount; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameCount = rand() % 1;
		_munchies[i]->pointWorth = 100;
	}
	numMunchies = munchieCount;
	for (int i = 0; i < powMunchieCount; i++)
	{
		_powMunchies[i] = new Enemy();
		_powMunchies[i]->currentFrameTime = 0;
		_powMunchies[i]->frameCount = rand() % 1;
		_powMunchies[i]->pointWorth = 500;
	}
	numPowMunchies = powMunchieCount;
	//setup new values for generation
	BuildLevel();
	//reset score
	score = 0;
	invasionCooldown = 0;

	//reset ghosts
	_ghosts[0]->position = new Vector2(976.0f, 600.0f);
	_ghosts[1]->position = new Vector2(16.0f, 100.0f);
	_ghosts[2]->position = new Vector2(16.0f, 600.0f);
	_ghosts[3]->position = new Vector2(976.0f, 100.0f);

	_ghosts[0]->ressurrectionCooldown = 10000;
	_ghosts[0]->isEaten = true;
	_ghosts[1]->ressurrectionCooldown = 0;
	_ghosts[1]->isEaten = false;
	_ghosts[2]->ressurrectionCooldown = 5000;
	_ghosts[2]->isEaten = true;
	_ghosts[3]->ressurrectionCooldown = 15000;
	_ghosts[3]->isEaten = true;
}

void Pacman::ResetPositions()
{
	_powerUpActive = false;
	//reset pacman pos, boosts, lives
	_pacman->dead = false;
	_pacman->direction = 0;
	_pacman->boostTime = 3000;
	_pacman->position = new Vector2(350.0f, 335.0f);

	_ghosts[0]->position = new Vector2(976.0f, 600.0f);
	_ghosts[1]->position = new Vector2(16.0f, 100.0f);
	_ghosts[2]->position = new Vector2(16.0f, 600.0f);
	_ghosts[3]->position = new Vector2(976.0f, 100.0f);

	_ghosts[0]->ressurrectionCooldown = 10000;
	_ghosts[0]->isEaten = true;
	_ghosts[1]->ressurrectionCooldown = 0;
	_ghosts[1]->isEaten = false;
	_ghosts[2]->ressurrectionCooldown = 5000;
	_ghosts[2]->isEaten = true;
	_ghosts[3]->ressurrectionCooldown = 15000;
	_ghosts[3]->isEaten = true;

	invasionCooldown = 0;
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	//Gets current state of mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();

	if (_gameState == MainMenu)
	{
		CheckStart(keyboardState, Input::Keys::SPACE);
	}
	else if (_gameState == Win || _gameState == Lose)
	{
		CheckHighScore(keyboardState, Input::Keys::RETURN);
	}
	else if (_gameState == HighScore)
	{
		if ((scores.size() < 10 || scores[scores.size() - 1].score < score) && _scoreboard->keyDown != true)
		{
			_scoreboard->keyDown = true;
			InputName(scores);
		}
		CheckRestart(keyboardState, Input::Keys::R);
	}
	else
	{
		_pacman->boostTime += elapsedTime;
		invasionCooldown += elapsedTime;
		_pacman->powerUpTime += elapsedTime;
		if (_pacman->powerUpTime >= 10000)
		{
			_powerUpActive = false;
		}
		for (int i = 0; i < GHOSTCOUNT; i++)
		{
			if (_ghosts[i]->isEaten && _ghosts[i]->ressurrectionCooldown > 0)
			{
				_ghosts[i]->ressurrectionCooldown -= elapsedTime;
			}
			if (_ghosts[i]->ressurrectionCooldown <= 0)
			{
				_ghosts[i]->isEaten = false;
			}
		}

		CheckPaused(keyboardState, Input::Keys::P);

		if (_gameState != Pause && _gameState != Lose && _gameState != Win) {

			Input(elapsedTime, keyboardState, mouseState);

			for (int i = 0; i < munchieCount; i++)
			{
				if (_munchies[i] == NULL)
				{
					continue;
				}
				UpdateMunchie(elapsedTime, i);
			}
			for (int i = 0; i < powMunchieCount; i++)
			{
				if (_powMunchies[i] == NULL)
				{
					continue;
				}
				UpdatePowMunchie(elapsedTime, i);
			}

			UpdatePacman(elapsedTime);
			
			if (invasionCooldown >= 60000 || _powerUpActive)
			{
				for (int i = 0; i < GHOSTCOUNT; i++)
				{
					RetreatGhost(_ghosts[i]);
				}
				if (invasionCooldown >= 90000)
				{
					invasionCooldown = 0;
				}
			}
			else
			{
				UpdateGreen(_ghosts[0]);
				UpdateRed(_ghosts[1]);
				UpdatePink(_ghosts[2]);
				UpdateOrange(_ghosts[3]);
			}
			for (int i = 0; i < GHOSTCOUNT; i++)
			{
				if (!_ghosts[i]->isEaten)
				{
					UpdateGhost(_ghosts[i], elapsedTime);
				}
			}

			CheckCollisions(elapsedTime);
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;

	SpriteBatch::BeginDraw(); // Starts Drawing
	int width;
	int height;
	switch (_gameState)
	{
	case MainMenu:
		stream << "PACMAN!\nPress SPACE to start.";

		SpriteBatch::Draw(_startMenu->_menuBackground, _startMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(stream.str().c_str(), _startMenu->_menuStringPosition, Color::Yellow);
		break;
	case Playing:
		stream << "Pacman X: " << (int)_pacman->position->X << " Y: " << (int)_pacman->position->Y << " Boosts: " << _pacman->availableBoosts << " Score: " << score << " Lives: " << _pacman->availableLives;

		width = _walls->size();
		height = _walls->at(0).size();
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

		for (int i = 0; i < munchieCount; i++)
		{
			if (_munchies[i] == NULL)
			{
				continue;
			}
			SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect); // Draws munchie
		}
		for (int i = 0; i < powMunchieCount; i++)
		{
			if (_powMunchies[i] == NULL)
			{
				continue;
			}
			SpriteBatch::Draw(_powMunchies[i]->texture, _powMunchies[i]->position, _powMunchies[i]->sourceRect); // Draws munchie
		}

		for (int i = 0; i < GHOSTCOUNT; i++)
		{
			if (!_ghosts[i]->isEaten)
			{
				if (_powerUpActive)
				{
					SpriteBatch::Draw(_ghosts[i]->scaredTex, _ghosts[i]->position, _ghosts[i]->sourceRect); // Draws ghosts
				}
				else
				{
					SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect); // Draws ghosts
				}
			}
		}

		if (!_cherry->isEaten)
		{
			SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect); //Draws cherry
		}

		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
		break;
	case Pause:
		stream << "PAUSED!\nPress P to unpause.";

		SpriteBatch::Draw(_pauseMenu->_menuBackground, _pauseMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(stream.str().c_str(), _pauseMenu->_menuStringPosition, Color::Red);
		break;
	case Win:
		stream << "YOU WIN!\nFinal score: " << score << "\nPress RETURN to continue.";

		SpriteBatch::Draw(_winMenu->_menuBackground, _winMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(stream.str().c_str(), _winMenu->_menuStringPosition, Color::Yellow);
		break;
	case Lose:
		stream << "YOU LOSE...\nFinal score: " << score << "\nPress RETURN to continue.";

		SpriteBatch::Draw(_winMenu->_menuBackground, _winMenu->_menuRectangle, nullptr);
		SpriteBatch::DrawString(stream.str().c_str(), _winMenu->_menuStringPosition, Color::Yellow);
		break;
	case HighScore:
		stream = DisplayScores(scores);

		SpriteBatch::Draw(_scoreboard->_menuBackground, _scoreboard->_menuRectangle, nullptr);
		SpriteBatch::DrawString(stream.str().c_str(), _scoreboard->_menuStringPosition, Color::White);
		break;
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState, Input::MouseState* mouseState)
{
	float pacmanSpeed = _pacman->cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	//mouse input - SPEED BOOST OPTIONAL IMPLEMENTATION: LIMITED TIME/USE
	if (mouseState->RightButton == Input::ButtonState::PRESSED && _pacman->boostTime >= 3000 && _pacman->availableBoosts > 0)
	{
		_pacman->speedMultiplier = 2.0f;
		_pacman->availableBoosts--;
		_pacman->boostTime = 0;
	}

	if (_pacman->boostTime >= 3000 && !_powerUpActive)
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

	//Toggles wall collision and wall wrapping.
	if (keyboardState->IsKeyDown(Input::Keys::TAB))
				_hasCollision = !_hasCollision;
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pauseMenu->keyDown) {
		_pauseMenu->keyDown = true;

		if (_gameState == Pause)
		{
			_gameState = Playing;
			Audio::Resume(_music);
		}
		else
		{
			_gameState = Pause;
			Audio::Pause(_music);
		}
	}
	if (state->IsKeyUp(pauseKey)) {
		_pauseMenu->keyDown = false;
	}
}

void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
{
	if (state->IsKeyDown(startKey)) {
		_gameState = Playing;
	}
}

void Pacman::CheckWin()
{
	if (numMunchies == 0 && numPowMunchies == 0)
	{
		_gameState = Win;
		Audio::Stop(_music);
		Audio::Play(_win);
	}
}

void Pacman::CheckHighScore(Input::KeyboardState* state, Input::Keys restartKey)
{
	if (state->IsKeyDown(restartKey))
	{
		LoadScores(scores);
		_gameState = HighScore;
	}
}

void Pacman::CheckRestart(Input::KeyboardState* state, Input::Keys restartKey)
{
	if (state->IsKeyDown(restartKey)) {
		_scoreboard->keyDown = false;
		RestartLevel();
		_gameState = MainMenu;
		Audio::Play(_music);
	}
}

//scoreboard methods
void Pacman::LoadScores(vector<ScoreEntry>& scores)
{
	scores.clear();
	ifstream inFile;
	inFile.open("scores.txt");

	if (inFile)
	{
		int entryNum = 1;
		ScoreEntry* entry;

		while (!inFile.eof())
		{
			entry = new ScoreEntry();
			getline(inFile, entry->name);
			string temp;
			getline(inFile, temp);
			try
			{
				entry->score = stoi(temp);
			}
			catch (string score)
			{
				cout << "File is incorrectly formatted." << endl;
				exit(1);
			}
			entry->order = entryNum;
			scores.push_back(*entry);
			entryNum++;
			delete entry;
		}
	}

	inFile.close();
}

void Pacman::SaveScores(vector<ScoreEntry>& scores)
{
	ofstream outFile;
	outFile.open("scores.txt", ios::trunc);
	for (int i = 0; i < scores.size(); i++)
	{
		outFile << scores[i].name << endl << scores[i].score;
		if (i < scores.size() - 1)
		{
			outFile << endl;
		}
	}
	outFile.close();
}

void Pacman::InputName(vector<ScoreEntry>& scores)
{
	ScoreEntry newEntry;
	newEntry.score = score;

	cout << "A valid score!" << endl << "Please enter your name." << endl;
	bool valid = false;
	while (!valid)
	{
		getline(cin, newEntry.name);
		if (newEntry.name.length() > 0)
		{
			valid = true;
		}
		else
		{
			cout << endl << "Invalid input! Try again." << endl;
		}
		cin.clear();
		cin.ignore();
	}
	newEntry.order = 0;

	SortScores(scores, newEntry);
	SaveScores(scores);
	return;
}

void Pacman::SortScores(vector<ScoreEntry>& scores, ScoreEntry& newEntry)
{
	int size = scores.size();
	bool moveUp = false;
	ScoreEntry temp;
	if (size == 0)
	{
		newEntry.order = 1;
		scores.push_back(newEntry);
	}
	else
	{
		for (int i = 0; i < size; i++)
		{
			if (!moveUp && scores[i].score < newEntry.score)
			{
				newEntry.order = i + 1;
				temp = scores[i];
				scores[i] = newEntry;
				moveUp = true;
				continue;
			}
			if (moveUp)
			{
				temp.order = i + 1;
				ScoreEntry temp2 = scores[i];
				scores[i] = temp;
				temp = temp2;
			}
		}
		if (size < 10)
		{
			temp.order++;
			scores.push_back(temp);
		}
	}
}

stringstream Pacman::DisplayScores(vector<ScoreEntry>& scores) //2.
{
	std::stringstream stream;
	stream << "SCOREBOARD" << endl << endl;
	if (scores.size() == 0)
	{
		stream << "There are no scores to display. Press R to return to the menu." << endl;
		return stream;
	}

	int size = scores.size();
	for (int i = 0; i < size; i++)
	{
		stream << scores[i].order << setw(10) << scores[i].name << setw(10) << scores[i].score << endl;
	}

	stream << endl << "Press R to return to the menu." << endl;
	return stream;
}


//game methods
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

void Pacman::UpdatePowMunchie(int elapsedTime, int index)
{
	Enemy* powMunchie = _powMunchies[index];
	powMunchie->currentFrameTime += elapsedTime;
	if (powMunchie->currentFrameTime > powMunchie->cFrameTime)
	{
		powMunchie->frameCount++;
		if (powMunchie->frameCount >= 2)
		{
			powMunchie->frameCount = 0;
		}
		powMunchie->currentFrameTime -= powMunchie->cFrameTime;
	}

	powMunchie->sourceRect->X = powMunchie->sourceRect->Width * powMunchie->frameCount;
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
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

void Pacman::UpdateGreen(MovingEnemy* ghost)
{
	//THIS IS INKY - Double vector line between red/blinky's position and two tiles ahead of pacman
	Vector2 pacPos;
	if (_pacman->direction == 0)
	{
		pacPos = Vector2(_pacman->position->X + 24, _pacman->position->Y);
	}
	else if (_pacman->direction == 1)
	{
		pacPos = Vector2(_pacman->position->X, _pacman->position->Y + 24);
	}
	else if (_pacman->direction == 2)
	{
		pacPos = Vector2(_pacman->position->X - 24, _pacman->position->Y);
	}
	else if (_pacman->direction == 3)
	{
		pacPos = Vector2(_pacman->position->X, _pacman->position->Y - 24);
	}

	//delete ghost->target;
	ghost->target = new Vector2((pacPos.X - _ghosts[1]->position->X)*2, (pacPos.Y - _ghosts[1]->position->Y)*2);
	PathfindTarget(ghost);
}

void Pacman::UpdateRed(MovingEnemy* ghost)
{
	//delete ghost->target;
	ghost->target = _pacman->position;
	PathfindTarget(ghost);
}

void Pacman::UpdatePink(MovingEnemy* ghost)
{
	//delete ghost->target;
	if (_pacman->direction == 0)
	{
		ghost->target = new Vector2(_pacman->position->X + 48, _pacman->position->Y);
	}
	else if (_pacman->direction == 1)
	{
		ghost->target = new Vector2(_pacman->position->X, _pacman->position->Y +48);
	}
	else if (_pacman->direction == 2)
	{
		ghost->target = new Vector2(_pacman->position->X - 48, _pacman->position->Y);
	}
	else if (_pacman->direction == 3)
	{
		ghost->target = new Vector2(_pacman->position->X, _pacman->position->Y - 48);
	}
	PathfindTarget(ghost);
}

void Pacman::UpdateOrange(MovingEnemy* ghost)
{
	//THIS IS CLYDE - try to get him to move to random points
	//OR: when 8 tiles away, target pacman. when closer, scatter to retreat tile
	int xDist = _pacman->position->X - ghost->position->X;
	xDist *= xDist;
	int yDist = _pacman->position->Y - ghost->position->Y;
	yDist *= yDist;
	int squareDist = xDist + yDist;
	if (squareDist >= 9216)
	{
		ghost->target = _pacman->position;
	}
	else
	{
		ghost->target = new Vector2(ghost->scatterTile->X, ghost->scatterTile->Y);
	}

	PathfindTarget(ghost);
}

void Pacman::RetreatGhost(MovingEnemy* ghost)
{
	ghost->target = new Vector2(ghost->scatterTile->X, ghost->scatterTile->Y);
	PathfindTarget(ghost);
}

void Pacman::CheckCollisions(int elapsedTime)
{
	CheckGhostCollisions();
	CheckViewportCollision();
	for (int i = 0; i < munchieCount; i++)
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
	for (int i = 0; i < powMunchieCount; i++)
	{
		if (_powMunchies[i] == NULL)
		{
			continue;
		}
		if (CheckObjectCollision(_powMunchies[i]))
		{
			Audio::Play(_powerUp);
			score += _powMunchies[i]->pointWorth;
			delete _powMunchies[i]->sourceRect;
			delete _powMunchies[i]->position;
			delete _powMunchies[i];
			_powMunchies[i] = NULL;
			numPowMunchies--;
			CheckWin();

			_powerUpActive = true;
			_pacman->speedMultiplier = 2.0f;
			_pacman->powerUpTime = 0;
		}
	}
	if (_cherry->isEaten == false && CheckObjectCollision(_cherry))
	{
		score += _cherry->pointWorth;
		_cherry->isEaten = true;
	}
	CheckPacWallCollision(elapsedTime);
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

	//for ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		if (!_hasCollision)
		{
			//prevents movement off right edge
			if (_ghosts[i]->position->X - _ghosts[i]->sourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width
			{
				//teleport to left wall
				_ghosts[i]->position->X = 0 - _ghosts[i]->sourceRect->Width;
			}
			//prevent movement off left edge
			if (_ghosts[i]->position->X + _ghosts[i]->sourceRect->Width < 0)
			{
				//teleport to right wall
				_ghosts[i]->position->X = Graphics::GetViewportWidth();
			}
			// off bottom edge
			if (_ghosts[i]->position->Y > Graphics::GetViewportHeight()) //1024 is game width
			{
				//teleport to top wall
				_ghosts[i]->position->Y = 0 - _ghosts[i]->sourceRect->Height;
			}
			// off top edge
			if (_ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height < 0)
			{
				//teleport to bottom wall
				_ghosts[i]->position->Y = Graphics::GetViewportHeight();
			}
		}
	}
}

void Pacman::CheckGhostCollisions()
{
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		if (_ghosts[i]->isEaten)
		{
			continue;
		}
		if (_ghosts[i]->position->X + 8 < _pacman->position->X + _pacman->sourceRect->Width &&
			_ghosts[i]->position->X + _ghosts[i]->sourceRect->Width - 8 > _pacman->position->X &&
			_ghosts[i]->position->Y + 8 < _pacman->position->Y + _pacman->sourceRect->Height &&
			_ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height - 8 > _pacman->position->Y)
		{
			if (_powerUpActive)
			{
				_ghosts[i]->isEaten = true;
				_ghosts[i]->ressurrectionCooldown = 30000;
				_ghosts[i]->position = new Vector2(_ghosts[i]->scatterTile->X, _ghosts[i]->scatterTile->Y);
				score += _ghosts[i]->pointWorth;
			}
			else
			{
				_pacman->availableLives -= 1;
				i = GHOSTCOUNT;
				Audio::Play(_death);
				if (_pacman->availableLives <= 0)
				{
					_pacman->dead = true;
					_gameState = Lose;
					Audio::Stop(_music);
				}
				else
				{
					ResetPositions();
				}
			}
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

void Pacman::CheckPacWallCollision(int elapsedTime)
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
			if ((*_walls)[x][y]->position->X + 4 < _pacman->position->X + _pacman->sourceRect->Width &&
				(*_walls)[x][y]->position->X + (*_walls)[x][y]->sourceRect->Width - 4 > _pacman->position->X &&
				(*_walls)[x][y]->position->Y + 4 < _pacman->position->Y + _pacman->sourceRect->Height &&
				(*_walls)[x][y]->position->Y + (*_walls)[x][y]->sourceRect->Height - 4 > _pacman->position->Y)
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

//PATHFINDING

void Pacman::PathfindTarget(MovingEnemy* ghost)
{ //avoids walls
	vector<int> options = GetOptions(ghost);
	if (options.size() >= 1)
	{
		int index = GetBestMove(ghost, options);
		ghost->direction = options[index];
	}
}

vector<int> Pacman::GetOptions(MovingEnemy* ghost)
{
	int oppDirection = 0;
	if (ghost->direction == 0)
	{
		oppDirection = 2;
	}
	else if (ghost->direction == 1)
	{
		oppDirection = 3;
	}
	else if (ghost->direction == 2)
	{
		oppDirection = 0;
	}
	else if (ghost->direction == 3)
	{
		oppDirection = 1;
	}

	vector<int> options;
	if (CheckPosition(*ghost->position, 0) && oppDirection != 0)
	{
		options.push_back(0);
	}
	if (CheckPosition(*ghost->position, 1) && oppDirection != 1)
	{
		options.push_back(1);
	}
	if (CheckPosition(*ghost->position, 2) && oppDirection != 2)
	{
		options.push_back(2);
	}
	if (CheckPosition(*ghost->position, 3) && oppDirection != 3)
	{
		options.push_back(3);
	}

	if (options.size() == 0)
	{
		options.push_back(oppDirection);
	}
	return options;
}

bool Pacman::CheckPosition(Vector2 initialPos, int direction)
{
	int xOffset = 0;
	int yOffset = 0;
	if (direction == 0) //right
	{
		xOffset += 8;
	}
	else if (direction == 2) //left
	{
		xOffset -= 8;
	}
	else if (direction == 1) //down
	{
		yOffset += 8;
	}
	else if (direction == 3) //up
	{
		yOffset -= 8;
	}
	Vector2* position = new Vector2(initialPos.X + xOffset, initialPos.Y + yOffset);

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
			if ((*_walls)[x][y]->position->X + 4 < position->X + _ghosts[0]->sourceRect->Width &&
				(*_walls)[x][y]->position->X + (*_walls)[x][y]->sourceRect->Width - 4 > position->X &&
				(*_walls)[x][y]->position->Y + 4 < position->Y + _ghosts[0]->sourceRect->Height &&
				(*_walls)[x][y]->position->Y + (*_walls)[x][y]->sourceRect->Height - 4 > position->Y)
			{
				delete position;
				return false;
			}
		}
	}
	delete position;
	return true;
}

int Pacman::GetBestMove(MovingEnemy* ghost, vector<int> &options)
{
	int lowestDiff = 9999999;
	int lowestChoice = 0;
	for (int i = 0; i < options.size(); i++)
	{
		int xOffset = 0;
		int yOffset = 0;
		if (options[i] == 0) //right
		{
			xOffset += 8;
		}
		else if (options[i] == 2) //left
		{
			xOffset -= 8;
		}
		else if (options[i] == 1) //down
		{
			yOffset += 8;
		}
		else if (options[i] == 3) //up
		{
			yOffset -= 8;
		}
		Vector2* position = new Vector2(ghost->position->X + xOffset, ghost->position->Y + yOffset);

		int xDiff = ghost->target->X - position->X;
		xDiff *= xDiff;
		int yDiff = ghost->target->Y - position->Y;
		yDiff *= yDiff;
		int diff = xDiff + yDiff;

		if (xDiff + yDiff < lowestDiff)
		{
			lowestDiff = xDiff + yDiff;
			lowestChoice = i;
		}
		delete position;
	}
	return lowestChoice;
}