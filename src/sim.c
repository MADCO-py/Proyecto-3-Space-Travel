/* Generated with OpenAI Codex. Orbital model and swept collision implementation. */
#include "sim.h"
#include "raymath.h"
#include <math.h>
const Body bodies[BODY_COUNT] = {
 {"CINDER", "01 / MUNDO DE CENIZA", "Fracturas termicas. La superficie nunca duerme.", 3.2f, 27, .13f, .35f, .06f, {239,111,62,255}},
 {"PELAGOS", "02 / ARCHIVO OCEANICO", "Un oceano global bajo nubes de cobre.", 5.1f, 48, .079f, 1.5f, -.10f, {80,199,209,255}},
 {"VESPER", "03 / GIGANTE ANILLADO", "Tormentas de amoniaco y un anillo de hielo.", 7.3f, 77, .049f, 2.8f, .16f, {222,180,117,255}},
 {"NIVAL", "04 / MUNDO DE CRISTAL", "Grietas azules recorren un desierto congelado.", 4.3f, 109, .032f, 4.2f, -.12f, {150,201,245,255}},
 {"OCULUS", "05 / OJO HUMANO", "Esclerotica, iris y pupila sobre una orbita.", 5.8f, 145, .021f, 5.5f, .09f, {190,160,115,255}}
};
Vector3 body_position(int i, double time) {
 const Body *b=&bodies[i]; float a=b->phase+(float)time*b->speed;
 return (Vector3){cosf(a)*b->orbit, sinf(a)*b->orbit*sinf(b->tilt), sinf(a)*b->orbit*cosf(b->tilt)};
}
int choose_lod(float d, float r) { return d<r*5 ? 0 : d<r*18 ? 1 : 2; }
int sweep_sphere(Vector3 from, Vector3 to, Vector3 center, float radius, float *hit) {
 Vector3 d=Vector3Subtract(to,from), m=Vector3Subtract(from,center);
 float c=Vector3DotProduct(m,m)-radius*radius;
 if(c<=0) { *hit=0; return 1; }
 float a=Vector3DotProduct(d,d), b=Vector3DotProduct(m,d), disc=b*b-a*c;
 if(a<1e-12f || b>=0 || disc<0) return 0;
 float t=(-b-sqrtf(disc))/a;
 if(t<0 || t>1) return 0;
 *hit=t; return 1;
}
Vector3 safe_move(Vector3 from, Vector3 to, double old_time, double new_time, int *blocked) {
 *blocked=0;
 // Relative motion catches fast ships AND moving planets; short simulation steps bound arc error.
 for(int pass=0;pass<3;pass++) {
  float first=2; int object=-2;
  for(int i=-1;i<BODY_COUNT;i++) {
   Vector3 c0=i<0?Vector3Zero():body_position(i,old_time);
   Vector3 c1=i<0?Vector3Zero():body_position(i,new_time);
   float r=(i<0?SUN_RADIUS:bodies[i].radius)+HULL_RADIUS+.15f, t;
   if(Vector3Distance(from,c0)<r) {
    Vector3 n=Vector3Normalize(Vector3Subtract(from,c0)); if(Vector3Length(n)<.1f) n=(Vector3){0,1,0};
    from=Vector3Add(c0,Vector3Scale(n,r+.02f));
   }
   if(sweep_sphere(Vector3Subtract(from,c0),Vector3Subtract(to,c1),Vector3Zero(),r,&t) && t<first) { first=t; object=i; }
  }
  if(object==-2) break;
  *blocked=1;
  to=Vector3Lerp(from,to,fmaxf(0,first-.002f));
 }
 // Final projection also handles an orbiting body carrying its exclusion volume into a stationary ship.
 for(int i=-1;i<BODY_COUNT;i++) {
  Vector3 c=i<0?Vector3Zero():body_position(i,new_time);
  float r=(i<0?SUN_RADIUS:bodies[i].radius)+HULL_RADIUS+.17f;
  if(Vector3Distance(to,c)<r) { Vector3 n=Vector3Normalize(Vector3Subtract(to,c)); if(Vector3Length(n)<.1f)n=(Vector3){0,1,0}; to=Vector3Add(c,Vector3Scale(n,r)); *blocked=1; }
 }
 return to;
}
Vector3 travel_point(Vector3 from, Vector3 to, float t) {
 t=Clamp(t,0,1); float s=t*t*(3-2*t);
 Vector3 p=Vector3Lerp(from,to,s);
 // Route over the orbital plane, avoiding the central star during cross-system transits.
 p.y += sinf(PI*s)*80;
 return p;
}
Vector3 arrival_point(int i,double t) {
 Vector3 p=body_position(i,t), outward=Vector3Normalize(p);
 Vector3 tangent=Vector3Normalize((Vector3){-outward.z,0,outward.x});
 Vector3 approach=Vector3Add(Vector3Scale(outward,-.6f),Vector3Scale(tangent,.8f));
 return Vector3Add(p,Vector3Add(Vector3Scale(approach,bodies[i].radius*3.2f+7),(Vector3){0,bodies[i].radius*1.1f,0}));
}
