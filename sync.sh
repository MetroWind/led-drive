#!/bin/zsh
# Run this in the build dir
cd ../..
rsync -avz --exclude="build" --exclude=".cache" --exclude=".git" led-drive/ backpi.xeno:programs/led-drive
