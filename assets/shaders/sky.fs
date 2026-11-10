// Generated with OpenAI Codex. Infinite star sphere anchored to camera, no image dependency.
#version 330
in vec3 worldPos;
uniform vec3 eye;
out vec4 finalColor;
float hash(vec3 p){return fract(sin(dot(p,vec3(127.1,311.7,74.7)))*43758.5453);}
void main(){
 vec3 d=normalize(worldPos-eye);
 vec3 base=mix(vec3(.006,.012,.028),vec3(.025,.037,.062),.5+.5*d.y);
 float band=pow(max(0.,1.-abs(d.y*.75+d.x*.3+.13)),18.);
 base+=vec3(.025,.032,.049)*band*(.7+.3*sin(d.x*21.+d.z*16.));
 vec3 cell=floor(d*650.);float h=hash(cell);
 float star=step(.9982,h);
 finalColor=vec4(base+star*mix(vec3(.40,.61,.85),vec3(1.,.83,.57),hash(cell+1.))*.85,1);
}
