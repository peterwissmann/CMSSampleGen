import numpy as np
import re
import matplotlib.pyplot as plt
import os
from matplotlib import colors
import matplotlib.colors as mcolors

def get_csv_filepaths(directory, run_range=None):
    """Gets all CSV file paths in the specified directory and filters them based on run numbers.

    Args:
        directory (str): Path to the directory containing the CSV files.
        run_range (tuple, optional): Tuple specifying the (min, max) range of run numbers to include. 
                                     Defaults to None (includes all files).

    Returns:
        list: List of file paths to filtered CSV files in the directory.
    """
    csv_files = []
    pattern = re.compile(r'calo(\d{1,5})-HR\.csv')

    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".csv"):
                match = pattern.match(file)
                if match:
                    run_number = int(match.group(1))  # Extract the run number from the filename

                    # Debugging output: print matched filename and run number
                    #print(f"Found file: {file}, Run number: {run_number}")

                    # If run_range is specified, filter based on the run number
                    if run_range is None or (run_range[0] <= run_number <= run_range[1]):
                        print(f"Adding file: {file} (Run {run_number}) to the list")
                        csv_files.append(os.path.join(root, file))
                    else:
                        print(f"File: {file} (Run {run_number}) outside range {run_range}")
    
    print(f"Total files found in range: {len(csv_files)}")
    return sorted(csv_files)  # Sort files if needed for consistent order


def process_multiple_csvs(filepaths):
    """Reads in multiple CSV files, creates energy matrices, and computes the average matrix.

    Args:
        filepaths (list): List of file paths to CSV files.

    Returns:
        array: The averaged energy matrix.
    """
    # Initialize variables for accumulating the matrices
    total_matrix = None
    num_files = len(filepaths)

    if num_files == 0:
        raise ValueError("No CSV files found in the specified range.")

    # Loop through each file and accumulate the energy matrices
    valid_files_count = 0  # Count how many valid files were processed

    for i, filepath in enumerate(filepaths):
        print(f"Processing file {i+1}/{num_files}: {filepath}")
        
        energy_grid = transform_data_csv(filepath)  # Get the matrix for the current CSV
        
        if energy_grid is None:
            print(f"Warning: Skipping file {filepath} because the matrix is None.")
            continue

        # Initialize the accumulator matrix on the first valid matrix
        if total_matrix is None:
            total_matrix = np.zeros_like(energy_grid)

        # Add the current matrix to the accumulator
        total_matrix += energy_grid
        valid_files_count += 1

    # Check if any valid files were processed
    if valid_files_count == 0:
        raise ValueError("No valid CSV files were processed. Please check the files or the format.")
    
    # Compute the average by dividing the total matrix by the number of valid files
    average_matrix = total_matrix / valid_files_count
    return average_matrix


def downsample(grid, new_shape):
    """
    Downsamples a 2D grid by averaging over blocks.
    grid: original 2D array to downsample (e.g., 96x96)
    new_shape: the desired shape after downsampling (e.g., 12x12)
    """
    # Get the shape of the original grid
    shape = grid.shape
    factor_x = shape[0] // new_shape[0]
    factor_y = shape[1] // new_shape[1]

    # Reshape the grid into smaller blocks and average
    downsampled_grid = grid.reshape(new_shape[0], factor_x, new_shape[1], factor_y).mean(axis=(1, 3))

    return downsampled_grid


def transform_data_csv(filepath, isDownsample = False, new_size = None):
    """This function takes a filepath to a csv file and transforms it to a plottable grid, to plot it
    use functions like pyplot.imshow(). Is able to downsample the data blockwise if wanted.

    Args:
        filepath (string): Path to the .csv file to be used
        downsample (bool, optional): Specifies whether downsampling should be applied or not. Defaults to False.
        new_size (tuple, optional): If downsampling is wanted, new size can be input to reshape the array. Defaults to None.

    Returns:
        array: Array containing the energy matrix.
    """

    data = np.genfromtxt(filepath, delimiter=",")
    data = data[:, [0, 1, 3]]
    x = data[:, 0].astype(int)
    y = data[:, 1].astype(int)
    energy = data[:,2]
    x_max = x.max() + 1
    y_max = y.max() + 1
    energy_grid = np.zeros((x_max, y_max))
    
    for i in range(len(data)):
        energy_grid[x[i], y[i]] = energy[i]

    if isDownsample == True:
        energy_grid = downsample(energy_grid, new_size)
    return energy_grid
    
