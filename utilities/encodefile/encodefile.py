#!/usr/bin/env python3
import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=argparse.FileType())
    parser.add_argument("output", type=argparse.FileType("w"))
    parser.add_argument("name", nargs="?")

    args = parser.parse_args()
    with args.input as i, args.output as o:
        o.write(f"const char *{args.name or 'inputvariable'} =\n")
        for line in i:
            o.write(f'  "{line.rstrip()}\\n"\n')
        o.write('  "\\n";\n')


if __name__ == "__main__":
    main()
