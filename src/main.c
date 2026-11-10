/* Generated with OpenAI Codex, November 2026. See ATTRIBUTION.md.
   This implementation intentionally uses Raylib 3D/OpenGL, as requested by the user.
   It is NOT a software rasterizer. */
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "sim.h"
#include "obj.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 1440
#define H 900
#define TRAVEL_SECONDS 3.6f
static const Color INK={9,17,27,235}, MUTED={126,151,163,255}, PAPER={226,237,232,255}, MINT={174,238,203,255};
typedef struct { Shader program; int time,eye,kind; } Surface;
typedef struct {
 Camera3D camera; Vector3 position,velocity; float yaw,pitch; double time; int selected;
 bool paused,help,orbits,travelling,mouse; float travel,collisionFlash; Vector3 departure,arrival;
 int lod[3],frames; float speed; bool smoke; const char *capture;
} State;
static Font font;
static Surface surfaces[7];
static Model spheres[3],sky,ship,ring;
static Texture2D textures[6],ringTexture,glow;
static Shader skyShader,ringShader;
static int ringCenter;
static int skyEye;
static unsigned int rng=37097;
static float random01(void){rng=rng*1664525u+1013904223u;return (float)(rng>>8)/16777216.0f;}
static float wave(float u,float v){return sinf(u*37+sinf(v*21)*1.8f)*sinf(v*47+cosf(u*13));}
static Texture2D planet_texture(int kind){
 const int width=768,height=384;Image image=GenImageColor(width,height,BLACK);Color *pixels=image.data;
 for(int y=0;y<height;y++)for(int x=0;x<width;x++){
  float u=(float)x/width*2*PI,v=(float)y/height*PI,n=wave(u,v),grain=random01()*.045f;
  Vector3 a={0},b={0};float f=0;
  switch(kind){
   case 0:a=(Vector3){.09,.065,.07};b=(Vector3){.46,.21,.11};f=Clamp(.45+n*.35f+grain,0,1);break;
   case 1:a=(Vector3){.025,.13,.20};b=(Vector3){.14,.48,.42};f=Clamp((n-.05f)*2,0,1);break;
   case 2:a=(Vector3){.35,.19,.15};b=(Vector3){.86,.69,.44};f=.5f+.38f*sinf(v*36+sin(u*3)*.8f)+grain;break;
   case 3:a=(Vector3){.18,.35,.51};b=(Vector3){.77,.89,.93};f=Clamp(.65f+n*.3f+grain,0,1);break;
   case 4:a=(Vector3){.09,.055,.17};b=(Vector3){.17,.36,.24};f=Clamp(.4f+n*.65f+grain,0,1);break;
   default:a=(Vector3){.9,.23,.035};b=(Vector3){1,.78,.27};f=Clamp(.6f+n*.3f+grain,0,1);break;
  }
  Vector3 c=Vector3Lerp(a,b,f);pixels[y*width+x]=(Color){(unsigned char)(c.x*255),(unsigned char)(c.y*255),(unsigned char)(c.z*255),255};
 }
 Texture2D t=LoadTextureFromImage(image);UnloadImage(image);GenTextureMipmaps(&t);SetTextureFilter(t,TEXTURE_FILTER_TRILINEAR);SetTextureWrap(t,TEXTURE_WRAP_REPEAT);return t;
}
static Surface surface_load(int kind){
 char *source=LoadFileText("assets/shaders/world.fs");char *vs=LoadFileText("assets/shaders/world.vs");
 if(!source||!vs){fprintf(stderr,"Missing shaders. Run from the repository root.\n");exit(2);}
 char *body=strchr(source,'\n'); // Comments precede #version; locate version explicitly.
 char *version=strstr(source,"#version 330");if(!version)exit(2);body=strchr(version,'\n');
 size_t size=strlen(body)+96;char *fs=malloc(size);
 snprintf(fs,size,"#version 330\n#define BODY_KIND %d\n%s",kind,body+1);
 Surface s={0};s.program=LoadShaderFromMemory(vs,fs);free(fs);UnloadFileText(source);UnloadFileText(vs);
 if(s.program.id==rlGetShaderIdDefault()){fprintf(stderr,"Surface shader %d failed\n",kind);exit(2);}
 s.time=GetShaderLocation(s.program,"time");s.eye=GetShaderLocation(s.program,"eye");s.kind=GetShaderLocation(s.program,"kind");
 SetShaderValue(s.program,s.kind,&kind,SHADER_UNIFORM_INT);return s;
}
static Model make_ring(void){
 const int segments=192;Mesh mesh={0};mesh.vertexCount=segments*6;mesh.triangleCount=segments*2;
 mesh.vertices=MemAlloc((unsigned int)mesh.vertexCount*3*sizeof(float));mesh.normals=MemAlloc((unsigned int)mesh.vertexCount*3*sizeof(float));mesh.texcoords=MemAlloc((unsigned int)mesh.vertexCount*2*sizeof(float));
 int corner[6]={0,2,1,1,2,3};
 for(int i=0;i<segments;i++)for(int k=0;k<6;k++){
  int c=corner[k],v=i*6+k;float a=(i+(c/2))*2*PI/segments,r=c%2?2.03f:1.36f;
  mesh.vertices[v*3]=cosf(a)*r;mesh.vertices[v*3+1]=0;mesh.vertices[v*3+2]=sinf(a)*r;
  mesh.normals[v*3]=0;mesh.normals[v*3+1]=1;mesh.normals[v*3+2]=0;
  mesh.texcoords[v*2]=(float)(c%2);mesh.texcoords[v*2+1]=(float)(i+c/2)/segments;
 }
 UploadMesh(&mesh,false);return LoadModelFromMesh(mesh);
}
static void load_assets(void){
 font=LoadFontEx("assets/fonts/Interface.ttf",64,NULL,0);SetTextureFilter(font.texture,TEXTURE_FILTER_BILINEAR);
 for(int i=0;i<6;i++){textures[i]=planet_texture(i);surfaces[i]=surface_load(i);}
 surfaces[6]=surface_load(6);
 const int sizes[3]={80,36,14};for(int i=0;i<3;i++)spheres[i]=LoadModelFromMesh(GenMeshSphere(1,sizes[i],sizes[i]));
 sky=LoadModelFromMesh(GenMeshSphere(1,24,32));skyShader=LoadShader("assets/shaders/world.vs","assets/shaders/sky.fs");
 if(skyShader.id==rlGetShaderIdDefault()){fprintf(stderr,"Sky shader failed\n");exit(2);}
 skyEye=GetShaderLocation(skyShader,"eye");sky.materials[0].shader=skyShader;
 ObjData obj;if(!obj_read("assets/manta.obj",&obj)){fprintf(stderr,"Invalid/missing MANTA OBJ\n");exit(2);}ship=obj_model(&obj);
 TraceLog(LOG_INFO,"MANTA: %d polygons (%d quads, %d ngons) -> %d triangles",obj.polygons,obj.quads,obj.ngons,obj.triangles);obj_free(&obj);
 ship.materials[0].shader=surfaces[6].program;
 ring=make_ring();ringShader=LoadShader("assets/shaders/world.vs","assets/shaders/ring.fs");
 if(ringShader.id==rlGetShaderIdDefault()){fprintf(stderr,"Ring shader failed\n");exit(2);}
 ring.materials[0].shader=ringShader;ringCenter=GetShaderLocation(ringShader,"center");
 float radius=bodies[2].radius;SetShaderValue(ringShader,GetShaderLocation(ringShader,"radius"),&radius,SHADER_UNIFORM_FLOAT);
 Image im=GenImageColor(256,4,BLANK);Color *p=im.data;
 for(int y=0;y<4;y++)for(int x=0;x<256;x++){float t=x/255.f;float bands=.6f+.4f*sinf(t*170);float gap=(t>.38f&&t<.45f)?0:1;p[y*256+x]=(Color){200,177,136,(unsigned char)(160*bands*gap)};}
 ringTexture=LoadTextureFromImage(im);UnloadImage(im);ring.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=ringTexture;
 im=GenImageColor(128,128,BLANK);p=im.data;
 for(int y=0;y<128;y++)for(int x=0;x<128;x++){float r=sqrtf((x-63.5f)*(x-63.5f)+(y-63.5f)*(y-63.5f))/63.5f;float a=powf(fmaxf(0,1-r),3);p[y*128+x]=(Color){255,140,55,(unsigned char)(a*220)};}
 glow=LoadTextureFromImage(im);UnloadImage(im);
}
static void unload_assets(void){
 for(int i=0;i<3;i++) { UnloadModel(spheres[i]); }
 UnloadModel(sky);UnloadModel(ship);UnloadModel(ring);
 for(int i=0;i<6;i++) { UnloadTexture(textures[i]); }
 UnloadTexture(ringTexture);UnloadTexture(glow);
 for(int i=0;i<7;i++) { UnloadShader(surfaces[i].program); }
 UnloadShader(skyShader);UnloadShader(ringShader);UnloadFont(font);
}
static Vector3 forward_of(const State *s){return (Vector3){sinf(s->yaw)*cosf(s->pitch),sinf(s->pitch),-cosf(s->yaw)*cosf(s->pitch)};}
static void look_at(State *s,Vector3 target){Vector3 d=Vector3Normalize(Vector3Subtract(target,s->position));s->pitch=asinf(Clamp(d.y,-1,1));s->yaw=atan2f(d.x,-d.z);}
static void sync_camera(State *s){s->camera.position=s->position;s->camera.target=Vector3Add(s->position,forward_of(s));s->camera.up=(Vector3){0,1,0};s->camera.fovy=55+(s->travelling?12*sinf(PI*s->travel):0);s->camera.projection=CAMERA_PERSPECTIVE;}
static void start_travel(State *s){
 if(s->travelling)return;
 s->travelling=true;s->travel=0;s->departure=s->position;s->velocity=Vector3Zero();
 // Freeze orbital time for transit so the arrival point remains accurate even if pause is toggled.
 s->arrival=arrival_point(s->selected,s->time);
}
static void update(State *s,float dt){
 if(IsKeyPressed(KEY_H))s->help=!s->help;
 if(IsKeyPressed(KEY_O))s->orbits=!s->orbits;
 if(IsKeyPressed(KEY_P)&&!s->travelling)s->paused=!s->paused;
 if(IsKeyPressed(KEY_F11))ToggleFullscreen();
 if(!s->travelling)for(int i=0;i<BODY_COUNT;i++)if(IsKeyPressed(KEY_ONE+i))s->selected=i;
 if(IsKeyPressed(KEY_ENTER))start_travel(s);
 if(IsKeyPressed(KEY_HOME)&&!s->travelling){s->position=(Vector3){182,110,213};s->velocity=Vector3Zero();look_at(s,Vector3Zero());}
 if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){DisableCursor();s->mouse=true;}
 if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)){EnableCursor();s->mouse=false;}
 double old=s->time;
 if(!s->paused&&!s->travelling)s->time+=dt;
 Vector3 wanted=s->position;
 if(s->travelling){
  s->travel=fminf(1,s->travel+dt/TRAVEL_SECONDS);wanted=travel_point(s->departure,s->arrival,s->travel);
  look_at(s,body_position(s->selected,s->time));
  if(s->travel>=1){s->travelling=false;wanted=s->arrival;}
 }else{
  if(s->mouse){Vector2 m=GetMouseDelta();s->yaw+=m.x*.0025f;s->pitch-=m.y*.0025f;}
  s->yaw+=(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT))*dt*1.2f;
  s->pitch+=(IsKeyDown(KEY_UP)-IsKeyDown(KEY_DOWN))*dt*1.2f;s->pitch=Clamp(s->pitch,-1.48f,1.48f);
  Vector3 f=forward_of(s),right=Vector3Normalize(Vector3CrossProduct(f,(Vector3){0,1,0}));
  Vector3 input=Vector3Add(Vector3Scale(f,(float)(IsKeyDown(KEY_W)-IsKeyDown(KEY_S))),Vector3Scale(right,(float)(IsKeyDown(KEY_D)-IsKeyDown(KEY_A))));
  input.y+=IsKeyDown(KEY_SPACE)-IsKeyDown(KEY_LEFT_CONTROL);
  float speed=IsKeyDown(KEY_LEFT_SHIFT)?58:18;
  Vector3 target=Vector3Scale(Vector3Normalize(input),speed);
  s->velocity=Vector3Lerp(s->velocity,target,1-expf(-dt*5));wanted=Vector3Add(s->position,Vector3Scale(s->velocity,dt));
 }
 int blocked;s->position=safe_move(s->position,wanted,old,s->time,&blocked);
 if(blocked){s->collisionFlash=1;s->velocity=Vector3Zero();}
 s->collisionFlash=fmaxf(0,s->collisionFlash-dt*.8f);s->speed=Vector3Length(s->velocity);sync_camera(s);
}
static void draw_orbit(int i,double time){
 const Body *b=&bodies[i];Color col=ColorAlpha(b->color,.22f);
 for(int j=0;j<160;j++){
  float a=j*2*PI/160,aa=(j+1)*2*PI/160;
  Vector3 p={cosf(a)*b->orbit,sinf(a)*b->orbit*sinf(b->tilt),sinf(a)*b->orbit*cosf(b->tilt)};
  Vector3 q={cosf(aa)*b->orbit,sinf(aa)*b->orbit*sinf(b->tilt),sinf(aa)*b->orbit*cosf(b->tilt)};
  DrawLine3D(p,q,col);
 }
 (void)time;
}
static void sphere_draw(State *s,int kind,Vector3 position,float radius,float rotation){
 int lod=choose_lod(Vector3Distance(s->position,position)-radius,radius);s->lod[lod]++;
 Model *m=&spheres[lod];m->materials[0].shader=surfaces[kind].program;m->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=textures[kind];
 DrawModelEx(*m,position,(Vector3){0,1,0},rotation,(Vector3){radius,radius,radius},WHITE);
}
static void draw_world(State *s){
 float t=(float)s->time;
 for(int i=0;i<7;i++){SetShaderValue(surfaces[i].program,surfaces[i].time,&t,SHADER_UNIFORM_FLOAT);SetShaderValue(surfaces[i].program,surfaces[i].eye,&s->position,SHADER_UNIFORM_VEC3);}
 SetShaderValue(skyShader,skyEye,&s->position,SHADER_UNIFORM_VEC3);
 Vector3 center=body_position(2,s->time);SetShaderValue(ringShader,ringCenter,&center,SHADER_UNIFORM_VEC3);
 memset(s->lod,0,sizeof(s->lod));
 BeginMode3D(s->camera);
 rlDisableBackfaceCulling();rlDisableDepthMask();DrawModel(sky,s->position,500,WHITE);rlEnableDepthMask();rlEnableBackfaceCulling();
 // Billboard halo is additive, depth tested and does not populate the depth buffer.
 rlDisableDepthMask();BeginBlendMode(BLEND_ADDITIVE);DrawBillboard(s->camera,glow,Vector3Zero(),65,WHITE);EndBlendMode();rlEnableDepthMask();
 if(s->orbits)for(int i=0;i<BODY_COUNT;i++)draw_orbit(i,s->time);
 sphere_draw(s,5,Vector3Zero(),SUN_RADIUS,t*3);
 for(int i=0;i<BODY_COUNT;i++)sphere_draw(s,i,body_position(i,s->time),bodies[i].radius,t*(i%2?-5:7));
 // Tilted translucent ring; draw after opaque geometry. Both sides are intentionally visible.
 rlDisableBackfaceCulling();rlDisableDepthMask();
 DrawModelEx(ring,body_position(2,s->time),(Vector3){1,0,0},21,(Vector3){bodies[2].radius,bodies[2].radius,bodies[2].radius},WHITE);
 rlEnableDepthMask();rlEnableBackfaceCulling();
 // Camera-following survey ship. Its complete bounds fit inside HULL_RADIUS.
 Vector3 f=forward_of(s),right=Vector3Normalize(Vector3CrossProduct(f,(Vector3){0,1,0})),up=Vector3Normalize(Vector3CrossProduct(right,f));
 Vector3 sp=Vector3Add(s->position,Vector3Add(Vector3Scale(f,3),Vector3Scale(up,-.93f)));
 ship.transform=MatrixMultiply(MatrixRotateX(s->pitch),MatrixRotateY(-s->yaw));
 DrawModel(ship,sp,.30f,(Color){163,198,207,255});
 // Thrusters remain emissive on the night side.
 for(int j=-1;j<=1;j+=2){Vector3 engine=Vector3Add(sp,Vector3Add(Vector3Scale(right,j*.195f),Vector3Scale(f,-.53f)));
  DrawSphereEx(engine,.065f+(s->travelling?.04f:0),8,8,(Color){100,239,222,255});
  DrawLine3D(engine,Vector3Add(engine,Vector3Scale(f,-(.18f+s->speed*.012f))),MINT);
 }
 EndMode3D();
}
static void text(const char *str,float x,float y,float size,Color c){DrawTextEx(font,str,(Vector2){x,y},size,1,c);}
static void right_text(const char *str,float x,float y,float size,Color c){Vector2 m=MeasureTextEx(font,str,size,1);text(str,x-m.x,y,size,c);}
static void panel(float x,float y,float w,float h){DrawRectangleRec((Rectangle){x,y,w,h},INK);DrawRectangleLinesEx((Rectangle){x,y,w,h},1,(Color){54,76,85,180});}
static Rectangle destination_rect(int i,int width){return (Rectangle){width-292.f,224.f+i*51,260,46};}
static void ui_input(State *s){
 if(s->mouse||s->travelling)return;
 Vector2 m=GetMousePosition();
 if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
  for(int i=0;i<BODY_COUNT;i++)if(CheckCollisionPointRec(m,destination_rect(i,GetScreenWidth())))s->selected=i;
  if(CheckCollisionPointRec(m,(Rectangle){GetScreenWidth()-292.f,623,260,44}))start_travel(s);
 }
}
static void draw_labels(State *s){
 for(int i=-1;i<BODY_COUNT;i++){
  Vector3 p=i<0?Vector3Zero():body_position(i,s->time);float r=i<0?SUN_RADIUS:bodies[i].radius;
  Vector3 d=Vector3Subtract(p,s->position);if(Vector3DotProduct(d,forward_of(s))<=0)continue;
  if(Vector3Distance(s->position,p)<r*3)continue;
  Vector2 v=GetWorldToScreen(Vector3Add(p,(Vector3){0,r+1.8f,0}),s->camera);
  if(v.x<42||v.x>GetScreenWidth()-330||v.y<130||v.y>GetScreenHeight()-130)continue;
  Color c=i<0?(Color){236,183,119,255}:bodies[i].color;
  DrawCircleV((Vector2){v.x,v.y+22},2,c);DrawLineV((Vector2){v.x,v.y+22},(Vector2){v.x+12,v.y+9},ColorAlpha(c,.6f));
  text(i<0?"SOL / HELIOS":bodies[i].name,v.x+17,v.y,13,c);
 }
}
static void draw_map(State *s,int height){
 float x=48,y=height-235.f,size=142;panel(x-16,y-30,194,208);text("CARTA ORBITAL",x,y-17,11,MUTED);
 Vector2 center={x+size*.5f,y+size*.5f+10};
 for(int i=0;i<BODY_COUNT;i++){
  float radius=bodies[i].orbit/145*67;DrawCircleLinesV(center,radius,(Color){39,59,69,255});
  Vector3 p=body_position(i,s->time);Vector2 v={center.x+p.x/145*67,center.y+p.z/145*67};DrawCircleV(v,i==s->selected?4:2.5f,bodies[i].color);
 }
 DrawCircleV(center,4,(Color){255,186,96,255});
 Vector2 v={center.x+s->position.x/145*67,center.y+s->position.z/145*67};
 v.x=Clamp(v.x,x-2,x+size+2);v.y=Clamp(v.y,y+10,y+size+10);DrawCircleLinesV(v,4,MINT);
 text("TU NAVE",x,y+160,10,MINT);text("PLANO XZ",x+100,y+160,10,MUTED);
}
static void draw_ui(State *s){
 int w=GetScreenWidth(),h=GetScreenHeight();float rx=w-292.f;
 DrawRectangleGradientV(0,0,w,160,(Color){6,13,22,235},(Color){6,13,22,0});
 DrawRectangleGradientV(0,h-150,w,150,(Color){6,13,22,0},(Color){6,13,22,240});
 DrawRectangle(32,35,3,52,MINT);text("H E L I O G R A P H",48,28,30,PAPER);
 text("EXPEDICION 07   /   EL ARCHIVO DEL SILENCIO",49,69,11,MUTED);
 right_text("MANTA-07  /  TELEMETRIA EN VIVO",w-32,35,12,MINT);
 right_text(TextFormat("%02d FPS   |   LOD %d / %d / %d",GetFPS(),s->lod[0],s->lod[1],s->lod[2]),w-32,57,11,MUTED);
 DrawLine(32,102,w-32,102,(Color){54,76,85,190});
 text("SISTEMA HELIOS",48,128,13,MINT);text("05 MUNDOS  /  01 ESTRELLA",48,151,11,MUTED);
 draw_labels(s);
 panel(rx-16,126,292,559);
 text("NAVEGACION",rx,145,12,MUTED);text("Elige tu proximo mundo.",rx,173,18,PAPER);
 for(int i=0;i<BODY_COUNT;i++){
  Rectangle r=destination_rect(i,w);bool chosen=i==s->selected;
  if(chosen){DrawRectangleRec(r,(Color){31,56,59,230});DrawRectangle((int)r.x,(int)r.y,2,(int)r.height,bodies[i].color);}
  else if(CheckCollisionPointRec(GetMousePosition(),r))DrawRectangleRec(r,(Color){24,37,48,210});
  text(TextFormat("0%d",i+1),r.x+10,r.y+15,12,chosen?PAPER:MUTED);DrawCircle((int)r.x+52,(int)r.y+23,5,bodies[i].color);
  text(bodies[i].name,r.x+69,r.y+13,16,chosen?PAPER:MUTED);
  right_text(TextFormat("%.0f AU",bodies[i].orbit),r.x+r.width-10,r.y+16,10,MUTED);
 }
 const Body *b=&bodies[s->selected];
 DrawLine((int)rx,493,w-32,493,(Color){54,76,85,180});text(b->type,rx,512,11,b->color);
 // Fixed short lines maintain legibility at the minimum supported window width.
 const char *details[5][2]={{"Fracturas termicas.","La superficie nunca duerme."},{"Un oceano global.","Nubes sobre un archivo perdido."},{"Tormentas de amoniaco.","Un anillo de hielo a la deriva."},{"Un desierto congelado.","Su corteza de cristal respira."},{"Una red viva en la oscuridad.","La noche enciende sus esporas."}};
 text(details[s->selected][0],rx,540,13,PAPER);text(details[s->selected][1],rx,561,12,MUTED);
 float distance=Vector3Distance(s->position,body_position(s->selected,s->time))-b->radius;
 text(TextFormat("DISTANCIA   %.1f AU",distance),rx,592,11,MUTED);
 Rectangle button={rx,623,260,44};DrawRectangleRec(button,s->travelling?(Color){41,64,68,255}:MINT);
 text(s->travelling?"TRANSITO EN CURSO":"ENTER  /  INICIAR SALTO",rx+17,637,12,s->travelling?PAPER:INK);
 draw_map(s,h);
 // Minimal flight reticle and telemetry.
 float cx=w*.5f,cy=h*.50f;DrawLine((int)cx-7,(int)cy,(int)cx-2,(int)cy,ColorAlpha(MINT,.4f));DrawLine((int)cx+2,(int)cy,(int)cx+7,(int)cy,ColorAlpha(MINT,.4f));
 text(s->paused?"ORBITAS EN PAUSA":"VUELO LIBRE",252,h-127,11,MINT);
 text(TextFormat("%05.1f",s->speed),250,h-106,31,PAPER);text("AU / s",365,h-86,11,MUTED);
 text(TextFormat("X %+07.1f   Y %+07.1f   Z %+07.1f",s->position.x,s->position.y,s->position.z),252,h-59,11,MUTED);
 right_text("WASD MOVER   ESPACIO / CTRL ALTURA   SHIFT IMPULSO",w-32,h-59,11,MUTED);
 right_text("CLIC DERECHO MIRAR   1-5 DESTINO   H AYUDA",w-32,h-37,11,MUTED);
 if(s->collisionFlash>0){DrawRectangleLinesEx((Rectangle){3,3,w-6.f,h-6.f},3,ColorAlpha((Color){244,170,81,255},s->collisionFlash));text("PROXIMIDAD / CASCO PROTEGIDO",252,h-155,12,(Color){244,170,81,255});}
 if(s->travelling){
  float a=sinf(PI*s->travel);BeginBlendMode(BLEND_ADDITIVE);
  for(int i=0;i<65;i++){
   float angle=i*2.399963f,r=90+fmodf(i*71+s->travel*670,650);Vector2 p={w*.5f+cosf(angle)*r,h*.5f+sinf(angle)*r};
   Vector2 q={w*.5f+cosf(angle)*(r+35+100*a),h*.5f+sinf(angle)*(r+35+100*a)};DrawLineEx(p,q,1.2f,ColorAlpha(MINT,a*.6f));
  }EndBlendMode();
  panel(w*.5f-190,113,380,58);text("CORREDOR DE TRANSITO",w*.5f-165,123,13,MINT);
  DrawRectangle((int)(w*.5f-165),149,330,3,(Color){40,65,72,255});DrawRectangle((int)(w*.5f-165),149,(int)(330*s->travel),3,MINT);
 }
 if(s->help){
  float x=w*.5f-290,y=h*.5f-192;panel(x,y,580,384);text("PROTOCOLO DE EXPLORACION",x+28,y+27,21,MINT);
  const char *lines[]={"W / A / S / D        Avanzar, retroceder y desplazarse","Espacio / Ctrl        Subir / bajar en el eje Y","Shift                      Impulso sostenido","Clic derecho            Mantener para mirar con el mouse","Flechas                   Mirar con el teclado","1-5 o clic                Seleccionar planeta","Enter                      Salto animado al planeta elegido","P / O                      Pausar orbitas / mostrar trayectorias","Home / F11             Vista general / pantalla completa","H / Esc                    Cerrar ayuda / salir"};
  for(int i=0;i<10;i++)text(lines[i],x+28,y+77+i*25,13,i%2?MUTED:PAPER);
 }
}
int main(int argc,char **argv){
 State s={0};s.selected=1;s.orbits=true;s.position=(Vector3){182,110,213};s.capture=NULL;
 int limit=0;bool near=false,surface=false;
 for(int i=1;i<argc;i++){
  if(strcmp(argv[i],"--smoke")==0){s.smoke=true;limit=240;}
  else if(strcmp(argv[i],"--frames")==0&&i+1<argc)limit=atoi(argv[++i]);
  else if(strcmp(argv[i],"--capture")==0&&i+1<argc)s.capture=argv[++i];
  else if(strcmp(argv[i],"--near")==0)near=true;
  else if(strcmp(argv[i],"--surface")==0){near=true;surface=true;s.paused=true;}
  else if(strcmp(argv[i],"--body")==0&&i+1<argc)s.selected=Clamp((float)atoi(argv[++i]),0,BODY_COUNT-1);
 }
 SetConfigFlags(FLAG_MSAA_4X_HINT|FLAG_WINDOW_RESIZABLE);
 InitWindow(W,H,"HELIOGRAPH / El archivo del silencio");
 if(!IsWindowReady()){fprintf(stderr,"Unable to open an OpenGL window. Check DISPLAY.\n");return 1;}
 SetWindowMinSize(1100,760);SetTargetFPS(60);SetExitKey(KEY_ESCAPE);
 load_assets();
 if(near){
  s.position=arrival_point(s.selected,s.time);
  if(surface){Vector3 c=body_position(s.selected,s.time);s.position=Vector3Add(c,Vector3Scale(Vector3Normalize(Vector3Subtract(s.position,c)),bodies[s.selected].radius+HULL_RADIUS+.5f));}
  look_at(&s,body_position(s.selected,s.time));
 }else look_at(&s,Vector3Zero());
 sync_camera(&s);
 double total=0,maxFrame=0,warmTotal=0,warmMax=0;int warmFrames=0;
 double samples[600]={0};int sampleCount=0;
 while(!WindowShouldClose()&&(!limit||s.frames<limit)){
  double begin=GetTime();float dt=fminf(GetFrameTime(),.05f);if(s.smoke)dt=1/60.f;
  if(s.smoke){if(s.frames==20)start_travel(&s);if(s.frames==100)s.help=true;if(s.frames==110)s.help=false;}
  ui_input(&s);
  // Relative sweeps handle continuous motion. Clamp dt to bound orbital arc error.
  update(&s,dt);
  BeginDrawing();ClearBackground((Color){5,10,19,255});draw_world(&s);draw_ui(&s);EndDrawing();
  double elapsed=GetTime()-begin;total+=elapsed;if(elapsed>maxFrame)maxFrame=elapsed;
  if(s.frames>=30){warmTotal+=elapsed;if(elapsed>warmMax)warmMax=elapsed;warmFrames++;if(sampleCount<600)samples[sampleCount++]=elapsed;}
  if(s.capture&&limit&&s.frames==limit-1){Image shot=LoadImageFromScreen();ExportImage(shot,s.capture);UnloadImage(shot);}
  s.frames++;
 }
 TraceLog(LOG_INFO,"RUN: frames=%d average=%.2fms max=%.2fms collision_clearance=%.2f",s.frames,s.frames?total/s.frames*1000:0,maxFrame*1000,Vector3Distance(s.position,body_position(s.selected,s.time))-bodies[s.selected].radius);
 if(warmFrames){
  for(int i=1;i<sampleCount;i++){double v=samples[i];int j=i-1;while(j>=0&&samples[j]>v){samples[j+1]=samples[j];j--;}samples[j+1]=v;}
  TraceLog(LOG_INFO,"WARM: frames=%d average=%.2fms p95=%.2fms max=%.2fms",warmFrames,warmTotal/warmFrames*1000,samples[(sampleCount-1)*95/100]*1000,warmMax*1000);
 }
 int result=0;
 if(s.smoke && (s.travelling || Vector3Distance(s.position,s.arrival)>3)){fprintf(stderr,"SMOKE FAIL: transit did not finish at its destination\n");result=3;}
 else if(s.smoke)TraceLog(LOG_INFO,"SMOKE PASS: animated transit completed at destination");
 unload_assets();CloseWindow();return result;
}
