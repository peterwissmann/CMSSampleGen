#!/bin/bash
source /net/software_g4rt/Alma9/GEANT4/10.04.p02/geant4.10.04.p02_gcc11.3_cmake3.25_root6.18_install/bin/geant4.sh
source "/cvmfs/sft.cern.ch/lcg/views/LCG_106a/x86_64-el9-gcc11-opt/setup.sh"
#source /net/software_g4rt/Alma9/GEANT4/10.04.p02/geant4.10.04.p02_gcc11.3_cmake3.25_root6.18_install/bin/geant4.sh

N_PARTICLES=2
Starting_Number=$JOB_ID
Ending_Number=$(($JOB_ID + $N_PARTICLES - 1))

PARTICLE=gamma
#PARTICLE=pi0
mkdir -p /home/home1/institut_3a/wissmann/generated_samples/$PARTICLE
RUNDIR=run-$PARTICLE-$Starting_Number-to-$Ending_Number
mkdir -p $RUNDIR

# Ensure the job directory exists before starting the simulation
if [ ! -d "$JOB_DIR" ]; then
  echo "Error: Job directory $JOB_DIR does not exist."
  exit 1
fi

RUNFILE=$RUNDIR/run-automated.mac
for NUMBER in `seq $Starting_Number $Ending_Number`; do
  cp "run-template.mac" $RUNFILE
  sed "s|PARTICLE|$PARTICLE|g" $RUNFILE 2>&1 > log && mv log $RUNFILE
  sed "s|NUMBER|$NUMBER|g"     $RUNFILE 2>&1 > log && mv log $RUNFILE
  /home/home1/institut_3a/wissmann/CaloGAN_for_SR_CMS/build/sim $RUNFILE | grep 'name = gamma' > /home/home1/institut_3a/wissmann/generated_samples/$PARTICLE/$NUMBER.txt
#  sed -i -e 's/^/# /' "/ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V03/$PARTICLE/$NUMBER.txt"
#  sed -i -e "1 { r /ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V03/$PARTICLE/$NUMBER.txt" -e 'N; }' "/ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V03/$PARTICLE/calo$NUMBER-HR.csv"
#  rm /ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V03/$PARTICLE/$NUMBER.txt
done

# After simulation is done, ensure CSV files are created
if [ ! -f "$JOB_DIR/calo0-HR.csv" ]; then
  echo "Error: Simulation output file calo0-HR.csv was not created."
  exit 1
fi
rm -r $RUNDIR
