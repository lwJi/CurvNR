#!/bin/bash

set -ex

export CURVBASESPACE="$PWD"
export WORKSPACE="$PWD/../workspace"
mkdir -p "$WORKSPACE"
cd "$WORKSPACE"

# Check out Cactus
wget https://raw.githubusercontent.com/gridaphobe/CRL/master/GetComponents
chmod a+x GetComponents
./GetComponents --no-parallel --shallow "$CURVBASESPACE/scripts/curvbase.th"

cd Cactus

# Create a link to the CurvBase repository
ln -s "$CURVBASESPACE" repos
# Create links for the CurvBase thorns
mkdir -p arrangements/CurvBase
pushd arrangements/CurvBase
ln -s ../../repos/CurvBase/* .
popd
