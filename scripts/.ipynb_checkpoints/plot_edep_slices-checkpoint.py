import pandas as pd
import numpy as np
import sys
import matplotlib, matplotlib.pyplot as plt
matplotlib.use('agg')


files = sys.argv[1:]

for file in files:
    print("Processing file: ", file)
    data = pd.read_csv(file, skiprows=2)
    data.columns=['iX', 'iY', 'iZ', 'edep', 'edep^2', 'entry']
    xbins = data['iX'].max()+1
    ybins = data['iY'].max()+1

    plt.figure()

    data2plot = data['edep'].values.reshape((xbins, ybins))/1000
    data2plot = np.ma.masked_where(data2plot == 0, data2plot)

    cmap = plt.cm.viridis
    cmap.set_bad(color='darkgray')
    im = plt.imshow(data2plot, cmap=cmap, interpolation=None)
    for xline in np.arange(xbins):
        plt.gca().axhline(xline+.5, color='black', linewidth=.1)
    for yline in np.arange(ybins):
        plt.gca().axvline(yline+.5, color='black', linewidth=.1)

    plt.gcf().colorbar(im, ax=plt.gca(), label='Edep in GeV')

    plt.gca().set_aspect(aspect=ybins/xbins)
    plt.gca().xaxis.set_major_locator(matplotlib.ticker.MaxNLocator(integer=True))
    plt.gca().yaxis.set_major_locator(matplotlib.ticker.MaxNLocator(integer=True))

    plt.xlabel('iY')
    plt.ylabel('iX')

    plt.tight_layout()
    plt.savefig("/net/e4-nfs-home.e4.physik.tu-dortmund.de/home/avdgraaf/CaloGAN/outputs/png/"+file.replace('.csv', '.png'))
