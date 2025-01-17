import pandas as pd
import numpy as np
import sys
import matplotlib, matplotlib.pyplot as plt

# this makes the rendering on remote machines a lot faster
# BUT: you cannot use the plt.show() command with this! If you want it, you need to take this out
matplotlib.use('agg')

# exit, it no result file is passed to the code
if len(sys.argv) < 2:
    print("You need to pass one or multiple files as arguments!")
    exit()

# accepts more than one result file
files = sys.argv[1:]

# loop over results files
for file in files:
    print("Processing file: ", file)
    # skip first two rows as they do not contain data or the header
    data = pd.read_csv(file, skiprows=2)

    # we do have a header, its ugly though. Let's set new column names:
    data.columns=['iX', 'iY', 'iZ', 'edep', 'edep^2', 'entry']

    # for reference: get the number of bins in each dimension
    xbins = data['iX'].max()+1
    ybins = data['iY'].max()+1
    zbins = data['iZ'].max()+1

    ########################################################################################
    ### create depth curve projection plot
    ########################################################################################

    # extract a numpy array from the pandas DataFrame
    edep = data['edep'].values
    # reshape the flat array into it's 3D form:
    # if you are not sure how and why this works, try it out yourself what it does
    edep3D = edep.reshape((xbins, ybins, zbins))
    # summarize over x and y dimension --> creates projection on z axis, which in this simulation direction of the beam
    edep_depth_projection = edep3D.sum(axis=(0,1))
    
    x = np.arange(0, 80)*6

    # plot markers and a line to guide the eye
    plt.plot(x, edep_depth_projection, '+', label='$E_\\mathrm{dep}$')
    plt.plot(x, edep_depth_projection, '--', label='line to guide the eye')
    plt.axvline(90, color='C3')
    plt.axvline(90+346, color='C3')
    # save stuff
    plt.xlabel('depth in mm')
    plt.ylabel('$E_\\mathrm{dep}$ in MeV')
    plt.legend(loc='best')
    plt.tight_layout()
    plt.savefig(file.replace('.csv', '_depthProjection.png'))
    
    plt.yscale('log')
    plt.savefig(file.replace('.csv', '_depthProjection_log.png'))

    plt.close()

    ########################################################################################
    ### create depth curve with standard error of the mean projection plot
    ########################################################################################

    # this works because mean(x^2) - mean(x)^2 ~= std_dev(x)^2
    # if you don't believe this, just derive it yourself from the equation for std_dev

    # extract a numpy array from the pandas DataFrame
    edep_depth_projection = data['edep'].values.reshape((xbins, ybins, zbins)).sum(axis=(0,1))
    edep_sq_depth_projection = data['edep^2'].values.reshape((xbins, ybins, zbins)).sum(axis=(0,1))
    N_depth_projection = data['entry'].values.reshape((xbins, ybins, zbins)).sum(axis=(0,1))

    sems = np.sqrt( N_depth_projection * ( edep_sq_depth_projection/N_depth_projection - edep_depth_projection**2/N_depth_projection**2 )  )

    # plot markers and a line to guide the eye
    #x = np.arange(len(edep_depth_projection))
    plt.errorbar(x, edep_depth_projection, yerr=sems, fmt='+', label='$E_\\mathrm{dep} \\pm \\sigma$')
    plt.plot(edep_depth_projection, '--', label='line to guide the eye')

    # save stuff
    plt.xlabel('depth in mm')
    plt.ylabel('$E_\\mathrm{dep}$ in MeV')
    plt.legend(loc='best')
    plt.tight_layout()
    plt.savefig(file.replace('.csv', '_depthProjection_with_sem.png'))
    plt.close()

    ########################################################################################
    ### create top view projection plot
    ########################################################################################

    # this is a tiny bit more difficult
    # all three preprocessing steps (numpy array, reshape and project) in one row:
    # this time: sum over y axis, leaving a projection on x-z plane
    edep_top_view_projection = data['edep'].values.reshape((xbins, ybins, zbins)).sum(axis=1)

    # for nicer visuals: set all pixels which have no hits at all (Edep=0) to gray instead of dark blue
    # comment this out to see the changes it makes
    edep_top_view_projection = np.ma.masked_where(edep_top_view_projection == 0, edep_top_view_projection)
    cmap = plt.cm.viridis
    cmap.set_bad(color='darkgray')

    plt.figure(figsize=(8,3))
    # plot the 2d energy deposition
    # turn of interpolation (blurring) at the edges pf pixels
    im = plt.imshow(edep_top_view_projection, cmap=cmap, aspect='auto')
    
    plt.axvline(90/6, linestyle='--', color='C3')
    plt.axvline((90+346)/6, linestyle='--', color='C3')
    
    # add a colorbar to the plot
    plt.gcf().colorbar(im, ax=plt.gca(), label='Edep in MeV')

#     # insert thin black lines to mark the borders of the individual pixels
#     for xline in np.arange(xbins):
#         plt.gca().axhline(xline+.5, color='black', linewidth=.1)
#     for yline in np.arange(ybins):
#         plt.gca().axvline(yline+.5, color='black', linewidth=.1)
#     # make ticks only on integers
#     plt.gca().xaxis.set_major_locator(matplotlib.ticker.MaxNLocator(integer=True))
#     plt.gca().yaxis.set_major_locator(matplotlib.ticker.MaxNLocator(integer=True))

#     # setting the aspect ratio, so this image is square. change this if you like
#     plt.gca().set_aspect(aspect=ybins/xbins)
    # labels and save stuff
    plt.xlabel('z in mm')
    plt.ylabel('x in mm')
    plt.tight_layout()
    plt.savefig(file.replace('.csv', '_topView.png'))
    plt.close('all')
