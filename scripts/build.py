import sys
import subprocess
import os
import shutil

MINGW_MODE = "mingw"
EMSCRIPTEN_MODE = "emscripten"

VALID_MODES = [MINGW_MODE, EMSCRIPTEN_MODE]

mode = sys.argv[1]

if mode not in VALID_MODES:
    raise Exception(f"First argument should be one of: {VALID_MODES}")

all_cpp_files = [];

dir_path = os.path.dirname(os.path.realpath(__file__))

os.chdir(f"{dir_path}/../")

for root, dirs, files in os.walk("src"):
    for file_name in files:
        if not file_name.endswith(".cpp"):
            continue
        
        all_cpp_files.append(os.path.join(root, file_name))

common_flags = [
    "-Wall",
    "-std=c++17",
]

def rebuild_dir(dir_name):
    if os.path.exists(dir_name):
        shutil.rmtree(dir_name)

    os.mkdir(dir_name)

if mode == MINGW_MODE:
    rebuild_dir("./bin/mingw")

    args = [
        "g++",
        *common_flags,
        "-fdiagnostics-color=always",
        "-g",
        *all_cpp_files,
        "-o",
        "./bin/mingw/main.exe",
        "-L./lib/mingw64",
        "-I./include",
        "-I./src",
        "-lraylib",
        "-lflecs",
        "-lm",
        "-lwinmm",
        "-lgdi32",
        "-lWs2_32"
    ]
else:
    rebuild_dir("./bin/emscripten")
    
    args = [
        "emcc.bat",
        *common_flags,
        *all_cpp_files,
        "-Os",
        "-o",
        "./bin/emscripten/index.html",
        "-L./lib/emscripten",
        "-I./include",
        "-I./src",
        "-s", 
        "ALLOW_MEMORY_GROWTH=1",
        "-s",
        "EXPORTED_RUNTIME_METHODS=cwrap",
        "-s",
        "USE_GLFW=3",
        "-DPLATFORM_WEB",
        "-lraylib",
        "-lflecs",
    ]


subprocess.call(args)