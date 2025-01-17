#!/bin/bash
source "/cvmfs/sft.cern.ch/lcg/views/LCG_106a/x86_64-el9-gcc11-opt/setup.sh"
# source "/cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos8-gcc11-opt/setup.sh"

N_PARTICLES=1
Starting_Number=0
Ending_Number=1

PARTICLE=gamma
#PARTICLE=pi0
mkdir -p /home/home1/institut_3a/wissmann/generated_samples/$PARTICLE
RUNDIR=run-$PARTICLE-$Starting_Number-to-$Ending_Number
mkdir -p $RUNDIR


RUNFILE=$RUNDIR/run-automated.mac

for NUMBER in `seq $Starting_Number $Ending_Number`; do
  cp "run-template.mac" $RUNFILE
  sed "s|PARTICLE|$PARTICLE|g" $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  sed "s|NUMBER|$NUMBER|g"     $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  sed "s|THICKNESS_IRON|$thickness_iron|g" $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  sed "s|POSITION|$position_iron|g" $RUNFILE > $RUNFILE.temp && mv $RUNFILE.temp $RUNFILE
  /home/home1/institut_3a/wissmann/CaloGAN_for_SR_CMS/build/sim $RUNFILE | grep 'name = gamma' > /home/home1/institut_3a/wissmann/generated_samples/$PARTICLE/$NUMBER.txt
  # ./sim $RUNFILE | grep 'name = gamma' > /ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V02/$PARTICLE/$NUMBER.txt
  # sed -i -e 's/^/# /' "/ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V02/$PARTICLE/$NUMBER.txt"
  # sed -i -e "1 { r /ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V02/$PARTICLE/$NUMBER.txt" -e 'N; }' "/ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V02/$PARTICLE/calo$NUMBER-HR.csv"
  # rm /ceph/groups/e4/users/avdgraaf/public/New_CaloSim_2022_V02/$PARTICLE/$NUMBER.txt
done


rm -r $RUNDIR