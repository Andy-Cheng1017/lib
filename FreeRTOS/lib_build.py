from os.path import join, realpath
Import('env')


src_filter = [
    "+<source/*>",
    "+<source/portable/memmang/heap_2.c>",
    "+<source/portable/GCC/ARM_CM4F/*>",
    "-<source/portable/common/*>",
]

env.Replace(SRC_FILTER=src_filter)
env.Append(CPPPATH=[realpath("source/include")])
env.Append(CPPPATH=[realpath("source/portable/GCC/ARM_CM4F")])
# FreeRTOSConfig.h is located in the project space, outside of the lib:
env.Append(CPPPATH=env.get("PROJECT_INCLUDE_DIR", []))


