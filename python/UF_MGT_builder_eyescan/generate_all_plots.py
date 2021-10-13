#!/usr/bin/python
"""
Original work Copyright (C) 2017 msilvaol (MIT License)
https://github.com/mvsoliveira/IBERTpy

Modified work Copyright (C) 2021 Aleksei Greshilov
aleksei.greshilov@cern.ch

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""
from eyescan_plot import eyescan_plot
from eyescan_plot import get_eye
from glob import glob
import os.path
import numpy as np
import argparse
import csv

parser = argparse.ArgumentParser(description='UF MGT builder eyescan tool.')
parser.add_argument('scale', metavar='BER scale', type=int, nargs='?', help='BER scale (from -6 to -15). Set to -7 by default', default=-7)
args = parser.parse_args()
minlog10ber = args.scale

overwrite = True

filename_i_list = glob('../../scans/csv/*')
filename_o_list = [p.replace('csv','png') for p in filename_i_list]

yticks = range(-127,0,16)+[0]+range(127,0,-16)[-1::-1]
xticks = list(np.arange(-0.5,0.625,0.125))

k=1

for i,o in zip(filename_i_list, filename_o_list):
    print('Saving file {0:03d} out of {1:d}.'.format(k,len(filename_i_list)))

    bathtub = False

    # opens the file
    with open(i, 'rb') as f:
            reader = csv.reader(f)
            scan_list = list(reader)

    # getting bathtub status
    bathtub = get_eye(scan_list)[6]
    f.close()

    if (not os.path.exists(o)) or overwrite:
        if bathtub:
            eyescan_plot(i, o, minlog10ber, colorbar=False, xaxis=False, yaxis=False, xticks_f=xticks, yticks_f=[])
        else:
            eyescan_plot(i, o, minlog10ber, colorbar=True, xaxis=True, yaxis=True, xticks_f=xticks, yticks_f=yticks)

    k += 1
    #break
