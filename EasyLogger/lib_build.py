import os
from os.path import join, realpath
Import('env')

src_filter = [
    "+<easylogger/port/*>",
    "+<easylogger/src/*>",
]

env.Replace(SRC_FILTER=src_filter)
env.Append(CPPPATH=[realpath("easylogger/inc")])





