#extension GL_EXT_gpu_shader4 : require

varying vec3 normal, lightDir, eyeVec;
varying vec2 vTexCoord;

uniform sampler2D tex;
uniform sampler2D reflectionTex;
uniform float time;

uniform float diffuseGain;
uniform float diffusePow;

uniform float specularGain;
uniform float specularClamp;
uniform float specularPow;

uniform float shaderVal1; //env mapping
uniform float shaderVal2; //normal map
uniform float shaderVal3; //outer halo range
uniform float shaderVal4; //outer halo alpha

uniform vec4 shaderColorInput;

vec3 vertical = vec3(0.,1.,0.);

vec4 envReflection(vec3 E, vec3 N){
	vec3 reflectDir = reflect(E, N);
	// compute altitude and azimuth angles
	vec2 index;
	index.y = dot(normalize(reflectDir), vec3(0.0, 1.0, 0.0));
	reflectDir.y = 0.0;
	index.x = dot(normalize(reflectDir), vec3(1.0, 0.0, 0.0)) * 0.5;
    // Translate index values into proper range
	if (reflectDir.z >= 0.0){
		index = (index + 1.0) * 0.5;
	}else{
		index.t = (index.t + 1.0) * 0.5;
		index.s = (-index.s) * 0.5 + 1.0;
	}
	return texture2D(reflectionTex, index);
}


void main( void ){

	// TEX /////////////////////////////////////
	vec4 texPix = texture2D(tex, vTexCoord * 5.0);

	float x = gl_FragCoord.x / 1024.0; //pixel location on screen
	float y = gl_FragCoord.y / 768.0;

	vec3 T = texPix.rgb;
	vec3 L = normalize(lightDir);
	vec3 N = normalize( mix(normal, normal * T, shaderVal2) ); //Normal with tex applied / merged
	vec3 E = normalize(eyeVec);
	vec3 R = reflect(-L, N);

	vec4 ambientV = vec4(0.0);
	vec4 diffuseV = vec4(0.0);
	vec4 specularV = vec4(0.0);

	// AMBIENT /////////////////////////////////
	ambientV = 	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) 
				+
		        (gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
	

	// DIFFUSE /////////////////////////////////
	float lambertTerm = dot(N, L);

	if(lambertTerm  > 0.0){
		diffuseV += diffuseGain * gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * pow(lambertTerm , diffusePow );

		// SPECULAR /////////////////////////////////
		float specular = specularGain * clamp( pow( max(dot(R,E), 0.0), gl_FrontMaterial.shininess * specularPow ), 0.0, specularClamp);
		specularV = gl_LightSource[0].specular * gl_FrontMaterial.specular * specular ;
	}

	// REFLECTION ENV /////////////////////////
	vec4 envReflectionTex = vec4(shaderVal1 * envReflection(E, N).rgb, 1.0);


	// OUTER_HALO ////////////////////////////
	float verticalness = dot(vertical, N);
	float rightness = dot(vec3(1.,0.,0.), N);
	float leftness = dot(vec3(-1.,0.,0.), N);

	float outerHaloV = 1.0 - dot(E, N); // side/edge glow
	outerHaloV = shaderVal4 * pow(outerHaloV, shaderVal3 * 3.0);

	vec4 outerHalo = vec4(outerHaloV * shaderColorInput.rgb, 1.0);

	// FINAL COLOR //
	gl_FragColor = 	(ambientV + diffuseV) * envReflectionTex //in case we have relfection
					+
					(ambientV + diffuseV) * (1.0 - shaderVal1) //in case relection is 0 (reflect is modulated by shaderVal1
					+
					specularV 
					+ 
					outerHalo * envReflectionTex
					; 

	//gl_FragColor = vec4(leftness, rightness, verticalness, 1.0);
	//gl_FragColor = vec4(refract(L, N, shaderVal2), 1.0);
}

