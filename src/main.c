/* Generated with OpenAI Codex. CPU rasterizer: no Raylib drawing functions.
   Raylib handles only window, timing, input and vector types/math.
   OpenGL only uploads/presents the completed CPU framebuffer. */
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#define Font X11Font
#include <X11/Xlib.h>
#undef Font
#include "raylib.h"
#include "raymath.h"
#include "sim.h"
#include "obj.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>
#define WIDTH 800
#define HEIGHT 500
#define TEXW 512
#define TEXH 256
static unsigned char pixels[HEIGHT][WIDTH][4], textures[6][TEXH][TEXW][3];
static float depth[HEIGHT][WIDTH];
typedef struct {Vector3 p,n; Vector2 uv;} Vertex;
typedef struct {Vertex *v; int count;} Geometry;
typedef struct {Vector3 pos,forward,right,up;} View;
static View view;
static Geometry globe[3];static ObjData hull;
static double simtime;
static int culled,triangles,lods[3];
static unsigned int noise(unsigned int n){n^=n>>16;n*=0x7feb352d;n^=n>>15;n*=0x846ca68b;n^=n>>16;return n;}
static void pixel(int x,int y,Color c){if(x<0||y<0||x>=WIDTH||y>=HEIGHT)return;unsigned char *p=pixels[y][x];float a=c.a/255.f;p[0]=(unsigned char)(p[0]*(1-a)+c.r*a);p[1]=(unsigned char)(p[1]*(1-a)+c.g*a);p[2]=(unsigned char)(p[2]*(1-a)+c.b*a);p[3]=255;}
static Color rgb(Vector3 c){return (Color){(unsigned char)(Clamp(c.x,0,1)*255),(unsigned char)(Clamp(c.y,0,1)*255),(unsigned char)(Clamp(c.z,0,1)*255),255};}
static void rect(int x,int y,int w,int h,Color c){for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++)pixel(i,j,c);}
// Original compact 5x7 bitmap alphabet. Every glyph is rasterized into our CPU buffer.
static const unsigned char glyphs[][5]={
{126,17,17,17,126},{127,73,73,73,54},{62,65,65,65,34},{127,65,65,34,28},{127,73,73,73,65},{127,9,9,9,1},{62,65,73,73,122},{127,8,8,8,127},{65,65,127,65,65},{32,64,65,63,1},{127,8,20,34,65},{127,64,64,64,64},{127,2,12,2,127},{127,4,8,16,127},{62,65,65,65,62},{127,9,9,9,6},{62,65,81,33,94},{127,9,25,41,70},{38,73,73,73,50},{1,1,127,1,1},{63,64,64,64,63},{31,32,64,32,31},{63,64,56,64,63},{99,20,8,20,99},{7,8,112,8,7},{97,81,73,69,67},
{62,81,73,69,62},{0,66,127,64,0},{98,81,73,73,70},{34,65,73,73,54},{24,20,18,127,16},{39,69,69,69,57},{62,73,73,73,50},{1,113,9,5,3},{54,73,73,73,54},{38,73,73,73,62}};
static void text(int x,int y,const char *s,int scale,Color c){for(;*s;s++,x+=6*scale){int g=*s>='A'&&*s<='Z'?*s-'A':*s>='0'&&*s<='9'?26+*s-'0':-1;if(g>=0)for(int i=0;i<5;i++)for(int j=0;j<7;j++)if(glyphs[g][i]&(1<<j))rect(x+i*scale,y+j*scale,scale,scale,c);else{}else if(*s=='-')rect(x,y+3*scale,5*scale,scale,c);else if(*s=='.')rect(x,y+6*scale,scale,scale,c);else if(*s=='/')for(int j=0;j<7;j++)rect(x+(4-j*4/7)*scale,y+j*scale,scale,scale,c);}}
// Generated with OpenAI Codex: anatomical eye texture, calculated once in CPU.
// Front of the eye is local +Z. Brown iris fibers and sparse branching scleral vessels.
static Vector3 eye_texture(float u,float v){
 float x=sinf(v)*cosf(u), y=cosf(v), z=sinf(v)*sinf(u);
 float radius=acosf(Clamp(z,-1,1))/.57f, a=atan2f(y,x);
 Vector3 color={.91f,.89f,.85f};
 if(radius<1){
  float fibers=.5f+.23f*sinf(a*137+radius*13)+.17f*sinf(a*231-radius*27)+.10f*cosf(a*61+radius*54);
  color=Vector3Lerp((Vector3){.13f,.072f,.027f},(Vector3){.48f,.32f,.13f},Clamp(fibers,0,1));
  float collarette=expf(-powf((radius-.47f)/.08f,2));
  color=Vector3Add(color,Vector3Scale((Vector3){.13f,.075f,.018f},collarette));
  float limbus=Clamp((1-radius)/.095f,0,1);
  color=Vector3Lerp((Vector3){.065f,.047f,.03f},color,limbus);
 }else{
  float vessel=fabsf(sinf(a*13+.38f*sinf(radius*8)+.16f*sinf(radius*21+a*3)));
  float branch=fabsf(sinf(a*27-radius*3+.32f*sinf(radius*15)));
  float strength=Clamp((radius-1.08f)*.7f,0,.45f);
  float vein=expf(-vessel*vessel*2100)*strength+expf(-branch*branch*4200)*strength*.3f;
  color=Vector3Lerp(color,(Vector3){.56f,.22f,.20f},Clamp(vein,0,.5f));
 }
 return color;
}
static void make_textures(void){for(int k=0;k<6;k++)for(int y=0;y<TEXH;y++)for(int x=0;x<TEXW;x++){
 float u=x/(float)TEXW*2*PI,v=y/(float)TEXH*PI;
 float n=sinf(u*7+sinf(v*11))*cosf(v*9+sinf(u*13));float f=.5f+.5f*n;
 Vector3 a,b;
 switch(k){case 0:a=(Vector3){.06,.04,.04};b=(Vector3){.5,.19,.07};break;case 1:a=(Vector3){.01,.08,.23};b=(Vector3){.08,.5,.38};f=Clamp((n-.2f)*2,0,1);break;case 2:a=(Vector3){.29,.13,.08};b=(Vector3){.88,.7,.39};f=.5f+.5f*sinf(v*42+sin(u*3));break;case 3:a=(Vector3){.17,.35,.6};b=(Vector3){.85,.93,.99};break;case 4:a=(Vector3){.13,.04,.22};b=(Vector3){.05,.35,.19};break;default:a=(Vector3){1,.25,.01};b=(Vector3){1,.85,.3};break;}
 Vector3 c=k==4?eye_texture(u,v):Vector3Lerp(a,b,f);for(int j=0;j<3;j++)textures[k][y][x][j]=(unsigned char)((j==0?c.x:j==1?c.y:c.z)*255);
}}
static Vector3 sample(int kind,Vector2 uv){int x=((int)floorf(uv.x*TEXW)%TEXW+TEXW)%TEXW;int y=(int)Clamp(uv.y*(TEXH-1),0,TEXH-1);unsigned char *p=textures[kind][y][x];return (Vector3){p[0]/255.f,p[1]/255.f,p[2]/255.f};}
static Color shade(int kind,Vector3 p,Vector3 n,Vector2 uv){
 if(kind==6)return rgb(Vector3Scale((Vector3){.42,.65,.72},.35f+.65f*fmaxf(0,Vector3DotProduct(n,Vector3Normalize(Vector3Subtract(view.pos,p))))));
 Vector3 base=sample(kind,uv),light=Vector3Normalize(Vector3Negate(p));float lam=fmaxf(0,Vector3DotProduct(n,light));float intensity=.07f+lam*1.2f/(1+.000009f*Vector3LengthSqr(p));Vector3 emission={0};float t=(float)simtime;
 if(kind==0){float crack=powf(fmaxf(0,sinf(uv.x*95+sinf(uv.y*43)*3)*sinf(uv.y*78)),16);emission=Vector3Scale((Vector3){1,.18,.01},crack*(1.2f+.5f*sinf(t*2+uv.y*30)));}
 if(kind==1){float cloud=Clamp((sinf(uv.x*31+t*.12f+sinf(uv.y*25))*sinf(uv.y*21)-.5f)*2,0,1);base=Vector3Lerp(base,(Vector3){.8,.9,.9},cloud*.8f);}
 if(kind==2)base=Vector3Scale(base,.86f+.14f*sinf(uv.y*120+sinf(uv.x*25+t*.3f)));
 if(kind==3){float ice=powf(1-fabsf(sinf(uv.x*92+sinf(uv.y*59))),16);emission=Vector3Scale((Vector3){.05,.3,.8},ice*.6f);}
 if(kind==4){
  float angle=acosf(Clamp(sinf(uv.y*PI)*sinf(uv.x*2*PI),-1,1))/.57f;
  // Subtle pupil response preserves the body's unique animated software shader.
  float pupil=.32f+.025f*sinf(t*.65f);
  float edge=Clamp((angle-pupil)/.018f,0,1);
  base=Vector3Lerp((Vector3){.004f,.003f,.002f},base,edge);
  Vector3 towardEye=Vector3Normalize(Vector3Subtract(view.pos,p));
  Vector3 halfway=Vector3Normalize(Vector3Add(light,towardEye));
  float gloss=powf(fmaxf(0,Vector3DotProduct(n,halfway)),90)*lam*(angle<1.05f?.85f:.2f);
  emission=(Vector3){gloss,gloss,gloss};
 }
 if(kind==5){intensity=1.3f+.15f*sinf(t+uv.x*80);}
 return rgb(Vector3Add(Vector3Scale(base,intensity),emission));
}
typedef struct {float x,y,z;Vertex v;} Projected;
static Projected project(Vertex v){Vector3 d=Vector3Subtract(v.p,view.pos);float z=Vector3DotProduct(d,view.forward);return (Projected){WIDTH*.5f+Vector3DotProduct(d,view.right)*480/z,HEIGHT*.5f-Vector3DotProduct(d,view.up)*480/z,z,v};}
static float edge(float ax,float ay,float bx,float by,float x,float y){return (x-ax)*(by-ay)-(y-ay)*(bx-ax);}
static void triangle(Vertex a,Vertex b,Vertex c,int kind){
 Vector3 normal=Vector3CrossProduct(Vector3Subtract(b.p,a.p),Vector3Subtract(c.p,a.p));
 if(Vector3DotProduct(normal,Vector3Subtract(view.pos,a.p))<=0){culled++;return;}
 Projected p=project(a),q=project(b),r=project(c);if(p.z<.1f||q.z<.1f||r.z<.1f)return;
 float area=edge(p.x,p.y,q.x,q.y,r.x,r.y);if(fabsf(area)<.001f)return;
 int minx=(int)Clamp(floorf(fminf(p.x,fminf(q.x,r.x))),0,WIDTH-1),maxx=(int)Clamp(ceilf(fmaxf(p.x,fmaxf(q.x,r.x))),0,WIDTH-1);
 int miny=(int)Clamp(floorf(fminf(p.y,fminf(q.y,r.y))),0,HEIGHT-1),maxy=(int)Clamp(ceilf(fmaxf(p.y,fmaxf(q.y,r.y))),0,HEIGHT-1);triangles++;
 for(int y=miny;y<=maxy;y++)for(int x=minx;x<=maxx;x++){
  float w0=edge(q.x,q.y,r.x,r.y,x+.5f,y+.5f)/area,w1=edge(r.x,r.y,p.x,p.y,x+.5f,y+.5f)/area,w2=1-w0-w1;
  if(w0<0||w1<0||w2<0)continue;
  w0/=p.z;w1/=q.z;w2/=r.z;float z=1/(w0+w1+w2);if(z>=depth[y][x])continue;
  w0*=z;w1*=z;w2*=z;Vector3 pos=Vector3Add(Vector3Add(Vector3Scale(a.p,w0),Vector3Scale(b.p,w1)),Vector3Scale(c.p,w2));
  Vector3 n=Vector3Normalize(Vector3Add(Vector3Add(Vector3Scale(a.n,w0),Vector3Scale(b.n,w1)),Vector3Scale(c.n,w2)));
  Vector2 uv={a.uv.x*w0+b.uv.x*w1+c.uv.x*w2,a.uv.y*w0+b.uv.y*w1+c.uv.y*w2};
  Color color=shade(kind,pos,n,uv);pixel(x,y,color);depth[y][x]=z;
 }
}
static Vertex sphere_vertex(int i,int j,int n){float u=i/(float)(2*n),v=j/(float)n,a=u*2*PI,b=v*PI;Vector3 p={sinf(b)*cosf(a),cosf(b),sinf(b)*sinf(a)};return (Vertex){p,p,{u,v}};}
static void make_spheres(void){int sizes[3]={48,24,12};for(int l=0;l<3;l++){int n=sizes[l];Geometry *g=&globe[l];g->count=n*2*n*6;g->v=malloc((size_t)g->count*sizeof(Vertex));int k=0;
 for(int j=0;j<n;j++)for(int i=0;i<2*n;i++){Vertex a=sphere_vertex(i,j,n),b=sphere_vertex(i+1,j,n),c=sphere_vertex(i,j+1,n),d=sphere_vertex(i+1,j+1,n);g->v[k++]=a;g->v[k++]=b;g->v[k++]=c;g->v[k++]=b;g->v[k++]=d;g->v[k++]=c;}
}}
static void sphere(int kind,Vector3 center,float radius){int l=choose_lod(Vector3Distance(view.pos,center)-radius,radius);lods[l]++;Geometry *g=&globe[l];float angle=(float)simtime*.07f,co=cosf(angle),si=sinf(angle);
 for(int i=0;i<g->count;i+=3){Vertex v[3];for(int j=0;j<3;j++){v[j]=g->v[i+j];Vector3 p=v[j].p;if(kind==4){
   Vector3 radial=Vector3Normalize((Vector3){center.x,0,center.z});
   Vector3 tangent={-radial.z,0,radial.x};
   Vector3 front=Vector3Normalize(Vector3Add(Vector3Add(Vector3Scale(radial,-.6f),Vector3Scale(tangent,.8f)),(Vector3){0,.25f,0}));
   Vector3 right=Vector3Normalize(Vector3CrossProduct((Vector3){0,1,0},front)),up=Vector3CrossProduct(front,right);
   p=Vector3Add(Vector3Scale(right,p.x),Vector3Add(Vector3Scale(up,p.y),Vector3Scale(front,p.z)));
  }else p=(Vector3){p.x*co+p.z*si,p.y,-p.x*si+p.z*co};v[j].n=p;float r=radius;if(kind==3)r*=1+.012f*sinf(p.x*19+(float)simtime)*sinf(p.z*23);v[j].p=Vector3Add(center,Vector3Scale(p,r));}triangle(v[0],v[1],v[2],kind);}
}
static void skybox(void){for(int y=0;y<HEIGHT;y++)for(int x=0;x<WIDTH;x++){
 Vector3 d=Vector3Normalize(Vector3Add(view.forward,Vector3Add(Vector3Scale(view.right,(x-WIDTH*.5f)/480),Vector3Scale(view.up,(HEIGHT*.5f-y)/480))));
 int a=(int)floorf(atan2f(d.z,d.x)*1200),b=(int)floorf(asinf(d.y)*1200);unsigned int h=noise((unsigned int)a*73856093u^(unsigned int)b*19349663u);
 Color c={5,10,19,255};if(h%1600<3)c=(Color){170+(h%70),175+(h%65),190+(h%50),255};pixel(x,y,c);depth[y][x]=FLT_MAX;
}}
static void line3(Vector3 a,Vector3 b,Color col){Vertex va={.p=a},vb={.p=b};Projected p=project(va),q=project(vb);if(p.z<.1f||q.z<.1f)return;float len=fmaxf(fabsf(q.x-p.x),fabsf(q.y-p.y));if(len>5000)return;for(int i=0;i<=(int)len;i++){float t=i/fmaxf(1,len);int x=(int)(p.x+(q.x-p.x)*t),y=(int)(p.y+(q.y-p.y)*t);float z=1/((1-t)/p.z+t/q.z);if(x>=0&&x<WIDTH&&y>=0&&y<HEIGHT&&z<depth[y][x])pixel(x,y,col);}}
// CPU ray/plane intersections for a translucent ring, with depth testing against opaque geometry.
static void rings(void){Vector3 c=body_position(2,simtime),n={0,.934f,.358f};float r=bodies[2].radius;
 for(int y=0;y<HEIGHT;y++)for(int x=0;x<WIDTH;x++){
 Vector3 d=Vector3Add(view.forward,Vector3Add(Vector3Scale(view.right,(x-WIDTH*.5f)/480),Vector3Scale(view.up,(HEIGHT*.5f-y)/480)));float den=Vector3DotProduct(d,n);if(fabsf(den)<.0001f)continue;float z=Vector3DotProduct(Vector3Subtract(c,view.pos),n)/den;if(z<.1f||z>=depth[y][x])continue;
 Vector3 p=Vector3Add(view.pos,Vector3Scale(d,z));float radial=Vector3Distance(p,c)/r;if(radial<1.36f||radial>2.03f||(radial>1.61f&&radial<1.66f))continue;
 float band=.5f+.5f*sinf(radial*230);Color col={190,166,120,(unsigned char)(100+90*band)};pixel(x,y,col);
 }}
