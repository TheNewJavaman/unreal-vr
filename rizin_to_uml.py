# rizin_to_uml.py
# Converts rizin zj output to UnrealModLoader's pattern format

import json
import sys

if len(sys.argv) > 1:
    n = int(sys.argv[1])
else:
    n = 16
for signatures in sys.stdin:
    for signature in json.loads(signatures):
        line = signature["name"] + ":"
        for i in range(n):
            if signature["mask"][i * 2:i * 2 + 2] == "00":
                line += " ?"
            else:
                line += " " + signature["bytes"][i * 2:i * 2 + 2].upper()
        print(line)
