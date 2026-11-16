"""Make the SET files that use SADX Object Ports' objects.

  python tools/make_sadx_ports_set.py [--sadx "<SADX folder>"]

Reads the mod's SET files (dist/stage13/set0013_s.bin, set0013_2p_s.bin) and
SADX's Emerald Coast SET files (system/SET0100S.BIN, SET0102B.BIN,
SET0101S.BIN; act 2 moved 2000 down in Z, as SetConv did), and writes
dist/stage13_sadx/set0013_s.bin and set0013_2p_s.bin:

- The SA2 stand-ins SetConv put in for SADX objects (ramp, WIND2, swinging
  spike ball, E SARU, E EMI, Beetle, dash panel) become the ported SADX objects again:
  SADX's rotation and scale (its object parameters), the mod's position (a
  few were moved by hand, e.g. a Rhinotank lowered by 8).
- SADX objects SetConv dropped for lack of an SA2 equivalent are added from
  SADX's files: O GOMBAN (elastic railings), O BGIRO (spike gates),
  O JPanel (jump panels), the orca triggers (O AO SUMMON, O AO KILL,
  O PO SUMMON). The 2P file also gets the O BEWIND updrafts (its
  older conversion has none).
- The rings SetConv made from SADX's ring groups are made again with SADX's
  formula (fix_ring_groups: SetConv's lines were mirrored and unevenly
  spaced, and its circles had a ring twice).

The object IDs (102+) are the entries LevelTest.cpp appends to City
Escape's object list (kSadxPorts); keep the two in step. The game loads these
files instead of dist/stage13's when SADX Object Ports is installed.
"""
import argparse
import math
import os
import re
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DIST = os.path.join(HERE, '..', 'dist')
STEAM = r'C:\Program Files (x86)\Steam\steamapps\common\Sonic Adventure DX'

# SADX Emerald Coast object list ID -> ID in the extended City Escape list
# (LevelTest.cpp kSadxPorts, same order).
NEW_ID = {
    40: 102,  # O JUMP
    79: 103,  # O BEWIND
    60: 104,  # O GOMBAN
    38: 105,  # O BGIRO
    4: 106,   # O IRONB
    18: 107,  # O JPanel
    29: 108,  # E SARU (Kiki)
    30: 109,  # E SAITO (Rhinotank)
    31: 110,  # E AMEMB (Sweep)
    3: 111,   # O AXPNL (dash panel)
    67: 112,  # O AO SUMMON (the chasing orca)
    68: 113,  # O AO KILL (sends it away)
    69: 114,  # O PO SUMMON (the path orca)
    59: 115,  # O PIER (was baked into the land)
    66: 116,  # O BIGROCK (was baked into the land)
    53: 117,  # O DOLPHIN
    75: 118,  # O DOLSW (the dolphins' switch)
    55: 119,  # O KAMOME (seagull)
    56: 120,  # O KAMOMEL (seagull)
    25: 121,  # OTTOTTO (teeter edge)
    61: 122,  # O ASIATO (footprints)
    27: 123,  # O BUBBLE
    28: 124,  # O BUBBLES
    20: 125,  # WALL (SADX's invisible wall, which pushes players out)
    58: 126,  # O PARASOL (breakable parasols, chairs, tables; were land)
    39: 127,  # O IWA (bobbing rocks; were land)
}
# SA2 stand-in ID -> the SADX IDs SetConv turned into it.
STAND_INS = {
    6: (40,),       # ramp <- O JUMP
    53: (79,),      # WIND2 <- O BEWIND
    55: (4,),       # swinging spike ball <- O IRONB
    56: (31, 29),   # Beetle <- E AMEMB (and E SARU in the 2P file)
    57: (30,),      # E EMI <- E SAITO
    91: (29,),      # E SARU <- E SARU
    7: (3,),        # dash panel <- O AXPNL
    49: (20,),      # SA2's wall <- WALL
}
DROPPED = (60, 38, 18, 67, 68, 69, 59, 66, 53, 75, 55, 56, 25, 61, 27, 28, 58, 39)  # added from SADX
ADD_2P = (79,)          # missing from the 2P file's conversion
BIG_SKIPPED = (61, 27, 28)  # O ASIATO / O BUBBLE / O BUBBLES of Big's act (SET0102B)


