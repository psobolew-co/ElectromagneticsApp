#include "pch.h"
#include "ChargedObjects/Puck.h"

Puck::Puck(ID3D11ShaderResourceView *m_Texture, XMFLOAT2 size, XMFLOAT2 position, Windows::Foundation::Rect* movementBounds) : 
Sprite(m_Texture, size, position, movementBounds, .1, 0, XMFLOAT2(size.x / 2, size.y / 2))
{
	Velocity = XMFLOAT2(0, 0);
	Speed = 1;
}

void Puck::Update(float timeTotal, float timeDelta, XMFLOAT2 currentField) {
	calculateVelocity(currentField);
	Sprite::Update(timeTotal, timeDelta);	
}

void Puck::calculateVelocity(XMFLOAT2 currentField) {
	Velocity.x += currentField.x;
	Velocity.y -= currentField.y;	// Negative because fields are measured in what we think is intuitive, not what the screen is measured in
}