#pragma once
#ifdef _WIN32

#include <windows.h>
	#include <GL/GL.h>
#endif

#include <Fl/gl.h>
#include <iostream>

#include "../StateManager.h"
#include "../../UtilSources/GLCamera.h"
#include "../../UtilSources/DrawUtils/gltest.h"
#include "../../UtilSources/DrawUtils/Draw2D.h"
#include "../../UtilSources/DrawUtils/SpecialMeshObject.h"

#define GL_SIMPLE_STATE_3D "GLSimpleState3d"

class GLSimpleState3d : public BaseState
{
	friend class StateManager;

public:
	virtual void init(void* data = nullptr);
	virtual void clear();
	virtual void update(void* data = nullptr);
	virtual void render();
	virtual bool initialized();
	virtual bool event(UIEvent* event);

protected:
	GLSimpleState3d();
	~GLSimpleState3d();

private:
	void lightOn();
	void lightOff();

private:
	bool m_initialized;

	OGLCamera m_camera;

	//input
	bool m_buttonPressed;
	int m_startX;
	int m_startY;
};

