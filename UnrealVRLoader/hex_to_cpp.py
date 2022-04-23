# hex_to_cpp.py
# Converts a hex string to C++-formatted bytes

import sys

for line in sys.stdin:
    n = int(len(line) / 2)
    output = str(n) + ":"
    for i in range(n):
        tmp = line[i * 2:i * 2 + 2]
        if i == 0:
            output += " 0x" + tmp
        else:
            output += ", 0x" + tmp
    print(output)
