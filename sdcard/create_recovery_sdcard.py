#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2017 Oran Avraham <contact@oranav.me>
#
# Distributed under terms of the GPLv3 license.
import argparse


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Create sboot USB mode SD card')
    parser.add_argument('--output', '-o', type=argparse.FileType('wb'),
                        required=True, help='output file to create')
    parser.add_argument('--sboot', '-s', type=argparse.FileType('rb'),
                        required=True, help='sboot.bin (from XXELLA)')
    args = parser.parse_args()

    args.output.write(open('header.bin','rb').read())
    args.output.write(open('bl1.HardKernel','rb').read())
    args.output.write(open('bl2.HardKernel','rb').read())
    args.output.write(b'\x00'*(0x7e00-args.output.tell()))
    args.output.write(args.sboot.read()[0x6000:])
    args.output.write(b'\xff'*(0x100000-args.output.tell()))
    args.output.write(b'\x00'*(0x107e00-args.output.tell()))
    args.output.write(open('tzsw.HardKernel','rb').read())
    args.output.write(b'\x00'*(0x400000-args.output.tell()))
    args.output.close()
