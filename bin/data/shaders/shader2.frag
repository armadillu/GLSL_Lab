#extension GL_EXT_gpu_shader4 : require

varying vec3 normal, lightDir, eyeVec;
varying vec2 vTexCoord;

uniform sampler2D tex;
uniform float time;

uniform float diffuseGain;
uniform float diffusePow;

uniform float specularGain;
uniform float specularClamp;
uniform float specularPow;

uniform float shaderVal1;
uniform float shaderVal2;
uniform float shaderVal3;

uniform vec4 shaderColorInput;

void main( void ){

	// TEX /////////////////////////////////////
	vec4 texPix = texture2D(tex, vTexCoord);
	float x = gl_FragCoord.x / 1024.0;
	float y = gl_FragCoord.y / 768.0;

	vec3 L = normalize(lightDir);
	vec3 N = normalize(normal) ;
	
	vec4 ambientV = vec4(0.0);
	vec4 diffuseV = vec4(0.0);
	vec4 specularV = vec4(0.0);
	
	// AMBIENT /////////////////////////////////
	ambientV = 	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) 
				+
		        (gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
	
	// DIFFUSE /////////////////////////////////
	
	float lambertTerm = dot(N, L);
	vec3 E = normalize(eyeVec);
	vec3 R = reflect(-L, N);
	
	float weird = 1.0 - dot(E, N);
	weird = pow(weird, shaderVal2 * 3.0);
	
	if(lambertTerm  > 0.0){
		diffuseV += diffuseGain * gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * pow(lambertTerm , diffusePow );

		// SPECULAR /////////////////////////////////
		float specular = specularGain * clamp( pow( max(dot(R,E), 0.0), gl_FrontMaterial.shininess * specularPow ), 0.0, specularClamp);
		specularV = gl_LightSource[0].specular * gl_FrontMaterial.specular * specular ;	
	}
	gl_FragColor = ambientV + diffuseV  + specularV + shaderVal1 * shaderColorInput  * weird ;
	
	//gl_FragColor =  vec4(weird,weird,weird,1.0);
}

