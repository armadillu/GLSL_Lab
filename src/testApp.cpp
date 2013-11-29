#include "testApp.h"
#include <GLUT/GLUT.h>

//--------------------------------------------------------------
void testApp::setup(){

	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();
	ofSetWindowPosition(20, 20);

	ofDisableArbTex();
	tex.loadImage("tex.png");
	normalTex.loadImage("gatoNormals.png");
	normalTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	normalNoiseTex.loadImage("normalNoise.png");
	normalNoiseTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	normalLavaTex.loadImage("lavaNormal.png");
	normalLavaTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);


	isShaderDirty = true;

	cam.setDistance(700);
	cam.setFov(30);

	light.enable();
	glEnable(GL_DEPTH_TEST);

	OFX_REMOTEUI_SERVER_SETUP(); 	//start server
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("SHADER");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(doShader);

	vector<string> shL; shL.push_back("SHADER_1"); shL.push_back("SHADER_2");
	shL.push_back("SHADER_3"); 
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(currentShader, SHADER_1, NUM_SHADERS-1, shL);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	vector<string> modelL; modelL.push_back("MODEL_TEAPOT"); modelL.push_back("MODEL_KOOPA");
	modelL.push_back("MODEL_DISNEY");
	modelL.push_back("MODEL_WARIO"); modelL.push_back("MODEL_ROBOT");
	modelL.push_back("MODEL_SONIC");
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(shownModel, MODEL_TEAPOT, NUM_MODELS-1, modelL);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawAxes);


	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(diffuseGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(diffusePow,0,2.0);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularClamp,0,1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularPow,0,2);


	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("LIGHT");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(timeSpeed, 0, 1);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightSpeed,0,10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightH,-100,100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightDist,100,600);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateLight);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateCam);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(fov, 20, 120);


	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("COLORS");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matSpecular);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightSpecular);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shaderVal1,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shaderVal2,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shaderVal3,0,2);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(shaderColorInput);

	OFX_REMOTEUI_SERVER_LOAD_FROM_XML();

	loadModel("WaltDisneyHeadsHigh.3ds");
	loadModel("koopa.3ds");
	loadModel("WaltDisneyHeadsHigh.3ds");
	loadModel("Wario.3ds");
	loadModel("Robot.3ds");
	loadModel("sonic.3ds");

	loadShader("shaders/shader1");
	loadShader("shaders/shader2");
	loadShader("shaders/shader3");
}

void testApp::loadShader(string s){

	ofxAutoReloadedShader*   shader;

	GLuint err = glGetError();	// we need this to clear out the error buffer.
	shader = new ofxAutoReloadedShader();
	shader->setMillisBetweenFileCheck(500);
	shader->load(s);
	err = glGetError();	// we need this to clear out the error buffer.
	ofLogNotice() << "Loaded Shader: " << err;

	shaders.push_back(shader);
}


void testApp::loadModel(string m){

	ofxAssimpModelLoader * model = new ofxAssimpModelLoader();
	model->loadModel(m, true);
	model->setPosition(0, -80, 0);
	model->setRotation(0, 180, 1, 0, 0);
	model->setRotation(0, 180, 0, 0, 1);
    model->setScale(0.3, 0.3, 0.3);
	model->stopAllAnimations();
	//normalize(model);
	models.push_back(model);
}


void testApp::normalize(ofxAssimpModelLoader * m){

	for(int i = 0; i < m->getNumMeshes(); i++){
		ofMesh mesh = m->getMesh(i);
		vector<ofVec3f> norms = mesh.getNormals();
		for(int j = 0; j < norms.size(); j++){
			ofVec3f n = norms[j];
			n.normalize();
		}
	}
}

