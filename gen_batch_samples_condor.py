import subprocess
import pandas as pd
import pyarrow as pa
import pyarrow.parquet as pq
import os
import sys
import time
import shutil
import numpy as np
import random

# Read subset size and output directory from command line arguments
subset_size = int(sys.argv[1])
output_dir = str(sys.argv[2])        # Directory to store Parquet files
job_id = sys.argv[3]            # Unique identifier for the job
output_dir_csvs = sys.argv[4] #Directory to store csvs before they are added to parquet
print(f"Running job with subset_size={subset_size}")

# Set up environment variables
env = os.environ.copy()

# Paths for script and output
simulation_script = "/PathToThisFolder/CaloGAN_for_SR_CMS/run_condor.sh" #Replace with your folder location here
output_parquet_file = os.path.join(output_dir, f"simulation_results_{job_id}.parquet")

# Set up directories
base_output_dir = output_dir_csvs
os.makedirs(output_dir, exist_ok=True)
job_dir = os.path.join(base_output_dir, f"job_{job_id}")
os.makedirs(job_dir, exist_ok=True)

# Ensure no leftover files from previous runs
for f in os.listdir(job_dir):
    os.remove(os.path.join(job_dir, f))

# Function to process a CSV file with additional validation for row completeness
def process_csv_file(filepath):
    try:
        data = np.genfromtxt(filepath, delimiter=",")
        
        # Check for empty or improperly formatted data
        if data.size == 0:
            raise ValueError(f"CSV file {filepath} is empty or improperly formatted.")
        # Separate columns and create energy grid
        x, y, energy = data[:, 0].astype(int), data[:, 1].astype(int), data[:, 3]
        
        # Validate data consistency
        if len(x) == 0 or len(y) == 0 or len(energy) == 0:
            raise ValueError(f"CSV file {filepath} has incomplete data.")
        
        # Create energy grid and fill with energy values
        energy_grid = np.zeros((x.max() + 1, y.max() + 1))
        energy_grid[x, y] = energy
        return energy_grid

    except IndexError:
        # Catches cases where some rows have fewer than expected columns
        print(f"Error: CSV file {filepath} has rows with missing values.")
        return None
    except Exception as e:
        print(f"Error processing {filepath}: {e}")
        return None

def extract_four_vector(file_path):
    """Extract four-vector information from the text file."""
    with open(file_path, 'r') as f:
        lines = f.readlines()

    four_vectors = []
    for line in lines:
        if 'parent = 0' in line:  # Ensures only top-level particles are captured
            data = {}
            for entry in line.split('|'):
                key, value = entry.strip().split('=')
                data[key.strip()] = value.strip()

            # Extract relevant fields
            four_vectors.append({
                'name': data.get('name', ''),
                'E': float(data.get('E', 0)),
                'px': float(data.get('px', 0)),
                'py': float(data.get('py', 0)),
                'pz': float(data.get('pz', 0))
            })

    return four_vectors

# Collect exactly `subset_size` samples
batch_data = []
generated_samples = 0
#start = time.time_ns()
while generated_samples < subset_size:
    print(f"Running simulation {generated_samples + 1} for job {job_id}")
    #Here, the thickness and position of the intermediate layer are set
    random_thickness = np.round(random.uniform(0.5, 1.5) * 1.757, decimals=3)  # Adjust range as needed, in cm, 1.757 is 1 rad length
    random_position = np.round(random.uniform(-50, -90), decimals=3) #in cm, detector is at 0, beam at -140.5
    print(f"Thickness set to {random_thickness} cm")
    print(f"Position set to {random_position} cm")
    result = subprocess.run([simulation_script, str(job_id), str(random_thickness), str(random_position), str(output_dir_csvs)], capture_output=True, text=True, env=env)
    print(result.stdout)
    print(result.stderr)
    if result.returncode != 0:
        print(f"Error running simulation: {result.stderr}")
        continue  # Skip to the next iteration without incrementing the counter

    # Wait for CSV file(s) to appear in `job_dir`
    for _ in range(25):
        output_files = [f for f in os.listdir(job_dir) if f.endswith('.csv') and f.startswith('calo')]
        text_files = [f for f in os.listdir(job_dir) if f.endswith('.txt') and f.startswith(str(job_id))]
        if output_files and text_files:
            break
        time.sleep(1)

    # Process the first available CSV file and delete it
    if output_files and text_files:
        output_file_path = os.path.join(job_dir, output_files[0])
        text_file_path = os.path.join(job_dir, text_files[0])
        print(f"Processing file: {output_file_path}, Text: {text_file_path}")

        energy_grid = process_csv_file(output_file_path)
        four_vector = extract_four_vector(text_file_path)
        if energy_grid is not None:
            batch_data.append({
            'energy_grid': energy_grid.tolist(),
            'random_thickness': random_thickness,
            'distance_to_detector': np.abs(random_position),
            'four_vector': four_vector
            })
            generated_samples += 1  # Increment only after successful processing
        
        os.remove(output_file_path)  # Delete after processing
    else:
        print(f"No CSV files found after waiting, skipping this sample.")

# Save processed data if available
if batch_data:
    energy_grids = [item['energy_grid'] for item in batch_data]
    thicknesses = [item['random_thickness'] for item in batch_data]
    positions = [item['distance_to_detector'] for item in batch_data]
    four_vectors = [item['four_vector'] for item in batch_data]
    batch_df = pd.DataFrame({
        'energy_grid': energy_grids,  # Nested energy grid data
        'thickness': thicknesses,  # Random thickness values
        'distance_to_detector': positions, #Distance to the detector
        'four_vector': four_vectors  # List of four-vector dictionaries
    })
    table = pa.Table.from_pandas(batch_df)
    pq.write_table(table, output_parquet_file)
    print(f"Job {job_id} completed and saved to {output_parquet_file}")
else:
    print(f"Job {job_id} completed but no data to save.")
#end = time.time_ns()
#print("Total time:", end-start)
# Clean up job directory
try:
    shutil.rmtree(job_dir)
    print(f"Job directory {job_dir} deleted successfully.")
except Exception as e:
    print(f"Error while deleting the job directory: {e}")