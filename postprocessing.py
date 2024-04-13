import re

dot_file = "new_icfg.dot.dot"
output_file = "new_icfg_configured.dot"
visited_nodes = []
pattern = re.compile(r"0x[a-zA-Z0-9]+")
with open("visited_nodes.txt", "r", encoding="utf-8") as file:
    visited_nodes = file.readlines()
    visited_nodes = [node.strip() for node in visited_nodes]


out_lines = []
with open(dot_file, "r", encoding="utf-8") as file:
    lines = file.readlines()
    for line in lines:
        matches = pattern.findall(line)
        is_included = False
        if len(matches) == 0:
            out_lines.append(line)
        elif len(matches) == 1:
            if matches[0] in visited_nodes:
                out_lines.append(line)
        elif len(matches) == 2:
            if matches[0] in visited_nodes and matches[1] in visited_nodes:
                out_lines.append(line)

with open(output_file, "w", encoding="utf-8") as file:
    file.writelines(out_lines)
