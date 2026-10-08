"""RogueEngine Blender Bridge exporter. Run inside Blender."""
import argparse
import json
import sys
from pathlib import Path
import bpy

SCHEMA = "rogueengine.bridge"
SCHEMA_VERSION = 1

def vec3(v): return [float(v[0]), float(v[1]), float(v[2])]
def quat(q): return [float(q[0]), float(q[1]), float(q[2]), float(q[3])]
def transform(obj):
    return {"position": vec3(obj.location), "rotation": quat(obj.rotation_quaternion), "scale": vec3(obj.scale)}

def export_shape_keys(obj):
    result = []
    if not obj.data.shape_keys: return result
    basis = obj.data.shape_keys.key_blocks[0]
    base = [tuple(p.co) for p in basis.data]
    for key in obj.data.shape_keys.key_blocks[1:]:
        deltas = []
        for i, point in enumerate(key.data):
            b = base[i]
            deltas.extend([float(point.co.x-b[0]), float(point.co.y-b[1]), float(point.co.z-b[2])])
        result.append({
            "name": key.name, "position_deltas": deltas, "weight": float(key.value),
            "slider_min": float(key.slider_min), "slider_max": float(key.slider_max),
            "category": str(key.get("DazMorphCategory", "")), "body_part": str(key.get("DazBodyPart", ""))
        })
    return result

def export_drivers(obj):
    result = []
    shape_keys = obj.data.shape_keys
    if not shape_keys or not shape_keys.animation_data: return result
    for fcurve in shape_keys.animation_data.drivers:
        variables = []
        for var in fcurve.driver.variables:
            for target in var.targets:
                variables.append({"name": var.name, "source": target.data_path, "scale": 1.0})
        result.append({
            "target": fcurve.data_path, "expression": fcurve.driver.expression, "variables": variables
        })
    return result

def export_armature(obj):
    bones = []
    for bone in obj.data.bones:
        bones.append({
            "name": bone.name, "parent": bone.parent.name if bone.parent else "",
            "rest_transform": {"position": vec3(bone.head_local), "rotation": [0,0,0,1], "scale": [1,1,1]}
        })
    return {"name": obj.name, "bones": bones}

def export_scene():
    nodes=[]; armatures=[]; morphs=[]; constraints=[]
    for obj in bpy.context.scene.objects:
        nodes.append({
            "name": obj.name, "parent": obj.parent.name if obj.parent else "",
            "mesh": obj.data.name if obj.type=="MESH" else "",
            "armature": obj.data.name if obj.type=="ARMATURE" else "",
            "transform": transform(obj)
        })
        if obj.type == 'MESH':
            keys=export_shape_keys(obj)
            if keys:
                morphs.append({
                    "name": obj.name, "category": str(obj.get("DazMorphCategory","")),
                    "body_part": str(obj.get("DazBodyPart","")), "shape_keys": keys,
                    "drivers": export_drivers(obj)
                })
        if obj.type == 'ARMATURE': armatures.append(export_armature(obj))
        for con in obj.constraints:
            constraints.append({
                "owner": obj.name, "type": con.type,
                "target": con.target.name if con.target else "", "influence": float(con.influence)
            })
    return {
        "schema": SCHEMA, "schema_version": SCHEMA_VERSION,
        "name": Path(bpy.data.filepath).stem if bpy.data.filepath else "Untitled",
        "source_format": "blend", "source_application": "Blender",
        "scene": {"nodes": nodes, "armatures": armatures, "morphs": morphs, "constraints": constraints},
        "metadata": {"blend_filepath": bpy.data.filepath, "object_count": len(bpy.context.scene.objects)}
    }

def main():
    argv=sys.argv[sys.argv.index('--')+1:] if '--' in sys.argv else []
    parser=argparse.ArgumentParser(); parser.add_argument('--output', required=True)
    args=parser.parse_args(argv)
    output=Path(args.output); output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(export_scene(), indent=2, ensure_ascii=False), encoding='utf-8')
    print('RogueEngine bridge export:', output)

if __name__ == '__main__': main()