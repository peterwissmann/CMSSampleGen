import pyarrow.parquet as pq
import pyarrow

# Path to your Parquet file
parquet_file_path = ''

# Number of entries you want to process
n = 10

# Read the Parquet file into a table
table = pq.read_table(parquet_file_path)

# Get the first n entries in the 'energy_grid' column
energy_grid_entries = table['energy_grid'][:n].to_pylist()

# Get the first n entries in the 'four_vector' column
four_vector_entries = table['four_vector'][:n].to_pylist()

# Loop over the first n entries and print the sum of the first matrix and the 'E' value from the first dictionary
for i in range(n):
    # Get the first matrix from the 'energy_grid' column (for the i-th entry)
    energy_grid_first_entry = energy_grid_entries[i]
    energy_grid_sum = sum(sum(row) for row in energy_grid_first_entry)  # Sum of all values in the matrix

    # Get the first dictionary from the 'four_vector' column (for the i-th entry)
    four_vector_first_entry = four_vector_entries[i]
    if four_vector_first_entry:
        first_four_vector = four_vector_first_entry[0]  # Get the first dictionary
        E_value = first_four_vector.get('E', 'N/A')  # Safely get the value of 'E'

    # Print the results for this entry
    print(f"Entry {i+1}:")
    print(f"  Sum of all entries in the first matrix of 'energy_grid': {energy_grid_sum}")
    print(f"  'E' value from the first entry in 'four_vector': {E_value}")
    print("-" * 40)