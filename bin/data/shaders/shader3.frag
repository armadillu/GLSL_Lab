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
	vec4 texPix = texture2D(tex, vTexCoord * 20.0);

	float x = gl_FragCoord.x / 1024.0; //pixel location on screen
	float y = gl_FragCoord.y / 768.0;

	vec3 T = texPix.rgb;
	vec3 L = normalize(lightDir);
	vec3 N = normalize( normal );
	vec3 Ndirty = normalize( normal * T ); //Normal with tex applied / merged
	
	vec4 ambientV = vec4(0.0);
	vec4 diffuseV = vec4(0.0);
	vec4 specularV = vec4(0.0);
	
	// AMBIENT /////////////////////////////////
	ambientV = 	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) 
				+
		        (gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
	
	// DIFFUSE /////////////////////////////////
	
	float lambertTerm = dot(Ndirty, L);
	vec3 E = normalize(eyeVec);
	vec3 R = reflect(-L, Ndirty);
	

	//float d = distance(N, L);
	//vec3 X =(N);
	
	//float weird = 1.0 - dot(E, N); // side/edge glow
	//weird = pow(weird, shaderVal2 * 3.0);

	vec3 reflectDir = normalize( vec3( 1.0 - cos(time), cos(time), sin(time) ) );
	float weird2 = pow( 1.0 - pow( dot( mix(N, Ndirty, shaderVal3) , reflectDir ), shaderVal2 ), shaderVal1 * 20.0);
	float specular = 0.0;
	if(lambertTerm  > 0.0){
		diffuseV += diffuseGain * gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * pow(lambertTerm , diffusePow );

		// SPECULAR /////////////////////////////////
		specular = specularGain * clamp( pow( max(dot(R,E), 0.0), gl_FrontMaterial.shininess * specularPow ), 0.0, specularClamp);
		specularV = gl_LightSource[0].specular * gl_FrontMaterial.specular * specular ;	
	}
	gl_FragColor = ambientV + diffuseV  + specularV + shaderColorInput * weird2 * specular;
	
	//gl_FragColor =  vec4(weird,weird,weird,1.0);	
	//gl_FragColor =  vec4(weird2,weird2,weird2,1.0);	
	
	//gl_FragColor =  vec4(X.rgb,1.0);	
}

