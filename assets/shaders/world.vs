// Generated with OpenAI Codex. Raylib 5.5 standard vertex attribute/uniform convention.
#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform float time;
uniform int kind;
out vec3 worldPos;
out vec3 normal;
out vec2 uv;
void main(){
 vec3 p=vertexPosition;
 // Nival's crystal crust changes geometry, not only color.
 if(kind==3) p*=1.0+0.013*sin(p.x*19.0+time)*sin(p.z*23.0-time*.7);
 worldPos=(matModel*vec4(p,1)).xyz;
 normal=normalize((matNormal*vec4(vertexNormal,0)).xyz);
 uv=vertexTexCoord;
 gl_Position=mvp*vec4(p,1);
}
