"""
ionvis.py

IonMD ion visualization functions.

This file is part of IonMD.

IonMD is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your
option) any later version.
  
IonMD is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with IonMD.  If not, see <http://www.gnu.org/licenses/>.
"""

import numpy as np
import ctypes
import matplotlib.pyplot as plt
import Image, ImageEnhance
from mayavi import mlab

# color order = red, forest green, cyan, gold
colors = [(1,0,0), (.13,.55,.13), (0,1,1), (1,.84,0)]

def display(fpos_fname='fpos.xyz', m_lc=138, outfile=None):
    scale = 25.
    ions, x, y, z = np.loadtxt(fpos_fname, skiprows=2, unpack=True)
    xlist = []
    m_last, ci = ions[0], 0
    fig = mlab.figure(size=(640,480), bgcolor=(0,0,0))
    for i, ion in enumerate(ions):
        if ion != m_last or i == len(ions)-1:
            xlist = np.array(xlist)
            mlab.points3d(xlist[:,0], xlist[:,1], xlist[:,2],
                          color=colors[ci], scale_factor=scale)
            xlist = []
            ci += 1
            m_last = ions[i]
        xlist.append([x[i], y[i], z[i]])
    mlab.view(azimuth=45, elevation=90)
    mlab.roll(180)
    mlab.orientation_axes()
    if outfile:
        mlab.savefig(outfile, figure=fig)
        mlab.close(all=True)
    else:
        mlab.show()

def toRGB(data, channels):
    """Convert 2D grayscale data to 2D 3 channel 'RGB' data."""
    new_data = np.zeros((data.shape[0],data.shape[1],3))
    for i in range(3):
        new_data[:,:,i] = data*channels[i]
    return new_data

def simCCD(ccd_fname_prefix, N_ccd, bins, extents,
           outfile=None, show=True, brightness=1, imgcmd="eog"):
    """Simulate a CCD image."""
    tmpimg = "images/tmp.png"
    ccd = np.zeros((bins,bins,3))
    for i in range(N_ccd):
        hist = np.fromfile(ccd_fname_prefix + "_" + str(i+1) + ".dat")
        ranges = hist[:(bins*2+2)]
        data = hist[(bins*2+2):]
        data /= data.max()
        data.shape = (bins,bins)
        ccd += toRGB(data, colors[i%len(colors)])
    plt.imsave(fname=tmpimg, arr=ccd)
    imgA = Image.open(tmpimg)
    imgB = ImageEnhance.Brightness(imgA)
    imgC = imgB.enhance(brightness)
    if outfile:
        imgC.save(outfile)
    if show:
        imgC.show(command=imgcmd)
    return ccd

if __name__ == "__main__":
    ccd_bins, ccd_extent = 512, 600
    display()
    #simCCD("ccd", 2, ccd_bins, ccd_extent, brightness=1.5,
    #       outfile="images/CCD_latest.png", show=True)
