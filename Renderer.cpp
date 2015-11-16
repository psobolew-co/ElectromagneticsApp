﻿#include "pch.h"
#include "Renderer.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;

Renderer::Renderer() {
	previousPoint = XMFLOAT2(0, 0);
	appState = AppState::MainMenu;
}

void Renderer::CreateDeviceResources()
{
	Direct3DBase::CreateDeviceResources();
}

void Renderer::CreateWindowSizeDependentResources()
{
	Direct3DBase::CreateWindowSizeDependentResources();

	scale = DisplayProperties::LogicalDpi / 96.0f;
	m_windowBounds.Height *= scale;
	m_windowBounds.Width *= scale;

	float localScale;

	m_spriteBatch = unique_ptr<SpriteBatch>(new DirectX::SpriteBatch(m_d3dContext.Get()));

	// Create the arrow
	arrowTexture = nullptr;
	localScale = .1;
	CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Assets/arrow.dds", nullptr, &arrowTexture, MAXSIZE_T);
	arrow = new Sprite(arrowTexture, XMFLOAT2(600, 457), XMFLOAT2(0, 0), &m_windowBounds, localScale);

	// Create the vector board
	XMFLOAT2 boardSize = XMFLOAT2(int(m_windowBounds.Width / 60), int(m_windowBounds.Height / 60));
	vectorBoard = new VectorBoard(arrowTexture, boardSize, &m_windowBounds);

	// Create the positive charge
	posChargeTexture = nullptr;
	CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Assets/charge.dds", nullptr, &posChargeTexture, MAXSIZE_T);
	posCharge = new ElectricObject(posChargeTexture, XMFLOAT2(500, 500), XMFLOAT2(100, 100), &m_windowBounds, boardSize, 1);
	chargeBoxTexture = nullptr;
	CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Assets/charge_box.dds", nullptr, &chargeBoxTexture, MAXSIZE_T);
	chargeBox = new Sprite(chargeBoxTexture, XMFLOAT2(500, 500), XMFLOAT2(m_windowBounds.Width - 100, m_windowBounds.Height - 100), &m_windowBounds, .2);
	textures[chargeBox] = posChargeTexture;

	// Create the negative charge
	CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Assets/neg_charge.dds", nullptr, &negChargeTexture, MAXSIZE_T);
	negCharge = new ElectricObject(negChargeTexture, XMFLOAT2(500, 500), XMFLOAT2(100, 100), &m_windowBounds, boardSize, -1);
	CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Assets/neg_charge_box.dds", nullptr, &negChargeBoxTexture, MAXSIZE_T);
	negChargeBox = new Sprite(negChargeBoxTexture, XMFLOAT2(500, 500), XMFLOAT2(m_windowBounds.Width - 200, m_windowBounds.Height - 100), &m_windowBounds, .2);
	textures[negChargeBox] = negChargeTexture;

	// Create the charge box and the electric object manager
	objectManager = new ElectricObjectManager(textures, XMFLOAT2(500, 500), &m_windowBounds, boardSize);

	// Create the puck
	CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Assets/puck.dds", nullptr, &puckTexture, MAXSIZE_T);
	puck = new Puck(puckTexture, XMFLOAT2(500, 500), XMFLOAT2(m_windowBounds.Width / 2, m_windowBounds.Height / 2), &m_windowBounds);
}

void Renderer::Update(float timeTotal, float timeDelta)
{
	switch (appState) {
	case AppState::MainMenu:
		break;
	case AppState::InGameSetup:
		objectManager->Update(timeTotal, timeDelta);
		vectorBoard->Update(timeTotal, timeDelta, objectManager->getElectricObjects());
		break;
	case AppState::InGameRunning:
		vectorBoard->Update(timeTotal, timeDelta, objectManager->getElectricObjects());
		puck->Update(timeTotal, timeDelta, vectorBoard->getClosestField(puck->getPosition()));
		objectManager->Update(timeTotal, timeDelta);
		break;
	default:
		break;
	}

}

void Renderer::Render()
{
	// Background color
	const float bg_color[] = { 245.0f / 255.0f, 241.0 / 255.0f, 196.0f / 255.0f, 1.000f };
	m_d3dContext->ClearRenderTargetView(
		m_renderTargetView.Get(),
		bg_color
		);

	// Necessary for proper rendering
	m_d3dContext->ClearDepthStencilView(
		m_depthStencilView.Get(),
		D3D11_CLEAR_DEPTH,
		1.0f,
		0
		);

	m_d3dContext->OMSetRenderTargets(
		1,
		m_renderTargetView.GetAddressOf(),
		m_depthStencilView.Get()
		);

	// Where my sprites will be drawn
	m_spriteBatch->Begin();
	switch (appState) {
	case AppState::MainMenu:
		break;
	case AppState::InGameSetup:
		vectorBoard->Draw(m_spriteBatch.get());
		puck->Draw(m_spriteBatch.get());
		objectManager->Draw(m_spriteBatch.get());
		break;
	case AppState::InGameRunning:
		vectorBoard->Draw(m_spriteBatch.get());
		puck->Draw(m_spriteBatch.get());
		objectManager->Draw(m_spriteBatch.get());
		break;
	default:
		break;
	}
	
	m_spriteBatch->End();
}

