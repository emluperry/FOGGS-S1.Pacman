#include "Munchie.h"

Munchie::Munchie()
{
	_isEaten = false;
	_munchiePosition = new Vector2(0.0f, 0.0f);
};

Munchie::~Munchie()
{

}

void Munchie::LoadContent()
{
	_munchiePosition = new Vector2();
}

Vector2* Munchie::GetPosition()
{
	return _munchiePosition;
}

void Munchie::SetPosition(Vector2* position)
{
	_munchiePosition = position;
}

bool Munchie::GetState()
{
	return _isEaten;
}