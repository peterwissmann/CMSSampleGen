import numpy as np
import pandas as pd
import sys, os

def main():
    data_list = sys.argv[1:]
    data = gather_data(data_list)
    np.save(os.path.join(os.path.dirname(sys.argv[1]), 'sim_results.npy'), data)
    energies = energy_from_name(data_list)
    np.save(os.path.join(os.path.dirname(sys.argv[1]), 'energies.npy'), energies)

def gather_data(file_list):
    data = map(get_content, file_list)
    return np.array(list(data))

def get_content(path):
    data = pd.read_csv(path, skiprows=2)
    data.columns = ['X', 'Y', 'Z', 'Edep', 'Edep^2', 'entries']
    return data['Edep'].values

def energy_from_name(file_list):
    return [x[x.find('@')+1:x.find('MeV')] for x in file_list]
if __name__ == '__main__':
    main()
