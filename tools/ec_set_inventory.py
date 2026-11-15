"""Every object in SADX's Emerald Coast SET files, and what the SA2 port does with it.

  python tools/ec_set_inventory.py [--sadx "<SADX folder>"] [--lists <object_lists.json>] [--md out.md]

Reads the SET files SetConv combined (system/SET0100S.BIN act 1, SET0102B.BIN
act 3 (Big), SET0101S.BIN act 2, Z - 2000) and SADX's Emerald Coast object
list (one list for all three acts; names from SA2_1P_in_2P
docs/sadx_objects/object_lists.json), and sorts each object ID by its fate:

  ported     a SADX Object Ports object (make_sadx_ports_set.py NEW_ID)
  SA2        replaced by an SA2 object (SetConv's idmap / special cases)
  geometry   baked into the level's land table by SetConv (solid or not)
  palm tree  drawn by the mod's PalmtreeMan
  missing    dropped: neither ported nor converted

Prints a table (ID, name, count per act, fate) and the missing objects with
every placement, so they can be ported and added to the SET.
"""
import argparse
import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import make_sadx_ports_set as ports  # noqa: E402  (NEW_ID: the ported objects)

STEAM = r'C:\Program Files (x86)\Steam\steamapps\common\Sonic Adventure DX'
LISTS = os.path.join(HERE, '..', '..', '..', '..', 'docs', 'sadx_objects', 'object_lists.json')
FILES = (('act 1', 'SET0100S.BIN', 0.0), ('act 3', 'SET0102B.BIN', 0.0),
         ('act 2', 'SET0101S.BIN', -2000.0))

# SetConv/Program.cs, Main: what it does with each SADX ID.
SA2_MAP = {0: 'ring', 1: 'spring A', 2: 'spring B', 3: 'dash panel', 4: 'swinging spike ball',
           7: 'emerald', 8: 'goal', 9: 'switch', 0xD: 'item box', 0x13: 'checkpoint',
           0x14: 'wall', 0x15: 'rings (ring group expanded)', 0x16: 'collision sphere',
           0x17: 'collision cylinder', 0x18: 'collision cube', 0x1A: 'Omochao (hint)',
           0x1D: 'E SARU', 0x1E: 'E EMI', 0x1F: 'Beetle', 0x28: 'ramp', 0x4F: 'WIND2',
           0x50: 'goal (froggy)', 0x51: 'goal', 0x52: 'item box (air)'}
PALM_TREES = (32, 33, 34, 35)
GEOMETRY_NONSOLID = (47, 48, 49, 50, 51, 52, 59, 62, 63, 64, 70)
GEOMETRY_SOLID = (36, 37, 39, 41, 42, 43, 44, 45, 46, 54, 58, 66, 71, 72, 73, 74)


def fate(oid):
    if oid in ports.NEW_ID:
        return 'ported'
    if oid in SA2_MAP:
        return 'SA2: ' + SA2_MAP[oid]
    if oid in PALM_TREES:
        return 'palm tree'
    if oid in GEOMETRY_SOLID:
        return 'geometry (solid)'
    if oid in GEOMETRY_NONSOLID:
        return 'geometry'
    return 'missing'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--sadx', default=STEAM)
    ap.add_argument('--lists', default=LISTS)
    ap.add_argument('--md', help='also write the tables as markdown')
    args = ap.parse_args()

    names = {}
    for t in json.load(open(args.lists)):
        if 'EmeraldCoast 1' in t['used_by']:
            names = {it['id']: it['name'].strip() for it in t['items']}
    by_id = {}
    for act, fname, dz in FILES:
        for e in ports.read_sadx(os.path.join(args.sadx, 'system', fname), dz):
            by_id.setdefault(e['id'], []).append((act, e))

    lines = ['| ID | Name | Act 1 | Act 2 | Act 3 (Big) | In the SA2 port |', '|---|---|---|---|---|---|']
    for oid in sorted(by_id):
        acts = [a for a, _ in by_id[oid]]
        lines.append('| %d | %s | %d | %d | %d | %s |' % (
            oid, names.get(oid, '?'), acts.count('act 1'), acts.count('act 2'),
            acts.count('act 3'), fate(oid)))
    missing = [oid for oid in sorted(by_id) if fate(oid) == 'missing']
    lines += ['', '## Missing objects: every placement', '']
    for oid in missing:
        lines.append('### %d %s (%d)' % (oid, names.get(oid, '?'), len(by_id[oid])))
        lines.append('')
        lines.append('| Act | Position | Rotation | Scale (parameters) |')
        lines.append('|---|---|---|---|')
        for act, e in by_id[oid]:
            lines.append('| %s | (%.1f, %.1f, %.1f) | (0x%04X, 0x%04X, 0x%04X) | (%g, %g, %g) |' % (
                act, *e['pos'], *e['rot'], *e['scl']))
        lines.append('')
    text = '\n'.join(lines)
    print(text)
    if args.md:
        with open(args.md, 'w', encoding='utf-8') as f:
            f.write(text + '\n')


if __name__ == '__main__':
    main()