//--------------------------------------------------------------
void testApp::update(){

	cam.setFov(fov);
	mMatMainMaterial.setAmbientColor( matAmbient);
	mMatMainMaterial.setDiffuseColor(matDiffuse);
	mMatMainMaterial.setSpecularColor(matSpecular);
	mMatMainMaterial.setShininess(25.0f);

	light.setAmbientColor(lightAmbient);
	light.setDiffuseColor(lightDiffuse);
	light.setSpecularColor(lightSpecular);

	if(animateLight || ofGetFrameNum() < 2){
		float r = lightDist;
		lightPos = ofVec3f(r * (sin(lightSpeed * ofGetElapsedTimef())),
						   lightH + 50,
						   r * (cos(lightSpeed * ofGetElapsedTimef()) ) );
	}
	light.setGlobalPosition(lightPos);

	if(animateCam || ofGetFrameNum() < 2){
		float r = 450;
		ofVec3f camPos = ofVec3f(
								 r * sin(lightSpeed * ofGetElapsedTimef() * 1.2),
								 lightH + 100,
								 r * cos(lightSpeed * ofGetElapsedTimef() * 1.2)
								 );

		cam.setGlobalPosition( camPos );
		cam.setTarget(ofVec3f());
	}

	if (ofGetFrameNum()%15 == 1) ofSetWindowTitle( ofToString( ofGetFrameRate()) );
}

//--------------------------------------------------------------
void testApp::draw(){


	////////////////////////////////////////////////////////////

	//light.enable();

//	gpuBlur.beginDrawScene();
	ofClear(0, 0, 0, 0);

	ofBackgroundGradient(ofColor::fromHsb(0, 0, 120), ofColor::fromHsb(0, 0, 0));
	
	cam.begin();
		ofSetColor(255);
		ofSphere(lightPos.x, lightPos.y, lightPos.z, 3);

		glEnable(GL_DEPTH_TEST);
		ofEnableLighting();
		light.enable();
		glDisable(GL_COLOR_MATERIAL);
		mMatMainMaterial.begin();

		if(doShader){

			shaders[(int)currentShader]->begin();

			shaders[(int)currentShader]->setUniformTexture("tex", tex, tex.getTextureReference().getTextureData().textureID);
			shaders[(int)currentShader]->setUniform1f("time", ofGetElapsedTimef() * timeSpeed);

			shaders[(int)currentShader]->setUniform1f("diffuseGain", diffuseGain);
			shaders[(int)currentShader]->setUniform1f("diffusePow", diffusePow);

			shaders[(int)currentShader]->setUniform1f("specularGain", specularGain);
			shaders[(int)currentShader]->setUniform1f("specularClamp", specularClamp);
			shaders[(int)currentShader]->setUniform1f("specularPow", specularPow);
			shaders[(int)currentShader]->setUniform1f("shaderVal1", shaderVal1);
			shaders[(int)currentShader]->setUniform1f("shaderVal2", shaderVal2);
			shaders[(int)currentShader]->setUniform1f("shaderVal3", shaderVal3);
			shaders[(int)currentShader]->setUniform4f("shaderColorInput", shaderColorInput.r/255., shaderColorInput.g/255., shaderColorInput.b/255., shaderColorInput.a/255.);

		}

		switch (shownModel) {
			case MODEL_TEAPOT:
				glutSolidTeapot(80);
				break;

			default:
				//models[(int)shownModel]->disableNormals();
				models[(int)shownModel]->disableColors();
				models[(int)shownModel]->disableTextures();
				models[(int)shownModel]->disableMaterials();
				//glEnable(GL_NORMALIZE);
				models[(int)shownModel]->drawFaces();
				break;
		}


		if(doShader){
			shaders[(int)currentShader]->end();
		}
		mMatMainMaterial.end();
		ofDisableLighting();

		ofSetColor(255);
		if(drawAxes)ofDrawAxis(100);

	cam.end();

	glDisable(GL_DEPTH_TEST);

	////////////////////////////////////////////////////////////

	//ofSetupScreen();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}


void testApp::exit(){
	OFX_REMOTEUI_SERVER_CLOSE();
	OFX_REMOTEUI_SERVER_SAVE_TO_XML();
}