void Renderer::HandlePressInput(Windows::UI::Input::PointerPoint^ currentPoint)
{
	XMFLOAT2 vectorPoint = XMFLOAT2(currentPoint->RawPosition.X * scale, currentPoint->RawPosition.Y * scale);

	switch (appState) {
	case AppState::InGameSetup:
		for (ElectricObject* thing : objectManager->getElectricObjects()) {
			if (onSprite(thing, vectorPoint)) {
				thing->isTouched(vectorPoint);
				thing->isMoving = true;
				break;	// break because we only want one charge to be moving
			}
		}

		// Check for if a object should be created.
		objectManager->checkForCreateObject(vectorPoint);
		break;
	case AppState::InGameRunning:
		for (ElectricObject* thing : objectManager->getElectricObjects()) {
			if (onSprite(thing, vectorPoint)) {
				thing->isTouched(vectorPoint);
				thing->isMoving = true;
				break;	// break because we only want one charge to be moving
			}
		}

		// Check for if a object should be created.
		objectManager->checkForCreateObject(vectorPoint);
		break;
	default:
		break;
	}


}

void Renderer::HandleReleaseInput(Windows::UI::Input::PointerPoint^ currentPoint)
{
	XMFLOAT2 vectorPoint = XMFLOAT2(currentPoint->RawPosition.X * scale, currentPoint->RawPosition.Y * scale);

	switch (appState) {
	case AppState::MainMenu:
		appState = AppState::InGameSetup;
	case AppState::InGameSetup:	// running and setup do same stuff
		previousPoint = XMFLOAT2(0, 0);
		for (ElectricObject* thing : objectManager->getElectricObjects()) {
			if (thing->isMoving) {
				thing->isMoving = false;
			}
		}

		// If you've dragged an object into the box
		objectManager->checkForDeleteObject();
		break;
	case AppState::InGameRunning:
		previousPoint = XMFLOAT2(0, 0);
		for (ElectricObject* thing : objectManager->getElectricObjects()) {
			if (thing->isMoving) {
				thing->isMoving = false;
			}
		}

		// If you've dragged an object into the box
		objectManager->checkForDeleteObject();
		break;
	default:
		break;
	}


}

void Renderer::HandleMoveInput(Windows::UI::Input::PointerPoint^ currentPoint)
{
	XMFLOAT2 vectorPoint = XMFLOAT2(currentPoint->RawPosition.X * scale, currentPoint->RawPosition.Y * scale);
	bool noneIsMoving = true;

	switch (appState) {
	case AppState::InGameSetup:
		for (ElectricObject* thing : objectManager->getElectricObjects()) {
			if (onSprite(thing, vectorPoint) && thing->isMoving) {
				thing->isTouched(vectorPoint);
				noneIsMoving = false;
			}
		}
		if (noneIsMoving) {	// Made for specific case where, if you haven't selected any charge when you were moving, the first you move over becomes the charge that moves.
			for (ElectricObject* thing : objectManager->getElectricObjects()) {
				if (onSprite(thing, vectorPoint)) {
					thing->isTouched(vectorPoint);
					thing->isMoving = true;
					break;
				}
			}
		}
		break;
	case AppState::InGameRunning:
		for (ElectricObject* thing : objectManager->getElectricObjects()) {
			if (onSprite(thing, vectorPoint) && thing->isMoving) {
				thing->isTouched(vectorPoint);
				noneIsMoving = false;
			}
		}
		if (noneIsMoving) {	// Made for specific case where, if you haven't selected any charge when you were moving, the first you move over becomes the charge that moves.
			for (ElectricObject* thing : objectManager->getElectricObjects()) {
				if (onSprite(thing, vectorPoint)) {
					thing->isTouched(vectorPoint);
					thing->isMoving = true;
					break;
				}
			}
		}
		break;
	default:
		break;
	}


}

bool Renderer::onSprite(Sprite* thing, XMFLOAT2 pointer)
{
	Point point = Point(pointer.x, pointer.y);
	Rect box = *thing->getBoundingBox();
	if (thing->getBoundingBox()->Contains(point))
		return true;
	return false;
}