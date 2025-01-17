#!/bin/sh
source "/cvmfs/sft.cern.ch/lcg/views/LCG_97/x86_64-centos7-gcc8-opt/setup.sh"

cd "/net/e4-nfs-home.e4.physik.tu-dortmund.de/home/avdgraaf/CaloGAN"
export projectBaseDir=$PWD

cd "/net/e4-nfs-home.e4.physik.tu-dortmund.de/home/avdgraaf/CaloGAN/build"

export outputPath="$projectBaseDir/outputs"
#export rawPath="/ceph/groups/e4/users/avdgraaf/private/Calo_images/Data_Set1_16x16"
#export rawPath="/ceph/groups/e4/users/avdgraaf/private/Calo_images/Test_Data_Set1_16x16"
export rawPath="/ceph/groups/e4/users/avdgraaf/public/TrainCNN_Pion_24x24_20GeV_5-0m"

if [ ! -d $rawPath ]
  then
    mkdir $rawPath
fi

export jobOutputPath="$projectBaseDir/$JOB_ID"
#export firstlayeroutputFileX1="$rawPath/L1_Photon_X1_@$JOB_ID.csv"
#export firstlayeroutputFileX4="$rawPath/L1_Photon_@$JOB_ID.csv"
export outputFileX1="$rawPath/L2_Photon_X1_@$JOB_ID.csv"
export outputFileX4="$rawPath/L2_Photon_X4_@$JOB_ID.csv"
#export thirdlayeroutputFile="$rawPath/L3_Photon_@$JOB_ID.csv"
export totallayeroutputFile="$rawPath/Total_Layer@$JOB_ID.csv"

if [ ! -d $jobOutputPath ]
  then
    mkdir $jobOutputPath
fi

cp  "$projectBaseDir/macros/run_batch_template.mac"  "$jobOutputPath/run.mac"
#sed -i "s|OUTPUT_FILE_PLACEHOLDERx1_L1|$firstlayeroutputFileX1|g" "$jobOutputPath/run.mac"
#sed -i "s|OUTPUT_FILE_PLACEHOLDERx4_L1|$firstlayeroutputFileX4|g" "$jobOutputPath/run.mac"
sed -i "s|OUTPUT_FILE_PLACEHOLDERx1|$outputFileX1|g" "$jobOutputPath/run.mac"
sed -i "s|OUTPUT_FILE_PLACEHOLDERx4|$outputFileX4|g" "$jobOutputPath/run.mac"
#sed -i "s|OUTPUT_FILE_PLACEHOLDER_Third_Layer|$thirdlayeroutputFile|g" "$jobOutputPath/run.mac"
sed -i "s|OUTPUT_FILE_PLACEHOLDER_Total_Layer|$totallayeroutputFile|g" "$jobOutputPath/run.mac"
sed -i "s|ENERGY_PLACEHOLDER|20000|g" "$jobOutputPath/run.mac"

./sim "$jobOutputPath/run.mac"
rm -r $jobOutputPath
