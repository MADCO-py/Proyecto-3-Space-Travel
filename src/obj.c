/* Generated with OpenAI Codex. OBJ parser and ear-clipping triangulation, no copied loader. */
#include "obj.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
static int emit(ObjData *o,Vector3 a,Vector3 b,Vector3 c) {
 if(o->count+3>o->capacity) {
  int cap=o->capacity?o->capacity*2:384;
  Vector3 *p=realloc(o->vertices,(size_t)cap*sizeof(*p)); if(!p)return 0;
  o->vertices=p; o->capacity=cap;
 }
 o->vertices[o->count++]=a;o->vertices[o->count++]=b;o->vertices[o->count++]=c;o->triangles++;return 1;
}
static float cross2(Vector2 a,Vector2 b,Vector2 c){return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);}
static int polygon(ObjData *o,Vector3 *p,int n) {
 Vector3 normal={0};
 for(int i=0;i<n;i++){Vector3 a=p[i],b=p[(i+1)%n];normal.x+=(a.y-b.y)*(a.z+b.z);normal.y+=(a.z-b.z)*(a.x+b.x);normal.z+=(a.x-b.x)*(a.y+b.y);}
 int axis=fabsf(normal.x)>fabsf(normal.y)?0:1;if(fabsf(normal.z)>(axis==0?fabsf(normal.x):fabsf(normal.y)))axis=2;
 Vector2 q[128];int idx[128];float area=0;
 for(int i=0;i<n;i++){q[i]=axis==0?(Vector2){p[i].y,p[i].z}:axis==1?(Vector2){p[i].x,p[i].z}:(Vector2){p[i].x,p[i].y};idx[i]=i;}
 for(int i=0;i<n;i++)area+=q[i].x*q[(i+1)%n].y-q[(i+1)%n].x*q[i].y;
 if(fabsf(area)<1e-8f)return 0;
 float sign=area>0?1:-1;int left=n;
 while(left>3){int found=0;
  for(int j=0;j<left;j++){
   int a=idx[(j+left-1)%left],b=idx[j],c=idx[(j+1)%left];
   if(sign*cross2(q[a],q[b],q[c])<=1e-7f)continue;
   int occupied=0;
   for(int k=0;k<left;k++){int v=idx[k];if(v==a||v==b||v==c)continue;
    if(sign*cross2(q[a],q[b],q[v])>=-1e-7f&&sign*cross2(q[b],q[c],q[v])>=-1e-7f&&sign*cross2(q[c],q[a],q[v])>=-1e-7f){occupied=1;break;}}
   if(occupied)continue;
   if(!emit(o,p[a],p[b],p[c]))return 0;
   memmove(&idx[j],&idx[j+1],(size_t)(left-j-1)*sizeof(int));left--;found=1;break;
  }
  if(!found)return 0;
 }
 if(!emit(o,p[idx[0]],p[idx[1]],p[idx[2]]))return 0;
 o->polygons++;o->quads+=n==4;o->ngons+=n>4;return 1;
}
int obj_read(const char *path,ObjData *o){
 memset(o,0,sizeof(*o));FILE *f=fopen(path,"r");if(!f)return 0;
 Vector3 *positions=NULL;int count=0,cap=0,ok=1;char line[8192];
 while(ok&&fgets(line,sizeof(line),f)){
  if(!strchr(line,'\n')&&!feof(f)){ok=0;break;}
  if(line[0]=='v'&&line[1]==' '){
   Vector3 v;if(sscanf(line+2,"%f %f %f",&v.x,&v.y,&v.z)!=3||!isfinite(v.x)||!isfinite(v.y)||!isfinite(v.z)){ok=0;break;}
   if(count==cap){cap=cap?cap*2:128;Vector3 *p=realloc(positions,(size_t)cap*sizeof(*p));if(!p){ok=0;break;}positions=p;}positions[count++]=v;
  }else if(line[0]=='f'&&line[1]==' '){
   Vector3 poly[128];int n=0;char *p=line+2;
   while(*p){while(*p==' '||*p=='\t'||*p=='\r'||*p=='\n')p++;if(!*p||*p=='#')break;
    char *end;long index=strtol(p,&end,10);if(end==p||n==128||index==0){ok=0;break;}
    long resolved=index<0?count+index:index-1;if(resolved<0||resolved>=count){ok=0;break;}
    poly[n++]=positions[resolved];p=end;while(*p&&*p!=' '&&*p!='\t'&&*p!='\r'&&*p!='\n')p++;
   }
   if(ok&&(n<3||!polygon(o,poly,n)))ok=0;
  }
 }
 fclose(f);free(positions);if(!ok||o->count==0){obj_free(o);return 0;}return 1;
}
void obj_free(ObjData *o){free(o->vertices);memset(o,0,sizeof(*o));}
Model obj_model(const ObjData *o){
 Mesh mesh={0};mesh.vertexCount=o->count;mesh.triangleCount=o->count/3;
 mesh.vertices=MemAlloc((unsigned int)o->count*3*sizeof(float));mesh.normals=MemAlloc((unsigned int)o->count*3*sizeof(float));mesh.texcoords=MemAlloc((unsigned int)o->count*2*sizeof(float));
 for(int i=0;i<o->count;i+=3){Vector3 n=Vector3Normalize(Vector3CrossProduct(Vector3Subtract(o->vertices[i+1],o->vertices[i]),Vector3Subtract(o->vertices[i+2],o->vertices[i])));
  for(int k=0;k<3;k++){int j=i+k;memcpy(mesh.vertices+j*3,&o->vertices[j],3*sizeof(float));memcpy(mesh.normals+j*3,&n,3*sizeof(float));mesh.texcoords[j*2]=mesh.texcoords[j*2+1]=0;}}
 UploadMesh(&mesh,false);return LoadModelFromMesh(mesh);
}
