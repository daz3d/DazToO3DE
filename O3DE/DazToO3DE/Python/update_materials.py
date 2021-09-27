import os
import json

g_file_dir = r"D:\o3de-projects\MyProject\Assets\DazImports\Kayo8"
g_tex_lib = {
    "Diffuse Color" : "baseColor",
    "Glossy Roughness" : "roughness",
    "Specular Lobe 1 Roughness" : "roughness",
    "Normal Map" : "normal",
    "Cutout Opacity" : "opacity",
    "Refraction Weight" : "opacity",
    "Metallic Weight" : "metallic"
}

def convert_color(color):
    '''Takes a hex rgb string (e.g. #ffffff) and returns an RGB tuple (float, float, float).'''
    return tuple(int(color[i:i + 2], 16) / 255. for i in (1, 3, 5)) # skip '#'

def find_dtu_path():
    for x in os.listdir(g_file_dir):
        if x.endswith(".dtu"):
            dtu_file = x
            break
    return os.path.join(g_file_dir, dtu_file)

def load_dtu(dtu_path):
    with open(dtu_path, "r") as f:
        dtu = json.load(f)
    return dtu

def load_dtu_materials(dtu_path):
    dtu = load_dtu(dtu_path)
    material_info = {}
    for mat in dtu["Materials"]:
        mat_name = mat["Material Name"]
        if mat_name not in material_info.keys():
            material_info[mat_name] = mat
    return material_info


def get_o3de_materials():
    o3de_mats = []
    for x in os.listdir(g_file_dir):
       if x.endswith(".material"):
           o3de_mats.append(os.path.join(g_file_dir,x))
    return o3de_mats

def update_materials(o3de_mats, materials_data):
    for mat_path in o3de_mats:
        material_data = {}
        root_dir = os.path.dirname(os.path.dirname(mat_path))
        print(mat_path)
        mat_name = os.path.basename(mat_path).replace(".material", "").replace("B_Kayo8_","")
        print(mat_name)
        mat_info = materials_data[mat_name]
        material_data["materialType"] = "Materials/Types/StandardPBR.materialtype"
        material_data["properties"] = {}
        for prop in  mat_info["Properties"]:
            if prop["Name"] in g_tex_lib.keys():
                if g_tex_lib[prop["Name"]] not in material_data["properties"].keys():
                    material_data["properties"][g_tex_lib[prop["Name"]]] = {}

                if prop["Data Type"] == "Color":
                    color = convert_color(prop["Value"])
                    material_data["properties"][g_tex_lib[prop["Name"]]]["color"] = [color[0], color[1], color[2], 1.0]
                if prop["Texture"] != "":
                    
                    material_data["properties"][g_tex_lib[prop["Name"]]]["textureMap"] = os.path.join(root_dir, prop["Texture"].replace("/","\\"))

                    if prop["Name"] == "Normal Map":
                        material_data["properties"][g_tex_lib[prop["Name"]]]["flipY"] = True
                    if prop["Name"] == "Cutout Opacity": 
                        if prop["Texture"] != "":
                            material_data["properties"][g_tex_lib[prop["Name"]]]["alphaSource"] = "Split"
                            material_data["properties"][g_tex_lib[prop["Name"]]]["doubleSided"] = True
                            material_data["properties"][g_tex_lib[prop["Name"]]]["mode"] = "Cutout"

                if prop["Name"] == "Refraction Weight": 
                    if prop["Value"] > 0:
                        material_data["properties"][g_tex_lib[prop["Name"]]]["alphaSource"] = "None"
                        material_data["properties"][g_tex_lib[prop["Name"]]]["doubleSided"] = True
                        material_data["properties"][g_tex_lib[prop["Name"]]]["mode"] = "Blended"
                        material_data["properties"][g_tex_lib[prop["Name"]]]["factor"] = 1 - prop["Value"]
                if prop["Name"] == "Metallic Weight": 
                    if prop["Value"] > 0:
                        material_data["properties"][g_tex_lib[prop["Name"]]]["factor"] = prop["Value"]
                        material_data["properties"]["specularF0"] = {}
                        material_data["properties"]["specularF0"]["factor"] = 1
        if "specularF0" not in material_data["properties"].keys():
            material_data["properties"]["specularF0"] = {}
            material_data["properties"]["specularF0"]["factor"] = 0.02

        with open(mat_path, "w") as f:
            json.dump(material_data, f, indent=2)
def main():
    dtu_path = find_dtu_path()
    material_info = load_dtu_materials(dtu_path)
    o3de_mats = get_o3de_materials()
    update_materials(o3de_mats, material_info)

main()