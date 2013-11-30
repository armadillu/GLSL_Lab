#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"
#include "ofxRemoteUIServer.h"
#include "ofxAssimpModelLoader.h"

class testApp : public ofBaseApp{

	enum models{
				MODEL_TEAPOT = 0, MODEL_DISNEY, MODEL_KOOPA, MODEL_WARIO,
				MODEL_ROBOT, MODEL_SONIC, MODEL_HYDRALISK, MODEL_ZERLING,
				MODEL_MARIO, MODEL_CHARIZARD, MODEL_LINK, MODEL_PENGUIN,
				MODEL_LUIGI, NUM_MODELS};
	enum Shaders{SHADER_1 = 0, SHADER_2, SHADER_3, SHADER_4, NUM_SHADERS};

	ofEasyCam	cam;
	vector<ofxAutoReloadedShader*> shaders;
	ofMaterial	mMatMainMaterial;

	ofVec3f		lightPos;
	ofLight		light;
	
	bool		isShaderDirty;

	ofColor matAmbient;
	ofColor matDiffuse;
	ofColor matSpecular;

	ofColor lightAmbient;
	ofColor lightDiffuse;
	ofColor lightSpecular;

	bool animateLight;
	bool animateCam;
	bool doShader;
	bool drawAxes;

	float fov;
	float lightSpeed;
	float lightDist;
	float timeSpeed;
	models shownModel;
	Shaders currentShader;

	float diffuseGain, diffusePow;
	float specularGain, specularClamp, specularPow;

	float shaderVal1, shaderVal2, shaderVal3;
	ofColor shaderColorInput;

	float lightH;

	vector<ofxAssimpModelLoader*> models;

	ofImage tex;
	ofImage normalTex;
	ofImage normalNoiseTex;
	ofImage normalLavaTex;
	ofImage maskLavaTex;
	
	public:
		void setup();
		void update();
		void draw();

	void loadModel(string m, float voffset = -70, float scale = 0.3);
	void loadShader(string s);
	void normalize(ofxAssimpModelLoader * m);
	void exit();

	void keyPressed  (int key);
		
};