def read_sadx(path, dz=0.0):
    d = open(path, 'rb').read()
    out = []
    for i in range(struct.unpack_from('<I', d, 0)[0]):
        raw, rx, ry, rz, px, py, pz, sx, sy, sz = struct.unpack_from('<H3H6f', d, 0x20 + i * 0x20)
        out.append({'id': raw & 0xFFF, 'flags': raw >> 12, 'rot': (rx, ry, rz),
                    'pos': (px, py, pz + dz), 'scl': (sx, sy, sz)})
    return out


def read_sa2(path):
    d = open(path, 'rb').read()
    out = []
    for i in range(struct.unpack_from('>I', d, 0)[0]):
        raw, rx, ry, rz, px, py, pz, sx, sy, sz = struct.unpack_from('>H3H6f', d, 0x20 + i * 0x20)
        out.append({'id': raw & 0xFFF, 'flags': raw >> 12, 'rot': (rx, ry, rz),
                    'pos': (px, py, pz), 'scl': (sx, sy, sz)})
    return out


def write_sa2(path, entries):
    data = bytearray(0x20)
    struct.pack_into('>I', data, 0, len(entries))
    for e in entries:
        data += struct.pack('>H3H6f', e['id'] | (e['flags'] << 12), *e['rot'], *e['pos'], *e['scl'])
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, 'wb').write(data)


def source_of(e, sadx):
    """The SADX entry a stand-in came from: same position, or (moved by hand)
    the nearest one of the right type within 1 unit in X / Z."""
    ids = STAND_INS[e['id']]
    best, best_d = None, None
    for s in sadx:
        if s['id'] not in ids:
            continue
        dx, dy, dz = (s['pos'][k] - e['pos'][k] for k in range(3))
        if abs(dx) > 1 or abs(dz) > 1:
            continue
        d = dx * dx + dy * dy + dz * dz
        if best is None or d < best_d:
            best, best_d = s, d
    return best


def convert(base, sadx, extra_ids):
    out, replaced = [], {}
    for e in base:
        if e['id'] not in STAND_INS:
            out.append(e)
            continue
        s = source_of(e, sadx)
        if not s:
            raise SystemExit('no SADX source for SA2 ID %d at %s' % (e['id'], e['pos']))
        out.append({'id': NEW_ID[s['id']], 'flags': e['flags'], 'rot': s['rot'],
                    'pos': e['pos'], 'scl': s['scl']})
        replaced[s['id']] = replaced.get(s['id'], 0) + 1
    added = {}
    for s in sadx:
        if s['id'] in extra_ids and not (s.get('big') and s['id'] in BIG_SKIPPED):
            # SADX's clip-level bits (1, 2) aren't SA2's; keep bit 8 as SetConv did.
            out.append({'id': NEW_ID[s['id']], 'flags': s['flags'] & 8, 'rot': s['rot'],
                        'pos': s['pos'], 'scl': s['scl']})
            added[s['id']] = added.get(s['id'], 0) + 1
    return out, replaced, added


# ---- ring groups (O_GRING, SADX ID 0x15) ------------------------------------
# SetConv turned each ring group into rings with its own formula, which
# differs from SADX's (RingGroup_Main 0x79B2F0, CalcRingPosLine 0x79B140,
# CalcRingPosCircle 0x79B210; the decomp's src/object/ring_group.c):
#   SADX line: rings 0..scl.x; ring i odd at +scl.y * ceil(i / 2) along the
#     group's z, even at -scl.y * i / 2. SetConv swapped the two and left out
#     the ceil (ring 1 half a step back instead of a step on).
#   SADX circle: rings 1..scl.x at i * 360 / scl.x degrees, scl.y out.
#     SetConv made rings 0..scl.x, so ring 0 and ring scl.x share a place.
#   SetConv also stopped at 8 rings; SADX doesn't.
# Both turn the offset by the group's angles as njRotateZ, X, Y (the vector
# turned by y first).
GROUP_ID = 0x15
RING_ID = 0


