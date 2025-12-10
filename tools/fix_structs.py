#!/usr/bin/env python3
"""Post-processing script to replace D_8009XXXX symbols with Savemap.field_name.

This script takes decompiled code and replaces raw address symbols that fall within
the Savemap struct with proper struct member accesses.

Usage:
    python fix_structs.py < input.c > output.c
    python fix_structs.py input.c
    cat file.c | python fix_structs.py
"""

import re
import sys

SAVEMAP_BASE = 0x8009C6E4

SAVEMAP_FIELDS = [
    # (offset, size, name, element_size) - element_size > 0 means array
    # SaveHeader (embedded)
    (0x000, 4, "header.checksum", 0),
    (0x004, 1, "header.leader_level", 0),
    (0x005, 1, "header.leader_portrait", 0),
    (0x006, 1, "header.portrait2", 0),
    (0x007, 1, "header.portrait3", 0),
    (0x008, 16, "header.leader_name", 1),
    (0x018, 2, "header.leader_hp", 0),
    (0x01A, 2, "header.leader_hp_max", 0),
    (0x01C, 2, "header.leader_mp", 0),
    (0x01E, 2, "header.leader_mp_max", 0),
    (0x020, 4, "header.gil", 0),
    (0x024, 4, "header.time", 0),
    (0x028, 32, "header.place_name", 1),
    (0x048, 12, "header.menu_color", 4),
    # SavePartyMember party[9] - 0x84 bytes each
    (0x054, 0x4A4, "party", 0x84),
    # Rest of SaveWork
    (0x4F8, 4, "partyID", 1),
    (0x4FC, 640, "inventory", 2),
    (0x77C, 800, "materia", 4),
    (0xA9C, 192, "yuffie_stolen_materia", 4),
    (0xB5C, 32, "unk_b5c", 1),
    (0xB7C, 4, "gil", 0),
    (0xB80, 4, "time", 0),
    (0xB84, 4, "countdown_timer_seconds", 0),
    (0xB88, 4, "game_timer_fraction", 0),
    (0xB8C, 4, "countdown_timer_fraction", 0),
    (0xB90, 4, "worldmap_exit_action", 0),
    (0xB94, 2, "current_module", 0),
    (0xB96, 2, "current_location_id", 0),
    (0xB98, 2, "padding2", 0),
    (0xB9A, 2, "field_x", 0),
    (0xB9C, 2, "field_y", 0),
    (0xB9E, 2, "field_triangle", 0),
    (0xBA0, 1, "field_direction", 0),
    (0xBA1, 1, "step_id", 0),
    (0xBA2, 1, "step_offset", 0),
    (0xBA3, 1, "padding3", 0),
    (0xBA4, 256, "memory_bank_1", 1),
    (0xCA4, 256, "memory_bank_2", 1),
    (0xDA4, 256, "memory_bank_3", 1),
    (0xEA4, 256, "memory_bank_4", 1),
    (0xFA4, 256, "memory_bank_5", 1),
    (0x10A4, 2, "phs_locking_mask", 0),
    (0x10A6, 2, "phs_visibility_mask", 0),
    (0x10A8, 48, "unk_10a8", 1),
    (0x10D8, 1, "battle_speed", 0),
    (0x10D9, 1, "battle_msg_speed", 0),
    (0x10DA, 2, "config", 0),
    (0x10DC, 16, "button_config", 1),
    (0x10EC, 1, "field_msg_speed", 0),
    (0x10ED, 1, "D_8009D7D1", 0),
    (0x10EE, 2, "D_8009D7D2", 0),
    (0x10F0, 4, "D_8009D7D4", 0),
]

SAVEMAP_END = SAVEMAP_BASE + 0x10F4


def build_offset_map():
    """Build a map from offset to (field_name, element_size, field_offset)."""
    offset_map = {}
    for field_offset, size, name, elem_size in SAVEMAP_FIELDS:
        if elem_size > 0:
            for i in range(size // elem_size):
                offset_map[field_offset + i * elem_size] = (name, elem_size, field_offset)
        else:
            offset_map[field_offset] = (name, 0, field_offset)
    return offset_map


def address_to_field(addr, offset_map):
    """Convert an address to a Savemap.field reference."""
    if addr < SAVEMAP_BASE or addr >= SAVEMAP_END:
        return None

    offset = addr - SAVEMAP_BASE

    if offset in offset_map:
        name, elem_size, field_offset = offset_map[offset]
        if elem_size > 0:
            idx = (offset - field_offset) // elem_size
            return f"Savemap.{name}[{idx}]"
        return f"Savemap.{name}"

    for field_offset, size, name, elem_size in SAVEMAP_FIELDS:
        if field_offset <= offset < field_offset + size:
            if elem_size > 0:
                idx = (offset - field_offset) // elem_size
                remainder = (offset - field_offset) % elem_size
                if remainder == 0:
                    return f"Savemap.{name}[{idx}]"
                return f"Savemap.{name}[{idx}] + 0x{remainder:X}"
            return f"Savemap.{name}"

    return f"Savemap + 0x{offset:X}"


def fix_symbols(code):
    """Replace D_8009XXXX symbols with Savemap.field references."""
    offset_map = build_offset_map()
    symbol_pattern = re.compile(r'\bD_(8009[0-9A-Fa-f]{4})\b')
    extern_pattern = re.compile(r'^extern\s+(?:/\*\?\*/\s*)?\w+\*?\s+Savemap\.[^;]+;\s*\n', re.MULTILINE)

    def replace_symbol(match):
        addr = int(match.group(1), 16)
        field = address_to_field(addr, offset_map)
        if field:
            return field
        return match.group(0)

    result = symbol_pattern.sub(replace_symbol, code)
    result = extern_pattern.sub('', result)
    return result


def main():
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r') as f:
            code = f.read()
        result = fix_symbols(code)
        print(result, end='')
    else:
        code = sys.stdin.read()
        result = fix_symbols(code)
        print(result, end='')


if __name__ == "__main__":
    main()
