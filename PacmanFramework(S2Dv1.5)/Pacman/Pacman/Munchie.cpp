#include "Munchie.h"

Munchie::Munchie(int argc, char* argv[]) : _cMunchieFrameTime(500)
{
	_munchieFrameCount = 0;
	_munchieCurrentFrameTime = 0;
	_isEaten = false;
};

Munchie::~Munchie()
{
	delete _munchieRect;
	delete _munchieBlueTexture;
}

void Munchie::LoadContent()
{
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Textures/Munchie.tga", true);
	_munchieRect = new Rect(0.0f, 0.0f, 12, 12);
	_munchiePosition = new Vector2(300.0f, 450.0f);
}

void Munchie::Update(int elapsedTime)
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
}

Vector2* Munchie::GetPosition()
{
	return _munchiePosition;
}

void Munchie::SetPosition(Vector2* position)
{
	_munchiePosition = position;
}