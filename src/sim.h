/* Generated with OpenAI Codex, November 2026. Review/attribution: README.md. */
#ifndef SIM_H
#define SIM_H
#include "raylib.h"
#define BODY_COUNT 5
#define SUN_RADIUS 9.0f
#define HULL_RADIUS 4.0f
typedef struct { const char *name, *type, *description; float radius, orbit, speed, phase, tilt; Color color; } Body;
extern const Body bodies[BODY_COUNT];
Vector3 body_position(int index, double time);
int choose_lod(float surface_distance, float radius);
int sweep_sphere(Vector3 from, Vector3 to, Vector3 center, float radius, float *hit);
Vector3 safe_move(Vector3 from, Vector3 to, double old_time, double new_time, int *blocked);
Vector3 travel_point(Vector3 from, Vector3 to, float t);
Vector3 arrival_point(int index, double arrival_time);
#endif
