#ifndef _CAMERA_H
#define _CAMERA_H
//camera class header file
#include "common.h"

class CCamera
{
public:
	CCamera(){};
	~CCamera(){};

	void Init();
	
	//camera matrix
	XMMATRIX camView;
	XMMATRIX camProjection;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;
};

#endif