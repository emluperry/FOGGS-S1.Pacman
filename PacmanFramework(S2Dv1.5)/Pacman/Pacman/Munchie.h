#pragma once
#include "S2D/S2D.h"
using namespace S2D;

class Munchie
{
private:
	// Data to represent Munchie
	Vector2* _munchiePosition;
	bool _isEaten;

public:
	/// <summary> Constructs the Munchie class. </summary>
	Munchie();

	/// <summary> Destroys any data associated with Munchie class. </summary>
	virtual ~Munchie();

	/// <summary> All content should be loaded in this method. </summary>
	void LoadContent();

	void UpdateMunchie(int elapsedTime);


	//Experimental (non tutorial work - delete if causes issues)
	Vector2* GetPosition();

	void SetPosition(Vector2* position);

	bool GetState();
};

