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
  O JPanel (jump panels). The 2P file also gets the O BEWIND updrafts (its
  older conversion has none).

The object IDs (102+) are the entries LevelTest.cpp appends to City
Escape's object list (kSadxPorts); keep the two in step. The game loads these
files instead of dist/stage13's when SADX Object Ports is installed.
"""
import argparse
import os
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
}
DROPPED = (60, 38, 18)  # added from SADX
ADD_2P = (79,)          # missing from the 2P file's conversion


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
        if s['id'] in extra_ids:
            # SADX's clip-level bits (1, 2) aren't SA2's; keep bit 8 as SetConv did.
            out.append({'id': NEW_ID[s['id']], 'flags': s['flags'] & 8, 'rot': s['rot'],
                        'pos': s['pos'], 'scl': s['scl']})
            added[s['id']] = added.get(s['id'], 0) + 1
    return out, replaced, added


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--sadx', default=STEAM)
    args = ap.parse_args()
    system = os.path.join(args.sadx, 'system')
    sadx = (read_sadx(os.path.join(system, 'SET0100S.BIN')) +
            read_sadx(os.path.join(system, 'SET0102B.BIN')) +
            read_sadx(os.path.join(system, 'SET0101S.BIN'), -2000.0))
    for name, extra in (('set0013_s.bin', DROPPED), ('set0013_2p_s.bin', DROPPED + ADD_2P)):
        base = read_sa2(os.path.join(DIST, 'stage13', name))
        out, replaced, added = convert(base, sadx, extra)
        write_sa2(os.path.join(DIST, 'stage13_sadx', name), out)
        print('%s: %d -> %d entries; replaced %s; added %s' % (name, len(base), len(out), replaced, added))


if __name__ == '__main__':
    main()
