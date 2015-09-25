#include "camera.h"
void CCamera::Init()
{
		//Camera information
	camPosition = XMVectorSet( 0.0f, 0.0f, -0.5f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	//Set the View matrix
	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

	//Set the Projection matrix
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, SCREEN_WIDTH/SCREEN_HEIGHT, 1.0f, 1000.0f);
}