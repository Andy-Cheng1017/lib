import os
from os.path import join, realpath
Import('env')

src_filter = [
    "+<port/*>",
    "+<RX8025T/*>",
    "+<MCP342X/*>",
]

env.Replace(SRC_FILTER=src_filter)
env.Append(CPPPATH=[realpath("port")])
env.Append(CPPPATH=[realpath("RX8025T")])
env.Append(CPPPATH=[realpath("MCP342X")])



