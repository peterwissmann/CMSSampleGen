import pyarrow.parquet as pq
import pyarrow

# Path to your Parquet file
parquet_file_path = ''  # Replace with your file path

table = pq.read_table(parquet_file_path)

for column_name in table.column_names:
    # Get the first entry in the column
    first_entry = table[column_name][0]

    # For the first column, print the dtype and shape of the first entry (if it's an array)
    if column_name == table.column_names[0]:
        if isinstance(first_entry, pyarrow.lib.ListScalar):  # Check if it's a ListScalar
            # For ListScalar, get the type of the list and its length
            print(f"First entry in column '{column_name}': Dtype = List, Shape = {len(first_entry)}")
        elif isinstance(first_entry, list):  # If it's a Python list
            # For Python lists, print their length
            print(f"First entry in column '{column_name}': Dtype = List, Shape = {len(first_entry)}")
        else:
            # If it's not a list or ListScalar, print its type and value
            print(f"First entry in column '{column_name}': Dtype = {type(first_entry)}, Value = {first_entry}")
    else:
        # For all other columns, print the first entry as is
        print(f"First entry in column '{column_name}': {first_entry}")