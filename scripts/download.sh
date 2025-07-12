#!/bin/bash

set -ex

export CURVNRSPACE="$PWD"
export WORKSPACE="$PWD/../workspace"
mkdir -p "$WORKSPACE"
cd "$WORKSPACE"

# Check out Cactus
wget https://raw.githubusercontent.com/gridaphobe/CRL/master/GetComponents
chmod a+x GetComponents
./GetComponents --no-parallel --shallow "$CURVNRSPACE/scripts/curvnr.th"

cd Cactus

# Create a link to the CurvNR repository
ln -s "$CURVNRSPACE" repos
# Create links for the CurvNR thorns
mkdir -p arrangements/CurvNR
pushd arrangements/CurvNR
ln -s ../../repos/CurvNR/* .
popd
