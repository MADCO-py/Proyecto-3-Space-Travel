/* Generated with OpenAI Codex. Behavioral checks, no display required. */
#include "sim.h"
#include "obj.h"
#include "raymath.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
static void collision_tests(void){
 float hit=0;
 assert(sweep_sphere((Vector3){-100,0,0},(Vector3){100,0,0},Vector3Zero(),5,&hit));
 assert(fabsf(hit-.475f)<.0001f); // High speed cannot tunnel through a planet.
 assert(!sweep_sphere((Vector3){-100,6,0},(Vector3){100,6,0},Vector3Zero(),5,&hit));
 assert(!sweep_sphere((Vector3){10,0,0},(Vector3){20,0,0},Vector3Zero(),5,&hit));
 assert(sweep_sphere(Vector3Zero(),Vector3Zero(),Vector3Zero(),5,&hit));
 int blocked;Vector3 p=safe_move((Vector3){-60,0,0},(Vector3){60,0,0},0,0,&blocked);
 assert(blocked&&Vector3Length(p)>SUN_RADIUS+HULL_RADIUS);
 // A moving exclusion volume cannot consume a stationary camera.
 Vector3 center=body_position(1,0);p=safe_move(center,center,0,.05,&blocked);
 assert(blocked&&Vector3Distance(p,body_position(1,.05))>bodies[1].radius+HULL_RADIUS);
}
static void travel_tests(void){
 for(int i=0;i<BODY_COUNT;i++)for(int j=0;j<BODY_COUNT;j++){
  double time=17.0;Vector3 from=arrival_point(i,time),to=arrival_point(j,time),p=from;
  assert(Vector3Distance(travel_point(from,to,0),from)<.001f);
  assert(Vector3Distance(travel_point(from,to,1),to)<.001f);
  for(int k=1;k<=432;k++){
   int blocked;p=safe_move(p,travel_point(from,to,k/432.f),time,time,&blocked);
   assert(Vector3Length(p)>=SUN_RADIUS+HULL_RADIUS);
   for(int b=0;b<BODY_COUNT;b++)assert(Vector3Distance(p,body_position(b,time))>=bodies[b].radius+HULL_RADIUS);
  }
  assert(Vector3Distance(p,to)<.001f);
 }
 // Orbital distance invariant and LOD boundary transitions.
 for(int i=0;i<BODY_COUNT;i++)for(int t=0;t<500;t++)assert(fabsf(Vector3Length(body_position(i,t))-bodies[i].orbit)<.001f);
 assert(choose_lod(1,5)==0);assert(choose_lod(30,5)==1);assert(choose_lod(100,5)==2);
}
static void fixture(const char *s){FILE *f=fopen("build/fixture.obj","w");assert(f);fputs(s,f);fclose(f);}
static void obj_tests(void){
 ObjData o;assert(obj_read("assets/manta.obj",&o));assert(o.quads>0&&o.ngons>0&&o.triangles>o.polygons);obj_free(&o);
 // Concave L-shaped polygon with negative indices: area 3, not a triangle fan crossing its notch.
 fixture("v 0 0 0\nv 2 0 0\nv 2 1 0\nv 1 1 0\nv 1 2 0\nv 0 2 0\nf -6 -5 -4 -3 -2 -1\n");
 assert(obj_read("build/fixture.obj",&o));assert(o.triangles==4);float area=0;
 for(int i=0;i<o.count;i+=3){Vector3 a=o.vertices[i],b=o.vertices[i+1],c=o.vertices[i+2];area+=Vector3Length(Vector3CrossProduct(Vector3Subtract(b,a),Vector3Subtract(c,a)))*.5f;}
 assert(fabsf(area-3)<.0001f);obj_free(&o);
 fixture("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 99\n");assert(!obj_read("build/fixture.obj",&o));
 fixture("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1/1/1 2/2/1 3/3/1\n");assert(obj_read("build/fixture.obj",&o));assert(o.triangles==1);obj_free(&o);
 remove("build/fixture.obj");
}
int main(void){collision_tests();travel_tests();obj_tests();puts("PASS: swept collisions, orbital invariants, 25 routes, 3 LODs, OBJ quads/concave/invalid indices.");return 0;}
