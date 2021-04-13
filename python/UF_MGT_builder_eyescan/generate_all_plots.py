# -*- coding: utf-8 -*-
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
from glob import glob
import os.path
import numpy as np

minlog10ber = -12
overwrite = True

filename_i_list = glob('scans/csv/*')
filename_o_list = [p.replace('csv','pdf') for p in filename_i_list]

yticks = range(-127,0,16)+[0]+range(127,0,-16)[-1::-1]
xticks = list(np.arange(-0.5,0.625,0.125))
k=1
#if os.path.exists('..\scans\eyedata.csv'):
#    pass
#else
#    eyedict

for i,o in zip(filename_i_list, filename_o_list):
    print('Saving file {0:03d} out of {1:d}.'.format(k,len(filename_i_list)))
    if (not os.path.exists(o)) or overwrite:
        eyescan_plot(i, o, minlog10ber, colorbar=True, xaxis=True, yaxis=True, xticks_f=xticks, yticks_f=yticks, mask_x1x2x3y1y2=(0.25, 0.4, 0.45, 0.25, 0.28))
    k += 1
    #break
