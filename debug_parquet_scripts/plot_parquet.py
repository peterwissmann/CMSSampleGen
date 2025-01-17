import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

# Directory where Parquet files are stored
parquet_dir = ""

# Function to save a single matrix plot as an image
def save_energy_grid_plot(energy_grid, filepath):
    # Ensure that each energy grid is a 2D float array
    energy_grid = np.array([np.array(row, dtype=float) for row in energy_grid])

    # Plot and save the energy grid
    plt.imshow(energy_grid, cmap="viridis", interpolation="nearest")
    plt.colorbar(label="Energy")
    plt.title("Energy Grid")
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")

    # Save the plot to the specified filepath
    plt.savefig(filepath)
    plt.close()  # Close the plot to free up memory

# Load and save each 24x24 matrix plot from all Parquet files in the directory
for filename in os.listdir(parquet_dir):
    if filename.endswith(".parquet"):
        filepath = os.path.join(parquet_dir, filename)
        print(f"Loading data from {filepath}")

        # Read the Parquet file
        df = pd.read_parquet(filepath)

        # Iterate over each energy grid in the file
        for idx, energy_grid in enumerate(df['energy_grid']):
            # Create a filename for each plot based on the Parquet file name and grid index
            plot_filename = f"{os.path.splitext(filename)[0]}_grid_{idx}.png"
            plot_filepath = os.path.join(parquet_dir, plot_filename)
            save_energy_grid_plot(energy_grid, plot_filepath)
            print(f"Saved plot to {plot_filepath}")