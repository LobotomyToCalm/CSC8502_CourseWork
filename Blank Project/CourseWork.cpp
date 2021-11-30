#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Welcome to Deckard's island", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			if (renderer.camMode == 4)renderer.camMode = 1;
			else renderer.camMode += 1;
			renderer.SetCamPosition(renderer.camMode);
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			renderer.camForwardSwitch = !renderer.camForwardSwitch;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			renderer.camRotateSwitch = !renderer.camRotateSwitch;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L)) {
			renderer.waterLineSwitch = !renderer.waterLineSwitch;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_7)) {
				renderer.drawMode = NORMAL_DRAW;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_8)) {
			renderer.drawMode == BLUR_DRAW ? 
				renderer.drawMode = NORMAL_DRAW : renderer.drawMode = BLUR_DRAW;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_9)) {
			renderer.drawMode == DEFFERED_DRAW ?
				renderer.drawMode = NORMAL_DRAW : renderer.drawMode = DEFFERED_DRAW;
		}
		
	}
	return 0;
}