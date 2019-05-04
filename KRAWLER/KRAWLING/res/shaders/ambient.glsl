#version 110 

void main()
{
	// transform the vertex position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	// transform the texture coordinates
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	// forward the vertex color
	gl_FrontColor = gl_Color; // sets bg to black
}
#DIVIDE
#version 110 
uniform sampler2D texture;

// varying vec4 vColour;

//Shader parameters
// uniform vec2 resolution;
uniform vec4 ambientColour; 
uniform float intensity;
// uniform sampler2D lightMapTexture; 

void main()
{
	//Get the colour value of the pixel in the texture
	vec4 diffuseColour = texture2D(texture, gl_TexCoord[0].xy);
	/* 
	//Find the UV coordinate of the current pixel to navigate our light texture
	vec2 lightCoord = (gl_FragCoord.xy / resolution.xy);
	//Work out the colour at this uv coordinate in the light texture
	vec4 light = texture2D(lightMapTexture, lightCoord);
	*/ 

	//Calculate the ambient lighting values into a vec3 (with intensity factored in)
	//vec3 ambient = ambientColour.rgb * ambientColour.a; 
	//vec3 intensity = ambient + light.rgb;
	//vec3 finalColor = diffuseColour.rgb * intensity; //Determine the final colour of the ambient lighting
	//Set the frag colour
	gl_FragColor =((1.0 / intensity) * ambientColour)*diffuseColour;//vColour * vec4(finalColor, diffuseColour.a);
}