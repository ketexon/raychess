import glob
import tomllib
from pathlib import Path
from typing import Any, Sequence, Tuple

output_dir = Path("src/generated/")
output_file_name = Path("Assets")

output_header_path = output_dir / output_file_name.with_suffix(".h")
output_source_path = output_dir / output_file_name.with_suffix(".c")

import_files = glob.glob("assets/**/*.import", recursive=True)

import_entries: Sequence[Tuple[bytes, dict]] = []

for import_file_path in import_files:
    file_path = Path(import_file_path).with_suffix("")
    if not file_path.exists() or file_path.is_dir():
        print(f"WARNING: import file `{import_file_path}' has no corresponding data file `{file_path}'.")

    file_data = None
    with file_path.open("rb") as f:
        file_data = f.read()

    meta = None
    with open(import_file_path, 'rb') as f:
        meta = tomllib.load(f)

    if not meta["name"]:
        print(f"WARNING: import file `{import_file_path}' has field \"name\".")

    import_entries.append((file_data, meta))

header_data = "\n".join(
    [f"extern const unsigned char {e["name"]}__DATA[{len(data)}];" for data,e in import_entries]
)

def get_metadata_entries(d: dict, prefix: str = ""):
    for k, v in d.items():
        if isinstance(v, dict):
            yield from get_metadata_entries(v, f"{prefix}{k}__")
        else:
            if k != "name":
                yield (f"{prefix}{k}", v)

header_metadata = "\n".join(
    [f"#define {k} {v}" for _, e in import_entries for k,v in get_metadata_entries(e, f"{e["name"]}__")]
);

header_file = f'''#ifndef ASSETS_H
#define ASSETS_H

{header_data}
{header_metadata}

#endif
'''

def partition[T](l: Sequence[T], size: int) -> Sequence[Sequence[T]]:
    return [l[i:i+size] for i in range(0, len(l), size)]

source_data = "\n".join(
    [
        f"const unsigned char {e["name"]}__DATA[{len(data)}] = {{\n"
            + "\n".join([
                "  " + " ".join([f"{hex(b)}," for b in row])
                for row
                in partition(data, 8)
            ])
            + "\n};"
        for data,e
        in import_entries
    ]
)

source_file = f'''#include "{output_file_name}.h"

{source_data}

'''


if not output_dir.exists():
    output_dir.mkdir(parents=True)

output_header_path.write_text(header_file)
output_source_path.write_text(source_file)
