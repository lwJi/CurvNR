#!/bin/bash

set -ex

export CURVBASESPACE="$PWD"
export WORKSPACE="$PWD/../workspace"
mkdir -p "$WORKSPACE"
cd "$WORKSPACE"

cd Cactus

# Set up SimFactory
cp "$CURVBASESPACE/scripts/actions-$ACCELERATOR-$REAL_PRECISION.cfg" ./simfactory/mdb/optionlists
cp "$CURVBASESPACE/scripts/actions-$ACCELERATOR-$REAL_PRECISION.ini" ./simfactory/mdb/machines
cp "$CURVBASESPACE/scripts/actions-$ACCELERATOR-$REAL_PRECISION.run" ./simfactory/mdb/runscripts
cp "$CURVBASESPACE/scripts/actions-$ACCELERATOR-$REAL_PRECISION.sub" ./simfactory/mdb/submitscripts
cp "$CURVBASESPACE/scripts/curvbase.th" .
cp "$CURVBASESPACE/scripts/defs.local.ini" ./simfactory/etc

# For Formaline
git config --global user.email "curvbase@einsteintoolkit.org"
git config --global user.name "Github Actions"

case "$MODE" in
    debug) mode='--debug';;
    optimize) mode='--optimise';;
    *) exit 1;;
esac

# Build
# The build log needs to be stored for later.
time ./simfactory/bin/sim --machine="actions-$ACCELERATOR-$REAL_PRECISION" build "$mode" --jobs $(nproc) sim 2>&1 |
    tee build.log

# Check whether the executable exists and is executable
test -x exe/cactus_sim
