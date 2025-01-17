# CMS Calorimeter simulation with parquet file output

This repository is a modification of the code used for this paper https://arxiv.org/abs/2308.09025. The original code uses CSV files as an output where one CSV file is used per event, resulting in a large number of files that take up a lot of space. The code presented in this repository uses this code to generate the CSV files and then store their content bundled in a parquet file to allow for a more efficient disk space usage and better accessibility.

## Modifications to the original Geant4 code

The original code was modified to include an extra material layer between the particle source and the detector, to allow for data collection of converted photons. The thickness of this layer and its distance to the detector can be set in the python script `gen_batch_samples_condor.py`. In addtion to that a magnetic field was applied in the whole Geant4 world volume with a strength of 4T in y-direction.

## Usage

To use this code to generate samples, one has to perform a few steps to achieve a correct set up first. The Geant4 code is already compiled and can be used as it is, although modifications are possible.

### In gen_batch_samples_condor.py:

-Add the full path to where you saved this folder in line 23

### In sampleGen.sub:

-Specify the executable python script, it has to be able to use the following modules: pandas, pyarrow, numpy (pyarrow is not installed in the standard python env on the cluster by my knowledge)

-Set the arguments that are passed to the python script, the variable names are self explanatory

-Configure output, log and error files to your liking, I recommend using a seperate folder for better visibility

-Change the number of jobs that have to be queued to match the total number of samples you want to generate: N(samples) = SUBSETSIZE * jobs.

## Additional features

In addition to the modifications in the code, a few helpful scripts for debugging were added. `plot_csv.py` contains a collection of useful functions when plotting and applying normalization to the data in the CSV format.
The folder `debug_parquet_scripts` contains some more useful scripts to debug the parquet files output by the sample generator, such as a sample counter that tests if every file that should have been created is in the right folder and contains the right amount of samples.
