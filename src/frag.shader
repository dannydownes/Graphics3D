#version 440 core

in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
uniform vec3 lightColour;
uniform float lightDistance;

void main()
{
	vec4 TexColour = texture(ourTexture, TexCoord);
	color = vec4(lightDistance * lightColour.x * TexColour.x, lightDistance * lightColour.y*TexColour.y, lightDistance * lightColour.z*TexColour.z, TexColour.a);
}