def basic_plotter(energy_grid, savepath, clean=False):
    """Basic plotting setup using pyplot imshow and the standard color scheme. 
    If clean is True, it removes axes labels, colorbars, and whitespace.

    Args:
        energy_grid (array): Array in a n x n shape.
        savepath (string): String to save the plot as png, pdf...
        clean (bool, optional): If True, generates a clean image with no axes or borders. Defaults to False.
    """
    fig, ax = plt.subplots()

    # Plot the energy grid
    img = ax.imshow(energy_grid, origin='lower', norm=colors.LogNorm()) #, norm=colors.LogNorm()

    if clean:
        # Remove axes labels, ticks, and colorbar
        ax.axis('off')  # Turns off the axes completely

        # Remove padding and margins (white borders)
        plt.subplots_adjust(left=0, right=1, top=1, bottom=0)

    else:
        # Add labels if not in clean mode
        ax.set_xlabel("Detector x")
        ax.set_ylabel("Detector y")
        
        # Add colorbar
        cbar = fig.colorbar(img, ax=ax)
        cbar.set_label("Energy in MeV")

    # Save the plot
    fig.savefig(savepath, bbox_inches='tight', pad_inches=0 if clean else 0.1)

    # Close the figure to free up memory
    plt.close(fig)
    
def max_6x6_submatrix_sum(matrix):
    n, m = matrix.shape
    max_sum = float('-inf')  # Smallest possible number
    best_submatrix = None
    
    # Iterate over all possible 6x6 submatrices
    for i in range(n - 5):
        for j in range(m - 5):
            # Extract 6x6 submatrix
            submatrix = matrix[i:i+6, j:j+6]
            submatrix_sum = np.sum(submatrix)
            
            # Update if a larger sum is found
            if submatrix_sum > max_sum:
                max_sum = submatrix_sum
                best_submatrix = submatrix
                
    return best_submatrix

def normalize_and_scale(matrix):
    """Normalizes the matrix and applies a power scaling with the exponent 0.3 (see https://arxiv.org/abs/2308.09025).

    Args:
        matrix (array): Input 2D array to normalize and scale.

    Returns:
        array: Normalized and power scaled matrix.
    """
    
    matrix_min = np.min(matrix)
    matrix_max = np.max(matrix)
    
    if matrix_max - matrix_min == 0:
        return np.zeros_like(matrix)
    
    normalized_matrix = matrix / matrix_max
    power_scaled_matrix = np.power(normalized_matrix, 0.3)
    
    return power_scaled_matrix

def process_data(folder_path, run_range = None, plotting_save_path = None, clean_plot = False):
    """Extracts data from folder path, creates matrices and scales them according to https://arxiv.org/abs/2308.09025.
    Creates plots if wished.

    Args:
        folder_path (str): Path to the folder where the data is located.
        run_range (tuple, optional): Data indices to create matrices for. Defaults to None.
        plotting_save_path (str, optional): Path to save the plots as .png files to. Defaults to None.
        clean_plot (bool, optional): Creates plot without any labes or borders if set to True. Defaults to False.
    """
    file_paths = get_csv_filepaths(folder_path, run_range)
    average_matrix = process_multiple_csvs(file_paths)
    best_submatrix = max_6x6_submatrix_sum(average_matrix)
    final_matrix = normalize_and_scale(best_submatrix)
    
    if plotting_save_path != None:
        basic_plotter(final_matrix, plotting_save_path, clean_plot)
        
    return

