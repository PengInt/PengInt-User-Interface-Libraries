import bpy
import bmesh
import json
import os

def export_mesh_to_atomic_json(filepath, colour, material):
    obj = bpy.context.active_object
    if not obj or obj.type != 'MESH':
        print("Please select a mesh object.")
        return

    # Get the evaluated mesh (applies modifiers like Decimate)
    depsgraph = bpy.context.evaluated_depsgraph_get()
    obj_eval = obj.evaluated_get(depsgraph)
    mesh_data = obj_eval.to_mesh()

    bm = bmesh.new()
    bm.from_mesh(mesh_data)

    # Force triangulation for your 3-index logic
    bmesh.ops.triangulate(bm, faces=bm.faces[:])

    # Ensure indices are fresh and sequential (0, 1, 2...)
    bm.verts.ensure_lookup_table()
    bm.faces.ensure_lookup_table()

    # The Flat Structure for C++ std::vector
    data = {
        "v": [], # Vertices: [x, y, z, x, y, z...]
        "t": []  # Triangles: [i1, i2, i3, r, g, b, mat, ...]
    }

    # 1. Flatten Vertices
    for v in bm.verts:
        # Rounding to 4 saves file size; 6 is "Sprocket" precision
        data["v"].extend([round(v.co.x, 4), round(v.co.y, 4), round(v.co.z, 4)])

    # 2. Flatten Triangles (The 7-element stride)
    for f in bm.faces:
        # i1, i2, i3
        data["t"].extend([v.index for v in f.verts])
        # R, G, B, Material (Padding)
        data["t"].extend([colour[0], colour[1], colour[2], material])

    # Write to file - indent=None makes the file much smaller/faster to parse
    with open(filepath, 'w') as f:
        json.dump(data, f)

    print(f"Atomic Age Export: {len(bm.verts)} verts, {len(bm.faces)} tris.")
    bm.free()
    obj_eval.to_mesh_clear()

# Config
output_filepath = "D:\\name\\Documents\\model_data.json"
export_mesh_to_atomic_json(output_filepath, [255, 127, 0], 0)