def _ang(a):
    return (a & 0xFFFF) * 2.0 * math.pi / 65536.0


def _turn(v, rot):
    x, y, z = v
    a = _ang(rot[1]); c, s = math.cos(a), math.sin(a); x, z = x * c + z * s, -x * s + z * c
    a = _ang(rot[0]); c, s = math.cos(a), math.sin(a); y, z = y * c - z * s, y * s + z * c
    a = _ang(rot[2]); c, s = math.cos(a), math.sin(a); x, y = x * c - y * s, x * s + y * c
    return x, y, z


def _place(g, offsets):
    return [tuple(g['pos'][k] + p[k] for k in range(3)) for p in (_turn(v, g['rot']) for v in offsets)]


def setconv_group(g):
    """SetConv's rings for a group (SetConv/Program.cs, case 0x15)."""
    last, space, pattern = g['scl']
    offsets = []
    for i in range(int(min(last + 1, 8))):
        if pattern == 1:
            a = int(i * 360.0 / last * 65536.0 * 0.002777777777777778)
            offsets.append((math.sin(_ang(a)) * space, 0.0, math.cos(_ang(a)) * space))
        else:
            offsets.append((0.0, 0.0, i * space * -0.5 if i % 2 == 1 else math.ceil(i * 0.5) * space))
    return _place(g, offsets)


def sadx_group(g):
    """SADX's rings for a group (CalcRingPosLine / CalcRingPosCircle)."""
    last, space, pattern = g['scl']
    count = int(last)  # __ftol2
    offsets = []
    for i in range(count + 1):
        if pattern == 0:
            z = space * math.ceil(i * 0.5) if i % 2 else -(space * i * 0.5)
            offsets.append((0.0, 0.0, z))
        elif pattern == 1 and i:
            a = int(i * 360.0 / last * 65536.0 / 360.0)  # (Angle) ftol
            offsets.append((space * math.sin(_ang(a)), 0.0, space * math.cos(_ang(a))))
    return _place(g, offsets)


def fix_ring_groups(entries, sadx):
    """Each group whose SetConv rings are in the file gets SADX's rings in
    their place. Groups SetConv left out (most of Big's act) stay out."""
    rings = [i for i, e in enumerate(entries) if e['id'] == RING_ID]
    drop, add, fixed = set(), [], 0
    for g in (s for s in sadx if s['id'] == GROUP_ID):
        found = []
        for p in setconv_group(g):
            for i in rings:
                if i not in drop and i not in found and math.dist(entries[i]['pos'], p) < 0.05:
                    found.append(i)
        if not found:
            continue
        drop.update(found)
        fixed += 1
        for p in sadx_group(g):
            add.append({'id': RING_ID, 'flags': entries[found[0]]['flags'], 'rot': (0, 0, 0), 'pos': p,
                        'scl': (0.0, 0.0, 0.0)})
    out = [e for i, e in enumerate(entries) if i not in drop] + add
    return out, fixed, len(drop), len(add)


# ---- land entries the ported objects replace ---------------------------------
# SetConv baked O PIER (drawn only) and O BIGROCK (drawn and solid) into the
# land, and MainMemory's BridgeSegment0..3 are the pier's floor. With SADX
# Object Ports, O PIER brings its own deck (col_pier) and O BIGROCK its
# capsule, and both break as the orca reaches them, so LevelTest.cpp clears
# these entries' flags (nothing drawn, nothing solid) while it uses them.
BAKED = re.compile(r'^O (PIER|BIGROCK|PARASOL|IWA) \d+(_cnk)?$')
BRIDGE_SEGMENTS = ['BridgeSegment%d.sa1mdl' % i for i in range(4)]
FLOATING_BRIDGE = ['Bridge Model.sa1mdl', 'Bridge Model COL.sa1mdl']


