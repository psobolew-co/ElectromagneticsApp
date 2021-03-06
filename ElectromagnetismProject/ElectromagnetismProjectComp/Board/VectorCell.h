#pragma once

#include "Sprite/Sprite.h"
#include "DDSTextureLoader.h"
#include <D3D11.h>
#include "Windows.h"
#include <list>
#include <vector>

using namespace DirectX;

static int MAX_STRENGTH = 15;

class VectorCell {
	public:
		VectorCell(Sprite* arrow, XMFLOAT2 position);
		
		void Update(float timeTotal, float timeDelta);
		void Draw(SpriteBatch* spriteBatch);
		
		void setFieldLine(XMFLOAT2 newFieldLine);
		XMFLOAT2 getPosition();

		// TODO: TEST METHOD ONLY
		XMFLOAT2 getFieldLine();
		
	private:
		Sprite* arrow;
		float opacity;
		float rotation;
		XMFLOAT2 fieldLine;
		XMFLOAT2 position;
		
		void updateOpacityRotation();
};