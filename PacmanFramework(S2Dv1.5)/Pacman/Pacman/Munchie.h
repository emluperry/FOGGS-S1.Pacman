#pragma once
#include "S2D/S2D.h"
using namespace S2D;

class Munchie
{
private:
	// Data to represent Munchie
	int _munchieFrameCount;
	Rect* _munchieRect;
	Texture2D* _munchieBlueTexture;
	Vector2* _munchiePosition;
	const int _cMunchieFrameTime;
	int _munchieCurrentFrameTime;
	bool _isEaten;

public:
	/// <summary> Constructs the Munchie class. </summary>
	Munchie(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Munchie class. </summary>
	virtual ~Munchie();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);


	//Experimental (non tutorial work - delete if causes issues)
	Vector2* GetPosition();

	void SetPosition(Vector2* position);
};

