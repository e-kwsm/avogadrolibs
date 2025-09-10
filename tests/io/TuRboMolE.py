#!/usr/bin/env -S -i python3 -I -S
import hashlib
import itertools


def hoge(
    *,
    periodic: int,
    q: int | None = None,
    is_periodic_first: bool = True,
    is_cell: bool = True,
    is_angs: bool = True,
    A: float = 6.0,
    B: float = 8.0,
    C: float = 10.0,
    ALPHA: float = 90.0,
    BETA: float = 90.0,
    GAMMA: float = 60.0,
    coord: str = "",
) -> str:
    """
    periodic: periodic dimensionality
    q: specifies $cell/$lattice dimensionality, defaults to periodic
    is_periodic_first: $periodic appears first if true
    is_cell: $cell if true, otherwise $lattice
    is_angs: append angs (Å) to $cell/$lattice if specified, otherwise none (bohr)
    """
    CELLS = {1: [A], 2: [A, B, GAMMA], 3: [A, B, C, ALPHA, BETA, GAMMA]}

    VECTORS = {
        1: [[A]],
        2: [[A, 0.0], [0.0, B]],
        3: [[A, 0.0, 0.0], [0.0, B, 0.0], [0.0, 0.0, C]],
    }

    LATTICES = {
        1: VECTORS[1],
        2: VECTORS[2],
        3: VECTORS[3],
    }

    if q is None:
        q = periodic

    s = ""
    if is_periodic_first:
        s += f"$periodic {periodic}\n"

    if is_cell:
        s += "$cell" + (" angs" if is_angs else "") + "\n"
        s += " ".join(map(str, CELLS[q]))
    else:
        s += "$lattice" + (" angs" if is_angs else "") + "\n"
        s += "\n".join(" ".join(map(str, v)) for v in LATTICES[q])

    s += "\n" + coord.strip()

    if not is_periodic_first:
        s += f"\n$periodic {periodic}"
    s += "\n$end\n"
    if periodic == q:
        s += "# valid\n"
    return s


def main():
    for periodic, q, cell, angs in itertools.product(
        range(1, 4),
        range(1, 4),
        [True, False],
        [True, False],
    ):
        s = hoge(periodic=periodic, q=q, is_cell=cell, is_angs=angs, coord="""$coord
3.0 5.0 7.0 fr""")
        h = hashlib.md5(s.encode()).hexdigest()
        with open(f"{h}.coord", "w") as f:
            f.write(s)


if __name__ == "__main__":
    main()