static GLuint presenter(void){const char *vs="#version 330\nout vec2 uv;void main(){vec2 p=vec2((gl_VertexID<<1)&2,gl_VertexID&2);uv=vec2(p.x,1.-p.y);gl_Position=vec4(p*2.-1.,0,1);}";const char *fs="#version 330\nin vec2 uv;uniform sampler2D frame;out vec4 color;void main(){color=texture(frame,uv);}";
 GLuint v=glCreateShader(GL_VERTEX_SHADER),f=glCreateShader(GL_FRAGMENT_SHADER);glShaderSource(v,1,&vs,NULL);glCompileShader(v);glShaderSource(f,1,&fs,NULL);glCompileShader(f);GLuint p=glCreateProgram();glAttachShader(p,v);glAttachShader(p,f);glLinkProgram(p);GLint ok;glGetProgramiv(p,GL_LINK_STATUS,&ok);if(!ok){char log[1024];glGetProgramInfoLog(p,1024,NULL,log);fprintf(stderr,"Framebuffer presenter failed: %s\n",log);exit(2);}glDeleteShader(v);glDeleteShader(f);return p;}
static void capture(const char *path){FILE *f=fopen(path,"wb");if(!f)return;fprintf(f,"P6\n%d %d\n255\n",WIDTH,HEIGHT);for(int y=0;y<HEIGHT;y++)for(int x=0;x<WIDTH;x++)fwrite(pixels[y][x],1,3,f);fclose(f);}
int main(int argc,char **argv){
 bool headless=false,smoke=false,paused=false,orbits=true,help=false;int limit=0,selected=1;const char *output=NULL;bool near=false,check_display=false;
 for(int i=1;i<argc;i++){if(!strcmp(argv[i],"--check-display"))check_display=true;else if(!strcmp(argv[i],"--headless"))headless=true;else if(!strcmp(argv[i],"--smoke")){smoke=true;limit=240;}else if(!strcmp(argv[i],"--frames")&&i+1<argc)limit=atoi(argv[++i]);else if(!strcmp(argv[i],"--capture")&&i+1<argc)output=argv[++i];else if(!strcmp(argv[i],"--near"))near=true;else if(!strcmp(argv[i],"--body")&&i+1<argc)selected=(int)Clamp(atoi(argv[++i]),0,4);}
 if(headless&&!limit)limit=1;
 // Check X authorization BEFORE InitWindow: Raylib 5.5 can dereference GLFW state on initialization failure.
 if(!headless){Display *display=XOpenDisplay(NULL);if(!display){fprintf(stderr,"No se pudo abrir X11. Ejecuta desde una terminal de tu escritorio, sin sudo.\nComprueba DISPLAY y XAUTHORITY. Para pruebas sin escritorio: make headless.\n");return 1;}XCloseDisplay(display);if(check_display)return 0;InitWindow(1280,800,"HELIOGRAPH / CPU SOFTWARE RENDERER");if(!IsWindowReady())return 1;}
 GLuint program=0,tex=0,vao=0;if(!headless){program=presenter();glGenVertexArrays(1,&vao);glBindVertexArray(vao);glGenTextures(1,&tex);glBindTexture(GL_TEXTURE_2D,tex);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,WIDTH,HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);}
 make_textures();make_spheres();if(!obj_read("assets/manta.obj",&hull)){fprintf(stderr,"No se pudo cargar assets/manta.obj. Ejecuta desde la raiz del proyecto.\n");return 2;}
 view.pos=near?arrival_point(selected,0):(Vector3){182,110,213};Vector3 look=Vector3Normalize(Vector3Subtract(near?body_position(selected,0):Vector3Zero(),view.pos));float yaw=atan2f(look.x,-look.z),pitch=asinf(look.y),travel=-1;Vector3 departure={0},arrival={0};double prev=0;int frame=0;double total=0;
 while((headless||!WindowShouldClose())&&(!limit||frame<limit)){
  struct timespec begin,end;clock_gettime(CLOCK_MONOTONIC,&begin);double now=begin.tv_sec+begin.tv_nsec/1e9;float dt=smoke?1/60.f:(float)Clamp(prev?now-prev:1/60.,.001,.05);prev=now;
  if(!headless){PollInputEvents();for(int i=0;i<5;i++)if(travel<0&&IsKeyPressed(KEY_ONE+i))selected=i;if(IsKeyPressed(KEY_P))paused=!paused;if(IsKeyPressed(KEY_O))orbits=!orbits;if(IsKeyPressed(KEY_H))help=!help;
   if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){Vector2 m=GetMouseDelta();yaw+=m.x*.003f;pitch-=m.y*.003f;}yaw+=(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT))*dt;pitch=Clamp(pitch+(IsKeyDown(KEY_UP)-IsKeyDown(KEY_DOWN))*dt,-1.48f,1.48f);
  }
  if(travel<0&&((smoke&&frame==20)||(!headless&&IsKeyPressed(KEY_ENTER)))){travel=0;departure=view.pos;arrival=arrival_point(selected,simtime);}
  view.forward=(Vector3){sinf(yaw)*cosf(pitch),sinf(pitch),-cosf(yaw)*cosf(pitch)};view.right=Vector3Normalize(Vector3CrossProduct(view.forward,(Vector3){0,1,0}));view.up=Vector3CrossProduct(view.right,view.forward);
  Vector3 wanted=view.pos;double old=simtime;
  if(travel>=0){travel=fminf(1,travel+dt/3.6f);wanted=travel_point(departure,arrival,travel);look=Vector3Normalize(Vector3Subtract(body_position(selected,simtime),wanted));yaw=atan2f(look.x,-look.z);pitch=asinf(look.y);if(travel>=1)travel=-1;}
  else{if(!paused)simtime+=dt;if(!headless){Vector3 direction=Vector3Add(Vector3Scale(view.forward,IsKeyDown(KEY_W)-IsKeyDown(KEY_S)),Vector3Scale(view.right,IsKeyDown(KEY_D)-IsKeyDown(KEY_A)));direction.y+=IsKeyDown(KEY_SPACE)-IsKeyDown(KEY_LEFT_CONTROL);wanted=Vector3Add(view.pos,Vector3Scale(Vector3Normalize(direction),dt*(IsKeyDown(KEY_LEFT_SHIFT)?58:18)));}}
  int blocked;view.pos=safe_move(view.pos,wanted,old,simtime,&blocked);
  culled=triangles=0;memset(lods,0,sizeof(lods));skybox();sphere(5,Vector3Zero(),SUN_RADIUS);for(int i=0;i<5;i++)sphere(i,body_position(i,simtime),bodies[i].radius);rings();
  if(orbits)for(int i=0;i<5;i++)for(int j=0;j<128;j++){Vector3 a=body_position(i,(j*2*PI/128-bodies[i].phase)/bodies[i].speed),b=body_position(i,((j+1)*2*PI/128-bodies[i].phase)/bodies[i].speed);Color c=bodies[i].color;c.a=65;line3(a,b,c);}
  Vector3 anchor=Vector3Add(view.pos,Vector3Add(Vector3Scale(view.forward,3),Vector3Scale(view.up,-.95f)));
  for(int i=0;i<hull.count;i+=3){Vertex v[3];Vector3 n=Vector3Normalize(Vector3CrossProduct(Vector3Subtract(hull.vertices[i+1],hull.vertices[i]),Vector3Subtract(hull.vertices[i+2],hull.vertices[i])));Vector3 normal=Vector3Add(Vector3Scale(view.right,n.x),Vector3Add(Vector3Scale(view.up,n.y),Vector3Scale(view.forward,-n.z)));
   for(int j=0;j<3;j++){Vector3 p=hull.vertices[i+j];v[j]=(Vertex){Vector3Add(anchor,Vector3Scale(Vector3Add(Vector3Scale(view.right,p.x),Vector3Add(Vector3Scale(view.up,p.y),Vector3Scale(view.forward,-p.z))),.3f)),normal,{0,0}};}triangle(v[0],v[1],v[2],6);}
  Color mint={174,238,203,255},white={226,237,232,255};rect(0,0,WIDTH,60,(Color){7,15,24,235});text(20,15,"HELIOGRAPH",3,white);text(22,44,"EXPEDICION 07 / RASTERIZACION CPU",1,mint);
  rect(620,80,175,230,(Color){7,15,24,235});text(633,94,"NAVEGACION",1,mint);for(int i=0;i<5;i++){if(i==selected)rect(627,119+i*29,160,23,(Color){30,60,64,255});char line[48];snprintf(line,sizeof(line),"%d %s",i+1,bodies[i].name);text(635,127+i*29,line,1,bodies[i].color);}text(632,283,"ENTER / SALTAR",1,mint);
  char stats[128];snprintf(stats,sizeof(stats),"TRI %d / CULL %d / LOD %d %d %d",triangles,culled,lods[0],lods[1],lods[2]);text(20,HEIGHT-42,stats,1,mint);text(20,HEIGHT-24,"WASD / ESPACIO CTRL / SHIFT / MOUSE DERECHO / H AYUDA",1,white);
  if(blocked)text(240,70,"PROXIMIDAD / CASCO PROTEGIDO",1,(Color){255,160,70,255});
  if(travel>=0){rect(200,72,360,22,(Color){7,15,24,235});text(212,78,"CORREDOR DE TRANSITO",1,mint);rect(200,98,(int)(360*travel),3,mint);for(int i=0;i<40;i++){float a=i*2.3999f,r=fmodf(i*31+travel*650,420);int x=400+cosf(a)*r,y=250+sinf(a)*r;for(int j=0;j<12;j++)pixel(x+(int)(cosf(a)*j),y+(int)(sinf(a)*j),(Color){130,230,210,180});}}
  if(help){rect(105,100,490,260,(Color){7,15,24,250});const char *lines[]={"PROTOCOLO DE EXPLORACION","WASD - MOVER / ESPACIO CTRL - ALTURA","SHIFT - IMPULSO / FLECHAS - MIRAR","MANTENER MOUSE DERECHO - MIRAR","1 A 5 - SELECCIONAR / ENTER - VIAJAR","P - PAUSA / O - ORBITAS / H - AYUDA","ESC - SALIR"};for(int i=0;i<7;i++)text(125,120+i*29,lines[i],1,i?white:mint);}
  if(!headless){glViewport(0,0,GetScreenWidth(),GetScreenHeight());glUseProgram(program);glBindVertexArray(vao);glBindTexture(GL_TEXTURE_2D,tex);glTexSubImage2D(GL_TEXTURE_2D,0,0,0,WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,pixels);glDrawArrays(GL_TRIANGLES,0,3);SwapScreenBuffer();}
  clock_gettime(CLOCK_MONOTONIC,&end);total+=(end.tv_sec-begin.tv_sec)+(end.tv_nsec-begin.tv_nsec)/1e9;frame++;
 }
 if(output){capture(output);}
 printf("CPU frames=%d mean=%.2fms triangles=%d culled=%d\n",frame,total/fmax(1,frame)*1000,triangles,culled);
 for(int i=0;i<3;i++){free(globe[i].v);}
 obj_free(&hull);if(!headless){glDeleteTextures(1,&tex);glDeleteVertexArrays(1,&vao);glDeleteProgram(program);CloseWindow();}return smoke&&(travel>=0||Vector3Distance(view.pos,arrival)>3)?3:0;
}
