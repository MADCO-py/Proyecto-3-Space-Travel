// Generated with OpenAI Codex. Five independently compiled variants via BODY_KIND.
#version 330
in vec3 worldPos;
in vec3 normal;
in vec2 uv;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 eye;
uniform float time;
uniform int kind;
out vec4 finalColor;
void main(){
 vec3 n=normalize(normal), view=normalize(eye-worldPos);
 vec3 light=normalize(-worldPos); // Physical source location: central star at the origin.
 float diffuse=max(dot(n,light),0.0);
 float attenuation=1.0/(1.0+0.000009*dot(worldPos,worldPos));
 vec3 base=texture(texture0,uv).rgb*colDiffuse.rgb;
 vec3 emission=vec3(0);
#if BODY_KIND == 0
 float crack=pow(max(0.0,sin(uv.x*94.0+sin(uv.y*49.0)*3.0)*sin(uv.y*78.0)),18.0);
 emission=vec3(1.0,.19,.025)*crack*(1.8+.7*sin(time*2.4+uv.y*25.0));
#elif BODY_KIND == 1
 float cloud=smoothstep(.57,.85,sin(uv.x*31.0+time*.10+sin(uv.y*23.0))*sin(uv.y*29.0-time*.05));
 base=mix(base,vec3(.85,.91,.88),cloud*.75);
 emission+=vec3(.15,.48,.5)*pow(1.0-max(dot(n,view),0.0),4.0)*.28;
#elif BODY_KIND == 2
 float storm=sin(uv.y*110.0+sin(uv.x*19.0+time*.24)*2.0);
 base*=.86+.14*storm;
#elif BODY_KIND == 3
 float fracture=pow(1.0-abs(sin(uv.x*92.0+sin(uv.y*59.0))),18.0);
 emission=vec3(.12,.43,.8)*fracture*.5;
 base+=vec3(.4,.7,1)*pow(max(dot(reflect(-light,n),view),0.0),70.0)*diffuse;
#elif BODY_KIND == 4
 float cells=pow(max(0.0,sin(uv.x*120.0+sin(uv.y*44.0))*cos(uv.y*95.0)),12.0);
 emission=vec3(.26,1,.52)*cells*(.7+.5*sin(time*1.7+uv.x*21.0))*(1.0-diffuse);
#elif BODY_KIND == 5
 emission=base*(1.4+.17*sin(time*1.4+uv.x*34.0+uv.y*51.0));
 base=vec3(0);
#endif
#if BODY_KIND == 6
 emission+=base*vec3(.19,.25,.29); // Onboard instrument lighting on the ship.
#endif
 vec3 lit=base*(.055+diffuse*attenuation*1.15)*vec3(1.0,.93,.83)+emission;
 finalColor=vec4(pow(lit,vec3(.86)),texture(texture0,uv).a*colDiffuse.a);
}
