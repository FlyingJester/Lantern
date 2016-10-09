import os
import sys

# The EnableableOption functions:
def EnabledString(that):
    if that:
        return "enabled"
    else:
        return "disabled"

def EnableableOption(name, default):
    AddOption("--enable-" + name,
        action="store_true", dest="en_" + name, default=False,
        help="Enables " + name)
    AddOption("--disable-" + name,
        action="store_true", dest="dis_" + name, default=False,
        help="Disables " + name)
    enable = GetOption("en_" + name)
    disable = GetOption("dis_" + name)
    if enable and disable:
        print("Warning: Both enable and disable " + name + " specified.")
        return default
    if disable:
        return False
    elif enable:
        return True
    else:
        return default

environment = Environment(ENV = os.environ)
environment.Append(LIBPATH = [os.path.join(os.getcwd(), "lib")], CPPPATH = [os.getcwd(), os.path.join(os.getcwd(), "include")])

environment.Append(tools=['nasm'])
environment.Replace(AS = "yasm")

is32bit = EnableableOption("m32", False)
mingw = EnableableOption("mingw", sys.platform == "msys" or sys.platform == "cygwin")

if mingw:
	environment.Append(CPPDEFINES="_WIN32=1")
else:
    environment.Append(CCFLAGS="/EHsc")

if is32bit:
	if mingw or os.name == "posix":
		environment.Append(ASFLAGS = " -f elf ")
	elif os.name == "nt":
		environment.Append(ASFLAGS = " -f win32 -d win32 ")
	else:
		print ("Add the asm init routines for your platform!")
		quit()
else:
	if mingw:
		environment.Append(CPPDEFINES="_WIN64=1")
	
	if mingw or os.name == "posix":
		environment.Append(ASFLAGS = " -f elf64 -m amd64 ")
	elif os.name == "nt":
		environment.Append(ASFLAGS = " -f win64 -d win64 ")
	else:
		print ("Add the asm init routines for your platform!")
		quit()

if mingw:
	environment.Append(LINKFLAGS = "-mwindows")
	if is32bit:
		environment.Append(LINKFLAGS = "-m32")
	environment.Append(LINKFLAGS = " -static-libgcc -static-libstdc++ ")
elif os.name == "nt":
    environment.Append(CCFLAGS = " /Zi ", LINKFLAGS = " /DEBUG ")

if mingw or os.name == "posix":
	environment.Append(LINKFLAGS = " -g ", CCFLAGS = " -g -funroll-loops -ffast-math -msse2 -Wall -Werror -Wextra -pedantic ",
		CFLAGS = " -ansi -Os ", CXXFLAGS = " -O3 -fno-rtti -fno-exceptions -std=c++98 ")

base = environment.Clone()

bufferfile = SConscript(dirs = ["bufferfile"], exports = ["environment"])
environment = base.Clone()

libnames = ["dlcl", "turbojson", "glow", "aimg", "spherefonts", "chrono"]
libs = [bufferfile]

for name in libnames:
    result = SConscript(dirs = [name], exports = ["environment", "bufferfile"])
    if os.name != "posix" and len(result) > 1:
        rlibs = [{}, {}]
        for r in result:
            if str(r).endswith(".dll"):
                rlibs[1] = r
            elif str(r).endswith(".a") or str(r).endswith(".lib"):
                rlibs[0] = r

        Install(os.path.join(os.getcwd(), "lantern"), rlibs[1])
        libs.append(rlibs[0])
    else:
        libs.append(result)
    environment = base.Clone()

lantern = SConscript(dirs = ["src"], exports = ["environment", "libs", "is32bit", "mingw"])
Install(os.path.join(os.getcwd(), "lantern"), lantern)
