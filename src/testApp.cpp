#include "testApp.h"
#include <GLUT/GLUT.h>

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(200);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();
	if(ofGetWindowMode() == OF_WINDOW) ofSetWindowPosition(20, 20);

	ofDisableArbTex();
	tex.loadImage("tex.png");
	tex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	reflectionTex.loadImage("reflectionTex.png");
	reflectionTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	normalTex.loadImage("gatoNormals.png");
	normalTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	normalNoiseTex.loadImage("normalNoise.png");
	normalNoiseTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	normalLavaTex.loadImage("lavaNormal.png");
	normalLavaTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);

	isShaderDirty = true;

	cam.setDistance(700);
	cam.setDrag(0.997);
	camLookAtModelOffset = ofVec3f(0,65,0);
	cam.setGlobalPosition( cam.getGlobalPosition() + camLookAtModelOffset );
	simple_shadow.setup(&cam);


	ofFbo::Settings s;
	s.width = ofGetWidth();
	s.height = ofGetHeight();
	s.internalformat = GL_RGBA;
	s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
	s.maxFilter = GL_LINEAR; GL_NEAREST;
	s.numSamples = 4;
	s.numColorbuffers = 3;
	s.useDepth = true;
	s.useStencil = false;

	shadowBlur.setup(s, false);
	shadowBlur.setBackgroundColor(ofColor(0,0));
	shadowBlur.blurOffset = 0.2;
	shadowBlur.blurPasses = 1;
	shadowBlur.numBlurOverlays =  1;
	shadowBlur.blurOverlayGain = 255;

	sceneBlur.setup(s, true);
	sceneBlur.setBackgroundColor(ofColor(0,0));
	sceneBlur.blurOffset = 0.2;
	sceneBlur.blurPasses = 1;
	sceneBlur.numBlurOverlays =  1;
	sceneBlur.blurOverlayGain = 255;



	light.enable();
	glEnable(GL_DEPTH_TEST);

	OFX_REMOTEUI_SERVER_SETUP(); 	//start server
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("SHADER");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(doShader);

	vector<string> shL; shL.push_back("SHADER_1"); shL.push_back("SHADER_2");
	shL.push_back("SHADER_3"); shL.push_back("SHADER_4");
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(currentShader, SHADER_1, NUM_SHADERS-1, shL);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	vector<string> modelL; modelL.push_back("MODEL_TEAPOT"); modelL.push_back("MODEL_KOOPA");
	modelL.push_back("WALT DISNEY"); modelL.push_back("WARIO"); modelL.push_back("ROBOT");
	modelL.push_back("SONIC"); modelL.push_back("HYDRALISK"); modelL.push_back("ZERGLING");
	modelL.push_back("MARIO"); modelL.push_back("CHARIZARD"); modelL.push_back("LINK");
	modelL.push_back("PENGUIN"); modelL.push_back("LUIGI");
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(shownModel, MODEL_TEAPOT, NUM_MODELS-1, modelL);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawAxes);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(bgColor);

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
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightH,-100,300);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightDist,10,600);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shadowY,-100,100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shadowAlpha,0,1);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shadowBlur.blurPasses,0,5);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shadowBlur.blurOffset,0,5);

	OFX_REMOTEUI_SERVER_SHARE_PARAM(sceneBlur.blurPasses,0,5);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(sceneBlur.blurOffset,0,5);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(sceneBlur.numBlurOverlays,0,5);

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

	cam.setFov(fov);


	loadModel("WaltDisneyHeadsHigh.3ds");
	loadModel("koopa.3ds");
	loadModel("WaltDisneyHeadsHigh.3ds");
	loadModel("Wario.3ds");
	loadModel("Robot.3ds");
	loadModel("sonic.3ds");
	loadModel("hydralisk.3DS", -10, 0.5);
	loadModel("zergling.3DS", -20, 0.5);
	loadModel("mario.3DS");
	loadModel("charizard.3DS");
	loadModel("link.3DS");
	loadModel("penguin.3DS");
	loadModel("luigi.3DS");

	loadShader("shaders/shader1");
	loadShader("shaders/shader2");
	loadShader("shaders/shader3");
	loadShader("shaders/shader4");
}

