#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "ofAppGLFWWindow.h"

//--------------------------------------------------------------
int main(){
	ofAppGLFWWindow win;
	win.setNumSamples(16);
	win.setMultiDisplayFullscreen(true);

	ofSetupOpenGL(&win, 1024,768, OF_WINDOW);	// <-------- setup the GL context

	ofRunApp(new testApp());
}
