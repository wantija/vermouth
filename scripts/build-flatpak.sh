#!/bin/bash

VERSION=$(git describe --tags --abbrev=0)
flatpak --user remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak-builder --repo=flatpakrepo --force-clean flatpakbuild com.dekomote.vermouth.yml --no-fuse
flatpak build-bundle flatpakrepo vermouth-${VERSION}.flatpak com.dekomote.vermouth
