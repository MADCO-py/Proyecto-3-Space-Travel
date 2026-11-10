#!/usr/bin/env python3
"""Generated with OpenAI Codex. MANTA-07 geometry; NOT claimed as student-made.
Run with Python to export OBJ, or Blender --background --python to also save .blend.
Faces retain quads and concave n-gons to exercise the application's OBJ loader.
"""
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
VERTICES, FACES = [], []
def prism(outline, bottom, top):
    # Ensure clockwise in XZ, giving the top cap an outward +Y normal.
    area = sum(outline[i][0]*outline[(i+1)%len(outline)][1]-outline[(i+1)%len(outline)][0]*outline[i][1] for i in range(len(outline)))
    if area > 0: outline = list(reversed(outline))
    start, n = len(VERTICES), len(outline)
    VERTICES.extend((x, bottom, z) for x, z in outline)
    VERTICES.extend((x, top, z) for x, z in outline)
    FACES.append(tuple(start+i for i in reversed(range(n))))
    FACES.append(tuple(start+n+i for i in range(n)))
    for i in range(n):
        j=(i+1)%n
        FACES.append((start+i,start+j,start+n+j,start+n+i))
# Forked survey hull, swept wings and raised instrument cabin.
prism([(0,-2.5),(-.58,-.8),(-2.8,1.15),(-2.2,1.65),(-.65,.9),(-.48,1.8),(.48,1.8),(.65,.9),(2.2,1.65),(2.8,1.15),(.58,-.8)],-.12,.12)
prism([(-.32,-1.05),(-.42,.45),(.42,.45),(.32,-1.05)],.12,.48)
for x in [-.65,.65]:
    prism([(x-.16,.2),(x-.2,1.75),(x+.2,1.75),(x+.16,.2)],-.26,.02)
# Vertical stabilizers, simple quads and triangles.
for side in [-1,1]:
    b=len(VERTICES)
    VERTICES.extend([(side*1.8,.12,.9),(side*1.8,.72,1.55),(side*1.8,.12,1.6), (side*1.86,.12,.9),(side*1.86,.72,1.55),(side*1.86,.12,1.6)])
    FACES.extend([(b,b+1,b+2),(b+5,b+4,b+3),(b,b+3,b+4,b+1),(b+1,b+4,b+5,b+2),(b+2,b+5,b+3,b)])
assets=ROOT/'assets'
assets.mkdir(exist_ok=True)
with (assets/'manta.obj').open('w') as f:
    f.write('# Generated with OpenAI Codex; design MANTA-07. See ATTRIBUTION.md.\n')
    for v in VERTICES: f.write('v %.6f %.6f %.6f\n'%v)
    for face in FACES: f.write('f '+' '.join(str(i+1) for i in face)+'\n')
print(f'MANTA: {len(VERTICES)} vertices, {len(FACES)} polygon faces')
try:
    import bpy
except ImportError:
    print('OBJ exported. Use Blender to also generate manta.blend.')
else:
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)
    mesh=bpy.data.meshes.new('Manta polygon source')
    mesh.from_pydata(VERTICES, [], FACES)
    mesh.update()
    ship=bpy.data.objects.new('MANTA-07 / AI-attributed survey craft',mesh)
    bpy.context.collection.objects.link(ship)
    bpy.context.view_layer.objects.active=ship
    ship.select_set(True)
    material=bpy.data.materials.new('Ceramic titanium')
    material.diffuse_color=(.27,.48,.54,1)
    mesh.materials.append(material)
    ship['attribution']='Generated with OpenAI Codex. Not student-authored.'
    bpy.ops.wm.save_as_mainfile(filepath=str(assets/'manta.blend'))
