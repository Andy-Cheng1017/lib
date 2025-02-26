import os
from os.path import join, realpath
Import('env')

src_filter = [
    "+<port/*>",
    "+<RX8025T/*>",
    "+<MCP342X/*>",
    # "+<wrapper/*>",
]

env.Replace(SRC_FILTER=src_filter)
env.Append(CPPPATH=[realpath("port")])
env.Append(CPPPATH=[realpath("RX8025T")])
env.Append(CPPPATH=[realpath("MCP342X")])
# env.Append(CPPPATH=[realpath("wrapper")])
# env.Append(CCFLAGS=["-xc++", "-std=c++11"])
# env.Append(CPPPATH=[realpath("easylogger/plugins/flash")])


