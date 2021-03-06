#pragma once
#include "Sprite/Sprite.h"

class Puck : public Sprite {

public:
	Puck(ID3D11ShaderResourceView *m_Texture, XMFLOAT2 size, XMFLOAT2 position, Windows::Foundation::Rect* movementBounds);

	void Update(float timeTotal, float timeDelta, XMFLOAT2 currentField);
	void calculateVelocity(XMFLOAT2 currentField);	// Adds the field the puck is currently in to its velocity

private:
};