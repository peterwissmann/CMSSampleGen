#!/bin/bash
# Source the necessary environment variables for Geant4 and LCG
source /net/software_g4rt/Alma9/GEANT4/10.04.p02/geant4.10.04.p02_gcc11.3_cmake3.25_root6.18_install/bin/geant4.sh
source "/cvmfs/sft.cern.ch/lcg/views/LCG_106a/x86_64-el9-gcc11-opt/setup.sh"
#export LD_LIBRARY_PATH=/net/software_g4rt/Alma9/GEANT4/10.04.p02/geant4.10.04.p02_gcc11.3_cmake3.25_root6.18_install/lib64:$LD_LIBRARY_PATH

#Some debug statements if you have problems with the Geant4 installation
#echo "GEANT4_PATH: $GEANT4_PATH"
#echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
#echo "Searching for libG4gl2ps.so in $GEANT4_PATH/lib64"
#find $GEANT4_PATH/lib64 -name 'libG4gl2ps.so'

# Set up particle type and other parameters
N_PARTICLES=1 #Only one particle per execution, total number is given by python script
PARTICLE=gamma

# Ensure that the necessary directories exist (use absolute paths)
OUTPUT_DIR=$4
mkdir -p $OUTPUT_DIR
echo "OUTPUT_DIR is set to: $OUTPUT_DIR"

# Set the run-specific directories based on JOB_ID passed from Python/HTCondor
Starting_Number=$1  # First argument is JOB_ID
Ending_Number=$(($Starting_Number + $N_PARTICLES - 1))

# Create a run-specific directory for the job
JOB_DIR="$OUTPUT_DIR/job_$Starting_Number"
mkdir -p $JOB_DIR
echo "Created job-specific directory: $JOB_DIR"
echo "Output files will be saved to: $JOB_DIR/calo$NUMBER-HR.csv"

export JOB_DIR

thickness_iron=$2
echo "Random thickness value: $thickness_iron"

position_iron=$3
echo "Random position value: $position_iron"
# Prepare the macro file for the simulation
RUNFILE="$JOB_DIR/run-automated.mac"
for NUMBER in `seq $Starting_Number $Ending_Number`; do
  # Copy the template and modify the macro file
  cp "run-template_condor.mac" $RUNFILE

  # Replace placeholders in the macro file
  sed "s|PARTICLE|$PARTICLE|g" $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  sed "s|NUMBER|$NUMBER|g"     $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  sed "s|\${JOB_DIR}|$JOB_DIR|g" $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  sed "s|THICKNESS_IRON|$thickness_iron|g" $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  sed "s|POSITION|$position_iron|g" $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  /home/home1/institut_3a/wissmann/CaloGAN_for_SR_CMS/build/sim $RUNFILE | grep 'parent = 0' > "$JOB_DIR/$NUMBER.txt"

  echo "Running simulation for $NUMBER, output will go to: $JOB_DIR/calo$NUMBER-HR.csv"

  # Check if CSV output was created, if so, log completion
  if [ -f "$JOB_DIR/calo${NUMBER}-HR.csv" ]; then
    echo "Simulation output for $NUMBER written to calo${NUMBER}-HR.csv"
  else
    echo "Warning: No CSV file created for run $NUMBER"
  fi

  # Remove the temporary text file
  #rm -f $TEMP_TXT
done

echo "Job $Starting_Number to $Ending_Number completed."