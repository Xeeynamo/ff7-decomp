#!.venv/bin/python3
import enum
import sys
import struct
import gzip
import argparse
from pathlib import Path

DEFAULT_KERNEL = Path(__file__).resolve().parent.parent / \
    'disks/us/INIT/KERNEL.BIN'

TEXT_SECTION_TYPE = 9
TEXT_ENCODING_BACKREF = 0xF9
TEXT_ENCODING_TERMINATOR = 0xFF
SECTION_HEADER = struct.Struct('<3H')  # packed size, unpacked size, type


# mirrors KernelTextBlockID in main_private.h
class KernelTextSections(enum.IntEnum):
    DESC_COMMAND = 0x00
    DESC_MAGIC = 0x01
    DESC_ITEM = 0x02
    DESC_WEAPON = 0x03
    DESC_ARMOR = 0x04
    DESC_ACCESSORY = 0x05
    DESC_MATERIA = 0x06
    DESC_KEY_ITEM = 0x07
    NAME_COMMAND = 0x08
    NAME_MAGIC = 0x09
    NAME_ITEM = 0x0A
    NAME_WEAPON = 0x0B
    NAME_ARMOR = 0x0C
    NAME_ACCESSORY = 0x0D
    NAME_MATERIA = 0x0E
    NAME_KEY_ITEM = 0x0F
    BATTLE_MESSAGES = 0x10
    NAME_SUMMON = 0x11


# mirrors BattleMessageArgType in battle.h
class BattleMsgArg(enum.IntEnum):
    CHAR_NAME = 0xEA
    ITEM_NAME = 0xEB
    NUMBER = 0xEC
    UNIT_NAME = 0xED
    MAGIC_NAME = 0xEE
    ENEMY_LETTER = 0xEF
    BATTLE_TEXT = 0xF0
    KERNEL_TEXT = 0xF1
    START = CHAR_NAME
    END = KERNEL_TEXT


# DESC_COMMAND's entry 0 is 0x3F, which points inside its own table, so its
# count can't be derived from the first entry like the other sections
COUNT_OVERRIDES = {
    KernelTextSections.DESC_COMMAND: 32,
}


# code + 1 operand (from SysMenuDrawString)
TWO_BYTE_CODES = {0xF8, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE}


def decompress_text_sections(kernel: bytes) -> list[bytes]:
    text_sections = []
    offset = 0
    index = 0

    while offset + SECTION_HEADER.size <= len(kernel):
        packed_size, unpacked_size, section_type = SECTION_HEADER.unpack_from(
            kernel, offset)
        offset += SECTION_HEADER.size

        if offset + packed_size > len(kernel):
            raise ValueError(
                f'section {index}: packed_size exceeds kernel length')

        if section_type == TEXT_SECTION_TYPE:
            span = kernel[offset:offset + packed_size]
            data = gzip.decompress(span)
            if len(data) != unpacked_size:
                raise ValueError(
                    f'section {index}: inflated to {len(data)} bytes, header says {unpacked_size}')
            text_sections.append(data)

        offset += packed_size
        index += 1

    return text_sections


def read_offsets(section: bytes, count: int | None = None) -> tuple[int, ...]:
    if count is None:
        (table_size,) = struct.unpack_from('<H', section, 0)

        if table_size == 0 or table_size > len(section):
            raise ValueError(
                f'table size {table_size:#x} invalid for section of {len(section):#x} bytes')

        # first string starts right after the table
        count = table_size // 2  # each offset entry is 2 bytes

    return struct.unpack_from(f'<{count}H', section, 0)


def decode_str(section: bytes, offset: int) -> str:
    result = ""
    i = offset

    while section[i] != TEXT_ENCODING_TERMINATOR:
        c = section[i]

        if c == TEXT_ENCODING_BACKREF:  # simplified LZ77 encoding scheme
            ref = section[i + 1]
            length = (ref >> 6) * 2 + 4  # {4, 6, 8, 10} bytes to copy
            distance = (ref & 0x3F) + 1  # 1~64 byte look-back

            for j in range(length):
                c = section[i - distance + j]
                if c == TEXT_ENCODING_TERMINATOR:  # backref can decode str terminators, so return early
                    return result
                result += decode_en(c)
            i += 2
            continue

        if BattleMsgArg.START <= c <= BattleMsgArg.END:  # placeholder: 2 parameter bytes follow
            result += f'{{{BattleMsgArg(c).name}}}'
            i += 3
        elif c in TWO_BYTE_CODES:
            i += 2
        else:  # ordinary encoding
            result += decode_en(c)
            i += 1

    return result


def decode_en(c: int) -> str:
    return chr(c + 0x20) if c < 0x60 else f'{{{c:02X}}}'


def dump_strings(section: bytes, offsets: tuple[int, ...], ids: list[int]):
    table_size = len(offsets) * 2

    for msg_id in ids:
        if msg_id >= len(offsets):
            print(
                f'{msg_id:#04x}  <no such entry, section has {len(offsets)}>', file=sys.stderr)

    for msg_id, offset in enumerate(offsets):
        if ids and msg_id not in ids:
            continue

        if not table_size <= offset < len(section):
            text = f'<bad offset {offset:#x}>'
        else:
            text = decode_str(section, offset)

        if text or ids:
            print(f'{msg_id:#04x}  {text or "<empty>"}')


def dump_section_strings(section_id: KernelTextSections, text_sections: list[bytes], ids: list[int]):
    section = text_sections[section_id]
    offsets = read_offsets(section, COUNT_OVERRIDES.get(section_id))

    dump_strings(section, offsets, ids)


def dump_all_section_strings(text_sections: list[bytes]):
    for section_id in KernelTextSections:
        print(f'--- {section_id.name}')
        try:
            dump_section_strings(section_id, text_sections, [])
        except ValueError as e:
            print(f'failed to read section {section_id.name}\n{e}')


def section_name(name: str) -> KernelTextSections:
    try:
        return KernelTextSections[name.upper()]
    except KeyError:
        names = ', '.join(s.name.lower() for s in KernelTextSections)
        raise argparse.ArgumentTypeError(
            f'unknown section {name!r} (choose from {names})')


def entry_id(text: str) -> int:
    return int(text, 0)  # accepts 82, 0x52


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description='Dump text from FF7 KERNEL.BIN (US/PAL encoding).')
    parser.add_argument('-k', '--kernel', type=Path, default=DEFAULT_KERNEL,
                        help='path to KERNEL.BIN (default: disks/us/INIT/KERNEL.BIN)')
    parser.add_argument('-s', '--section', type=section_name,
                        help='section to dump, e.g. battle_messages (default: all)')
    parser.add_argument('ids', nargs='*', type=entry_id,
                        help='entry IDs to print, e.g. 0x52 (default: all)')

    args = parser.parse_args()
    if args.ids and args.section is None:
        parser.error('IDs need --section')

    return args


def main() -> None:
    args = parse_args()

    kernel = Path(args.kernel).read_bytes()
    text_sections = decompress_text_sections(kernel)

    if args.section is None:
        dump_all_section_strings(text_sections)
    else:
        dump_section_strings(args.section, text_sections, args.ids)


if __name__ == "__main__":
    sys.exit(main())
