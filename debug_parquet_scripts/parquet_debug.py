import pandas as pd

# Replace 'your_file.parquet' with the path to your Parquet file
file_path = ''

# Read the Parquet file into a Pandas DataFrame
df = pd.read_parquet(file_path)

# Display the contents of the DataFrame
print(df)

# Optional: Display column names and basic info
print("\nColumns:", df.columns.tolist())
print("\nDataFrame Info:")
print(df.info())