import os
from os.path import join, realpath
Import('env')

src_filter = [
    "+<easylogger/port/*>",
    "+<easylogger/src/*>",
    # "-<easylogger/src/elog_async.c>",
    # "+<easylogger/plugins/file/*>",
    # "+<easylogger/plugins/flash/*>",
]

env.Replace(SRC_FILTER=src_filter)
env.Append(CPPPATH=[realpath("easylogger/inc")])
# env.Append(CPPPATH=[realpath("easylogger/plugins/file")])
# env.Append(CPPPATH=[realpath("easylogger/plugins/flash")])




