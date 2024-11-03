#version 330 core
out vec4 color;
uniform  sampler2D u_Texture;

in vec2 v_TexCoord;

void main(){
	color = texture(u_Texture, v_TexCoord * 3.0) * vec4(0.2, 0.2, 0.3, 1.0);
}
