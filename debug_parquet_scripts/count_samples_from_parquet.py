import pandas as pd
import os

# Directory where Parquet files are stored
parquet_dir = ""

# Input the number of expected files
expected_num_files = int(input("Enter the total number of expected Parquet files: "))

# Generate a set of expected file names
expected_files = {f"simulation_results_{i}.parquet" for i in range(expected_num_files)}

# Get a set of actual file names in the directory
actual_files = {f for f in os.listdir(parquet_dir) if f.endswith(".parquet")}

# Identify missing and created files
missing_files = sorted(
    expected_files - actual_files,
    key=lambda x: int(x.split("_")[2].split(".")[0])  # Extract and sort by the number
)
created_files = sorted(
    expected_files & actual_files,
    key=lambda x: int(x.split("_")[2].split(".")[0])  # Extract and sort by the number
)

#Uncomment if you want to see which files are missing
#if missing_files:
    #print("Missing files:")
    #for missing_file in missing_files:
        #print(f"  {missing_file}")

# Count samples in each created file
total_samples = 0
for filename in created_files:
    filepath = os.path.join(parquet_dir, filename)

    # Read the Parquet file
    try:
        df = pd.read_parquet(filepath)

        # Count the number of samples (rows) in the file
        num_samples = len(df)
        total_samples += num_samples
        #Uncomment this to print the number of samples per file for debugging
        #print(f"The file {filename} contains {num_samples} samples.")
    except Exception as e:
        print(f"Error reading {filename}: {e}")
        
# Print summary of file existence
print(f"Total number of expected files: {expected_num_files}")
print(f"Total number of created files: {len(created_files)}")
print(f"Total number of missing files: {len(missing_files)}")
print(f"Total number of samples: {total_samples}")