def sa2lvl_cols(path):
    """(index, centre, model label) of each COL in an SA2 .sa2lvl."""
    d = open(path, 'rb').read()
    land, meta = struct.unpack_from('<II', d, 8)
    labels = {}
    p = meta
    while p + 8 <= len(d):
        kind, size = struct.unpack_from('<4sI', d, p)
        if kind == b'END\0':
            break
        if kind == b'LABL':
            q = p + 8
            while True:
                addr, off = struct.unpack_from('<iI', d, q)
                if addr == -1:
                    break
                labels[addr] = d[p + 8 + off:d.index(b'\0', p + 8 + off)].decode('ascii', 'replace')
                q += 8
        p += 8 + size
    count = struct.unpack_from('<h', d, land)[0]
    cols = struct.unpack_from('<I', d, land + 16)[0]
    out = []
    for i in range(count):
        cx, cy, cz, r, model = struct.unpack_from('<4fI', d, cols + i * 0x20)
        out.append((i, (cx, cy, cz), labels.get(model, '')))
    return out


def first_object_label(path):
    d = open(path, 'rb').read()
    i = d.find(b'object_')
    return d[i:d.index(b'\0', i)].decode('ascii')


def write_baked_cols(header):
    floors = {first_object_label(os.path.join(HERE, '..', f)) for f in BRIDGE_SEGMENTS}
    rows = [(i, c, name) for i, c, name in sa2lvl_cols(os.path.join(DIST, 'LandTable.sa2lvl'))
            if BAKED.match(name) or name in floors]
    with open(header, 'w', newline='\r\n') as f:
        f.write('// Made by tools/make_sadx_ports_set.py: the land entries SADX Object Ports\n'
                '// replaces (O PIER, O BIGROCK, the pier floor BridgeSegment0..3). Index\n'
                '// in dist/LandTable.sa2lvl, and its centre (checked before clearing).\n'
                '#pragma once\n\n'
                'struct SadxPortsBakedCol { int index; float x, y, z; };\n\n'
                'static const SadxPortsBakedCol kSadxPortsBakedCols[] = {\n')
        for i, (x, y, z), name in rows:
            f.write('\t{ %d, %.3ff, %.3ff, %.3ff }, // %s\n' % (i, x, y, z, name))
        f.write('};\n')
        # The floating bridge, fixed in the land (SetConv's Bridge Model / COL):
        # SADX Object Ports API 8 brings SADX's, riding the sea.
        bridge = {first_object_label(os.path.join(HERE, '..', f)) for f in FLOATING_BRIDGE}
        brows = [(i, c, name) for i, c, name in sa2lvl_cols(os.path.join(DIST, 'LandTable.sa2lvl'))
                 if name.replace('_cnk', '') in bridge]
        f.write('\n// The floating bridge (Bridge Model, Bridge Model COL): hidden with API 8.\n'
                'static const SadxPortsBakedCol kSadxPortsBridgeCols[] = {\n')
        for i, (x, y, z), name in brows:
            f.write('\t{ %d, %.3ff, %.3ff, %.3ff }, // %s\n' % (i, x, y, z, name))
        f.write('};\n')
    return len(rows) + len(brows)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--sadx', default=STEAM)
    args = ap.parse_args()
    system = os.path.join(args.sadx, 'system')
    big = read_sadx(os.path.join(system, 'SET0102B.BIN'))
    for s in big:
        s['big'] = True  # Big's act: its footprints and bubbles aren't in the port's land
    sadx = (read_sadx(os.path.join(system, 'SET0100S.BIN')) + big +
            read_sadx(os.path.join(system, 'SET0101S.BIN'), -2000.0))
    for name, extra in (('set0013_s.bin', DROPPED), ('set0013_2p_s.bin', DROPPED + ADD_2P)):
        base = read_sa2(os.path.join(DIST, 'stage13', name))
        out, replaced, added = convert(base, sadx, extra)
        out, groups, dropped, made = fix_ring_groups(out, sadx)
        write_sa2(os.path.join(DIST, 'stage13_sadx', name), out)
        print('%s: %d -> %d entries; replaced %s; added %s; %d ring groups: %d SetConv rings -> %d SADX rings' % (
            name, len(base), len(out), replaced, added, groups, dropped, made))
    n = write_baked_cols(os.path.join(HERE, '..', 'LevelTest', 'SadxPortsCols.h'))
    print('LevelTest/SadxPortsCols.h: %d land entries' % n)


if __name__ == '__main__':
    main()
