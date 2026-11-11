/* Generated with OpenAI Codex. */
#ifndef OBJ_H
#define OBJ_H
#include "raylib.h"
typedef struct { Vector3 *vertices; int count, capacity, polygons, triangles, quads, ngons; } ObjData;
// Triangulates planar simple polygons (including concave n-gons) using ear clipping.
int obj_read(const char *path, ObjData *out);
void obj_free(ObjData *obj);

#endif
