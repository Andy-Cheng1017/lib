import os
from os.path import join, realpath
Import('env')

# include_dirs = []
# for root, dirs, files in os.walk("source/include"):
#     if any(fname.endswith('.h') for fname in files):
#         include_dirs.append(os.path.realpath(root))
        
# print("Include dirs:", include_dirs)  # 確認實際包含的路徑

src_filter = [
    "+<port/*>",
    "+<source/api/*>",
    "+<source/netif/*>",
    "+<source/core/*>",
]

env.Replace(SRC_FILTER=src_filter)
env.Append(CPPPATH=[realpath("port")])
env.Append(CPPPATH=[realpath("port/arch")])
env.Append(CPPPATH=[realpath("source/include")])
env.Append(CPPPATH=[realpath("source/include/lwip")])
env.Append(CPPPATH=[realpath("source/include/netif")])
# env.Append(CPPPATH=[realpath("source/include/compat")])
# env.Append(CPPPATH=include_dirs)




