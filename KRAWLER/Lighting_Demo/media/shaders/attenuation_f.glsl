#version 110

varying vec4 vertColor;
uniform sampler2D texture;
uniform vec2 point;
float lightIntensity;
uniform vec4 lightColour;

float calculateAttenuation() 
{ 
    //TODO: move values to C++ and store in header to allow changes
    float constAtten = 0.0; 
    float linearAtten = 0.0; 
    float quadraticAtten = 0.002;
    
    float distance = length(gl_FragCoord.xy - point);
    float distanceSquared = distance * distance;
    float attenVal = 200.0;
    // return clamp(1.f / (constAtten + (linearAtten * distance) + (quadraticAtten * distanceSquared)), 0.0f, 1.0f);
    return clamp ((1.0 / (distance + distanceSquared)) * attenVal , 0.0, 1.0);
}

void main(void)
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    vec4 col = vec4(vertColor);
    
    col.xyz += pixel.xyz;
    col.w *= calculateAttenuation();
    
    gl_FragColor = col;
}
