// Generated with OpenAI Codex. Two-sided translucent ring lit by Helios, with planetary shadow.
#version 330
in vec3 worldPos;
in vec3 normal;
in vec2 uv;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 center;
uniform float radius;
out vec4 finalColor;
void main(){
 vec3 light=normalize(-worldPos),delta=center-worldPos;
 float along=dot(delta,light);
 float shadow=(along>0. && length(delta-light*along)<radius)? .12:1.;
 float illumination=.22+.78*abs(dot(normalize(normal),light))*shadow;
 vec4 tex=texture(texture0,uv);
 finalColor=vec4(tex.rgb*illumination,tex.a*colDiffuse.a);
}