void testApp::windowResized(int w, int h){

	ofFbo::Settings s;
	s.width = ofGetWidth();
	s.height = ofGetHeight();
	s.internalformat = GL_RGBA;
	s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
	s.maxFilter = GL_LINEAR; GL_NEAREST;
	s.numSamples = 4;
	s.numColorbuffers = 3;
	s.useDepth = true;
	s.useStencil = false;

	shadowBlur.setup(s, false);
	sceneBlur.setup(s, false);

	simple_shadow.setup(&cam);
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


void testApp::loadModel(string m, float voffset, float scale){

	ofxAssimpModelLoader * model = new ofxAssimpModelLoader();
	model->loadModel(m, true);
	model->setPosition(0, voffset, 0);
	model->setRotation(0, 180, 1, 0, 0);
	model->setRotation(0, 180, 0, 0, 1);
    model->setScale(scale, scale, scale);
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

	ofBackground(bgColor);
	cam.setFov(fov);
	mMatMainMaterial.setAmbientColor( matAmbient);
	mMatMainMaterial.setDiffuseColor(matDiffuse);
	mMatMainMaterial.setSpecularColor(matSpecular);
	mMatMainMaterial.setShininess(25.0f);

	light.setAmbientColor(lightAmbient);
	light.setDiffuseColor(lightDiffuse);
	light.setSpecularColor(lightSpecular);

	simple_shadow.setLightPosition(light.getGlobalPosition());
	simple_shadow.setShadowColor( ofFloatColor(0, 1.0) );


	if(animateLight || ofGetFrameNum() < 2){
		float r = lightDist;
		lightPos = ofVec3f(r * (sin(lightSpeed * ofGetElapsedTimef())),
						   lightH + 50,
						   r * (cos(lightSpeed * ofGetElapsedTimef()) ) );
	}
	light.setGlobalPosition(lightPos);

	if(animateCam ){
		float r = 450;
		ofVec3f camPos = ofVec3f(
								 r * sin(lightSpeed * ofGetElapsedTimef() * 0.4),
								 camLookAtModelOffset.y,
								 r * cos(lightSpeed * ofGetElapsedTimef() * 0.4)
								 );

		cam.setGlobalPosition( camPos );
		cam.setTarget(camLookAtModelOffset);
	}

	if (ofGetFrameNum()%15 == 1) ofSetWindowTitle( ofToString( ofGetFrameRate()) );
}

//--------------------------------------------------------------
void testApp::draw(){

	//shadow
	light.disable();
	ofDisableLighting();

	shadowBlur.beginDrawScene(); //blurscene only has shadow
		ofClear(255, 255, 255, 0);

		cam.begin();
			simple_shadow.begin();
			glDisable(GL_DEPTH_TEST);
			drawModel();
			simple_shadow.end();
		cam.end();
	shadowBlur.endDrawScene();

	glDisable(GL_DEPTH_TEST);
	shadowBlur.performBlur();

	ofSetColor( 255, 255 * shadowAlpha);
	//shadowBlur.drawSceneFBO();
	shadowBlur.drawBlurFbo(true);

	/**/sceneBlur.beginDrawScene();
	ofClear(255, 255, 255, 0);

	//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	light.enable();
	cam.begin();
		ofSetColor(255);
		ofSphere(lightPos.x, lightPos.y, lightPos.z, 3);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		ofEnableLighting();
		light.enable();
		glDisable(GL_COLOR_MATERIAL);
		mMatMainMaterial.begin();

		if(doShader){

			shaders[(int)currentShader]->begin();

			shaders[(int)currentShader]->setUniformTexture("tex", tex, tex.getTextureReference().getTextureData().textureID);
			shaders[(int)currentShader]->setUniformTexture("reflectionTex", reflectionTex, reflectionTex.getTextureReference().getTextureData().textureID);
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
	ofPushMatrix();
	ofTranslate(0, shadowY, 0);
			drawModel();
	ofPopMatrix();

		if(doShader){
			shaders[(int)currentShader]->end();
		}
		simple_shadow.end();
		mMatMainMaterial.end();
		ofDisableLighting();

		ofSetColor(255);
		if(drawAxes)ofDrawAxis(100);

	cam.end();

	glDisable(GL_DEPTH_TEST);

/**/	sceneBlur.endDrawScene();

	sceneBlur.performBlur();

	sceneBlur.drawSceneFBO();

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	sceneBlur.drawBlurFbo();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

}

void testApp::drawModel(){

	switch (shownModel) {
		case MODEL_TEAPOT:
			glutSolidTeapot(60);
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
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}
