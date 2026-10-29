#!/usr/bin/env python3
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
OUT = ROOT / "src" / "games" / "pm64" / "Pm64SpriteNames.generated.cpp"

SPRITE_ID = re.compile(r"^#define SPR_([A-Za-z0-9]+) (0x[0-9A-Fa-f]+)$", re.M)
PLAYER_ID = re.compile(r"^\s*SPR_([A-Za-z0-9_]+) = (0x[0-9A-Fa-f]+),$", re.M)
PREFIXES = ("Battle", "World")


def spaced(name: str) -> str:
    return re.sub(r"(?<=[a-z0-9])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])", " ", name).replace("_", " ")


def group_of(sheet: str) -> str:
    for prefix in PREFIXES:
        if sheet.startswith(prefix) and len(sheet) > len(prefix) and sheet[len(prefix)].isupper():
            return sheet[len(prefix):]
    return sheet


def raster_names(text: str, sheet: str) -> list:
    pattern = re.compile(r"^#define SPR_IMG_" + re.escape(sheet) + r"_([A-Za-z0-9_]+) (0x[0-9A-Fa-f]+)$", re.M)
    names = {}
    for name, value in pattern.findall(text):
        names.setdefault(int(value, 16), name)
    return [names.get(i) for i in range(max(names) + 1)] if names else []


def c_array(name: str, values: list) -> str:
    items = ", ".join(f'"{v}"' if v else "nullptr" for v in values)
    return f"const char* const {name}[] = {{ {items} }};\n"


def main() -> None:
    if len(sys.argv) != 2:
        sys.exit("usage: gen_pm64_sprites.py <PaperBoat>/include/sprite")
    sprite_dir = pathlib.Path(sys.argv[1])

    sheets = {"Npc": [], "Player": []}
    for header in sorted((sprite_dir / "npc").glob("*.h")):
        text = header.read_text(encoding="utf-8")
        for name, value in SPRITE_ID.findall(text):
            if name == header.stem:
                sheets["Npc"].append((int(value, 16), name, group_of(name), raster_names(text, name)))
    player_text = (sprite_dir / "player.h").read_text(encoding="utf-8")
    for name, value in PLAYER_ID.findall(player_text):
        group = "Peach" if name.startswith("Peach") else "Mario"
        sheets["Player"].append((int(value, 16), name, group, raster_names(player_text, name)))

    out = [
        "#include \"Pm64SpriteNames.h\"\n\n",
        "namespace Pm64 {\n",
        "namespace {\n",
    ]
    for kind, entries in sheets.items():
        for _, name, _, rasters in entries:
            if rasters:
                out.append(c_array(f"k{kind}{name}Rasters", rasters))
    out.append("} // namespace\n\n")
    for kind, entries in sheets.items():
        entries.sort()
        out.append(f"const SpriteSheetName k{kind}SpriteNames[] = {{\n")
        expected = 1
        for value, name, group, rasters in entries:
            while expected < value:
                out.append("    { nullptr, nullptr, nullptr, 0 },\n")
                expected += 1
            table = f"k{kind}{name}Rasters" if rasters else "nullptr"
            out.append(f'    {{ "{spaced(group)}", "{spaced(name)}", {table}, {len(rasters)} }},\n')
            expected += 1
        out.append("};\n")
        out.append(f"const int k{kind}SpriteNameCount = {expected - 1};\n\n")
    out.append("} // namespace Pm64\n")
    OUT.write_text("".join(out), encoding="utf-8")
    print(f"Wrote {OUT.relative_to(ROOT)} ({len(sheets['Npc'])} npc, {len(sheets['Player'])} player sprites)")


if __name__ == "__main__":
    main()
