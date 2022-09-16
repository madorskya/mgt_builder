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
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import csv
import operator as op
from matplotlib.colors import ListedColormap
import matplotlib.pylab as pl
import matplotlib as mpl
from matplotlib.path import Path
import matplotlib.patches as patches
import os

# function for getting eye data
def get_eye(scan_list, gt):
    eyedata = False
    bathtub = False
    yticks = []
    img = []
    header = "N/A"
    date = "N/A"
    time = "N/A"

    for row in scan_list:
        if row[0].startswith('Date and Time Started'):
            dt = row[1]
            date = dt.split(' ')[0]
            time = dt.split(' ')[1]

        if row[0].startswith('Scan Name'):
            header = row[1]

        if row[0].startswith('Scan End'):
            eyedata = False

        if eyedata:
            yticks.append(row[0])
            img.append(row[1:])

        if row[0].startswith('2d statistical'):
            xticks = row[1:]
            eyedata = True

        if row[0].startswith('1d bathtub'):
            xticks = row[1:]
            eyedata = True

    if gt:	
    	for elm in img:
		if elm[0] < elm[1]:
			for val in elm:
				if elm.index(val) == 1:
					elm[1]=elm[0]
				elif elm.index(val)%2 == 1:
					elm[elm.index(val)] = elm[elm.index(val)-1]
		if elm[0] > elm[1]:
			for val in elm: 
				if elm.index(val) != 1 and elm.index(val) !=len(elm)-1 and elm.index(val)%2 == 0:
					elm[elm.index(val)] = elm[elm.index(val)+1]
				elif elm.index(val) == len(elm)-1:
					elm[elm.index(val)]=elm[elm.index(val)-1]

    img = [[float(y) for y in x] for x in img]

    xticks = [int(x) for x in xticks]
    yticks = [int(y) for y in yticks]

    if len(yticks) == 1:
        bathtub = True
        img = [float(x) for x in img[0]]

    return [img, xticks, yticks, date, time, header, bathtub]


#Generate eyescan plots
def eyescan_plot(filename_i, filename_o, minlog10ber, gt, colorbar=True, xaxis=True, yaxis=True, xticks_f=[],yticks_f=[]):

    # opens the file
    with open(filename_i, 'rb') as f:
            reader = csv.reader(f)
            scan_list = list(reader)


    # getting eye data
    [img, xticks, yticks, date, time, header, bathtub] = get_eye(scan_list, gt)

    # function for calculating x-y axis ranges in a such way that ticks is in the center of each entry
    def get_extent(xticks_n,yticks_r):
        xmin = xticks_n[0]
        xmax = xticks_n[-1]
        xstep = (xmax-xmin)/(len(xticks_n)-1)
        xmin_e = xmin-xstep/2
        xmax_e = xmax+xstep/2
        ymin = yticks_r[0]
        ymax = yticks_r[-1]
        ystep = (ymax-ymin)/(len(yticks_r)-1)
        ymin_e = ymin-ystep/2
        ymax_e = ymax+ystep/2
        return [xmin_e, xmax_e, ymin_e, ymax_e]


    # Generating, formating plot
    verts = [
        (-0.118, 0.),
        (0., 30.5),
        (0.118, 0.),
        (0., -30.5),
        (-0.118, 0.),
    ]

    if gt:
    	verts = [
        	(-0.118, 0.),
        	(0., 45.5),
        	(0.118, 0.),
        	(0., -45.5),
        	(-0.118, 0.),
    	]

    codes = [
        Path.MOVETO,
        Path.LINETO,
        Path.LINETO,
        Path.LINETO,
        Path.CLOSEPOLY,
    ]

    path = Path(verts, codes)

    fig, ax = plt.subplots(num=None, figsize=(10, 7), dpi=80, facecolor='w', edgecolor='k')
    xticks_n = [float(x)/(2*xticks[-1]) for x in xticks]
    yticks_r = [y for y in reversed(yticks)]

    if bathtub:
        yticks_r = np.log10(img)
        ax.plot(xticks_n, yticks_r)
    else:
        myplot = plt.imshow(np.log10(img),interpolation='none', vmin = minlog10ber, vmax = 0, aspect='auto', extent = get_extent(xticks_n,yticks_r), cmap = 'jet')
        patch = patches.PathPatch(path, facecolor='none', lw=2)
        ax.add_patch(patch)
        if xaxis:
            if not yticks:
                plt.xticks(xticks_n)
            else:
                plt.xticks(xticks_f)
        if yaxis:
            if not yticks:
                plt.yticks(yticks_r)
            else:
                plt.yticks(yticks_f)
        else:
            plt.yticks([])

    # plot title
    base = os.path.basename(filename_i)
    name = os.path.splitext(base)[0]
    device = "N/A"
    GTx = "N/A"
    GTy = "N/A"
    while True:
        try:
            name_splt = name.split('_')
            if len(name_splt) > 3:
                device = name_splt[1]
                GTx = name_splt[2]
                GTy = name_splt[3]
            break
        except ValueError:
            break

    title = "UF MGT builder eyescan" + "\n\n" + " Device: " + device + ", GTx: " + GTx + ", GTy: " + GTy + ", Date: " + date + ", Time: " + time + "\n"

    # formating colorbar axis
    if colorbar:
        def fmt(x, pos):
            return '1E{0:d}'.format(x)
        plt.title(title)
        plt.xlabel("Unit Interval")
        plt.ylabel("Voltage (Codes)")
        cb = plt.colorbar(myplot, format=ticker.FuncFormatter(fmt), ticks=range(minlog10ber,1,1))
        cb.set_label('BER')

    if bathtub:
        plt.title(title)
        plt.xlabel("Unit Interval")
        plt.ylabel("log10(BER)")


    # saving plot
    plt.savefig(filename_o,bbox_inches='tight')
    # showing plot if needed
    #plt.show()
    plt.close()
