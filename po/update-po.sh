#!/bin/sh
# Merges updated vermouth.pot into all language .po files

BASEDIR=$(dirname "$0")
POTFILE="$BASEDIR/vermouth.pot"

for po in "$BASEDIR"/*/vermouth.po; do
    echo "Updating $po..."
    msgmerge --update --backup=none "$po" "$POTFILE"
done

echo "Done."
