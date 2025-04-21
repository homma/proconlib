# build file for example01.cpp

import subprocess

# Config
target_name = "procon_check"
src = "example01.cpp"

# Library
## SDL3
sdl3 = {}
sdl3["lib"] = "SDL3"
sdl3["path"] = (
    subprocess.Popen(["brew", "--prefix", "sdl3"], stdout=subprocess.PIPE)
    .stdout.read()
    .decode("UTF-8")
    .strip()
)
sdl3["lib_path"] = sdl3["path"] + "/lib"
sdl3["include_path"] = sdl3["path"] + "/include"

# Executable
Program(
    target_name,
    src,
    LIBS=[sdl3["lib"]],
    LIBPATH=[sdl3["lib_path"]],
    CPPPATH=[sdl3["include_path"]],
    CCFLAGS=["-std=c++2c"],
)
