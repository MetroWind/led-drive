#!/bin/zsh
# Run this in the build dir
cd ../..
rsync -avz --exclude="build" --exclude=".cache" led-drive/ backpi.xeno:programs/led-drive
