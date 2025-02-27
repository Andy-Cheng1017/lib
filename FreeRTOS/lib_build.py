from os.path import join, realpath

Import("env")


board_config = env.BoardConfig()

cpu_type = board_config.get(
    "build.cpu", ""
)  # 通常會回傳類似 "cortex-m3"、"cortex-m4"、"cortex-m4f"等
# mcu_type = board_config.get("build.mcu", "")  # 若你想讀 MCU 類型也可以
print("cpu_type =", cpu_type, flush=True)

def has_fpu_by_cpu_type():
    # 如果是 "cortex-m4f" 或有 "f"（但還要看實際定義），可以直接視為有 FPU
    print(f"Checking FPU from cpu_type: '{cpu_type}'", flush=True)  # 記錄 cpu_type
    return "m4f" in cpu_type or "m7f" in cpu_type


def has_fpu_by_flags():
    cflags = env.get("CCFLAGS", [])
    for f in cflags:
        # 簡單判斷，只要有 -mfpu= 或 -mfloat-abi=hard 之類就認定為有 FPU
        if "-mfpu=" in f:
            return True
        if "-mfloat-abi=softfp" in f:
            return True
    return False


if has_fpu_by_cpu_type() or has_fpu_by_flags():
    #
    # 2) 如果偵測有 FPU (ARM_CM4F)
    #
    print("Using ARM_CM4F", flush=True)
    src_filter = [
        "+<source/*>",
        "+<source/portable/memmang/heap_4.c>",
        "-<source/portable/GCC/ARM_CM3/*>",
        "+<source/portable/GCC/ARM_CM4F/*>",
        "-<source/portable/common/*>",
    ]

    env.Replace(SRC_FILTER=src_filter)
    env.Append(CPPPATH=[realpath("source/include")])
    env.Append(CPPPATH=[realpath("source/portable/GCC/ARM_CM4F")])
else:
    #
    # 3) 否則就用 ARM_CM3 或其他
    
    print("Using ARM_CM3", flush=True)
    src_filter = [
        "+<source/*>",
        "+<source/portable/memmang/heap_4.c>",
        "+<source/portable/GCC/ARM_CM3/*>",
        "-<source/portable/GCC/ARM_CM4F/*>",
        "-<source/portable/common/*>",
    ]

    env.Replace(SRC_FILTER=src_filter)
    env.Append(CPPPATH=[realpath("source/include")])
    env.Append(CPPPATH=[realpath("source/portable/GCC/ARM_CM3")])
