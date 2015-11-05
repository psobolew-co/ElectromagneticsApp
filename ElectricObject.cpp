#include "pch.h"
#include "ElectricObject.h"


// TODO: Make ElectricObject a child of Sprite

ElectricObject::ElectricObject(ID3D11ShaderResourceView *m_Texture, XMFLOAT2 size, XMFLOAT2 position, Windows::Foundation::Rect* movementBounds, XMFLOAT2 boardSize) : 
Sprite(m_Texture, size, position, movementBounds, 1.0, 0, XMFLOAT2(size.x / 2, size.y / 2))
{
	createBoard(boardSize);
}

void ElectricObject::createBoard(XMFLOAT2 boardSize) {

}

void ElectricObject::isTouched(XMFLOAT2 point) {
	Windows::Foundation::Point specPoint = Windows::Foundation::Point(point.x, point.y);
	if (getBoundingBox()->Contains(specPoint))
		setPosition(point);
}

vector<vector<XMFLOAT2>> ElectricObject::getGrid() {
	vector<vector<XMFLOAT2>> a;
	vector <XMFLOAT2> b;
	b.push_back(XMFLOAT2(0, 0));
	a.push_back(b);
	return a;
}

XMFLOAT2 ElectricObject::calculateField(XMFLOAT2 point) {
	float x = point.x - position.x;
	float y = point.y - position.y;
	float distance = sqrt(pow(y, 2) + pow(x, 2));
	float magnitude = charge / pow(distance, 2);	// approximate electric field equation
	float angle = atan(y / x);	// in radians
	return XMFLOAT2(magnitude*cos(angle), magnitude*sin(angle));
}