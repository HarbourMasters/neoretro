#!/usr/bin/env python3
import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
ARB_DIR = ROOT / "l10n"
OUT = ROOT / "src" / "l10n" / "Strings.generated.cpp"


def strip_emoji(value: str) -> str:
    kept = []
    for ch in value:
        cp = ord(ch)
        if cp == 0xFE0F or 0x2600 <= cp <= 0x27BF or cp >= 0x1F000:
            continue
        kept.append(ch)
    return "".join(kept).rstrip()


def c_string(value: str) -> str:
    out = []
    for byte in value.encode("utf-8"):
        ch = chr(byte)
        if ch == "\\":
            out.append("\\\\")
        elif ch == '"':
            out.append('\\"')
        elif ch == "\n":
            out.append("\\n")
        elif 0x20 <= byte < 0x7F:
            out.append(ch)
        else:
            out.append(f'\\x{byte:02x}""')
    return '"' + "".join(out) + '"'


def main() -> None:
    locales = {}
    for arb in sorted(ARB_DIR.glob("app_*.arb")):
        data = json.loads(arb.read_text(encoding="utf-8"))
        locale = arb.stem.split("_", 1)[1]
        locales[locale] = {
            k: strip_emoji(v) for k, v in data.items() if not k.startswith("@") and isinstance(v, str)
        }

    lines = [
        '#include "Strings.h"',
        "",
        "namespace L10n::Generated {",
        "const std::vector<Locale> kLocales = {",
    ]
    for locale in sorted(locales, key=lambda l: (l != "en", l)):
        lines.append(f'    {{ "{locale}", {{')
        for key, value in sorted(locales[locale].items()):
            lines.append(f'        {{ "{key}", {c_string(value)} }},')
        lines.append("    } },")
    lines += ["};", "} // namespace L10n::Generated", ""]
    OUT.write_text("\n".join(lines), encoding="utf-8")
    print(f"Wrote {OUT.relative_to(ROOT)} ({len(locales)} locales)")


if __name__ == "__main__":
    main()
