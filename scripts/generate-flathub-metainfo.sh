#!/bin/bash
# Generate a Flathub-ready metainfo XML from the template and CMakeLists.txt values.
# Usage: ./scripts/generate-flathub-metainfo.sh [output_path]
#   output_path defaults to com.dekomote.vermouth.metainfo.xml in the project root.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
TEMPLATE="${PROJECT_DIR}/assets/com.dekomote.vermouth.metainfo.xml.in"
OUTPUT="${1:-${PROJECT_DIR}/com.dekomote.vermouth.metainfo.xml}"

# Extract variables from CMakeLists.txt
extract_var() {
    local var="$1"
    grep -oP "set\(${var} \"?\K[^\")]*" "${PROJECT_DIR}/CMakeLists.txt" | head -1
}

extract_version() {
    grep -oP 'project\(vermouth VERSION \K[0-9.]+' "${PROJECT_DIR}/CMakeLists.txt"
}

APP_ID=$(extract_var APP_ID)
APP_DESCRIPTION=$(extract_var APP_DESCRIPTION)
APP_LICENSE=$(extract_var APP_LICENSE)
APP_HOMEPAGE=$(extract_var APP_HOMEPAGE)
APP_BUG_ADDRESS=$(extract_var APP_BUG_ADDRESS)
APP_AUTHOR_NAME=$(extract_var APP_AUTHOR_NAME)
APP_AUTHOR_EMAIL=$(extract_var APP_AUTHOR_EMAIL)
APP_LONG_DESCRIPTION=$(extract_var APP_LONG_DESCRIPTION)
PROJECT_VERSION=$(extract_version)
APP_RELEASE_DATE=$(date +%Y-%m-%d)

sed \
    -e "s|@APP_ID@|${APP_ID}|g" \
    -e "s|@APP_DESCRIPTION@|${APP_DESCRIPTION}|g" \
    -e "s|@APP_LICENSE@|${APP_LICENSE}|g" \
    -e "s|@APP_HOMEPAGE@|${APP_HOMEPAGE}|g" \
    -e "s|@APP_BUG_ADDRESS@|${APP_BUG_ADDRESS}|g" \
    -e "s|@APP_AUTHOR_NAME@|${APP_AUTHOR_NAME}|g" \
    -e "s|@APP_AUTHOR_EMAIL@|${APP_AUTHOR_EMAIL}|g" \
    -e "s|@APP_LONG_DESCRIPTION@|${APP_LONG_DESCRIPTION}|g" \
    -e "s|@PROJECT_VERSION@|${PROJECT_VERSION}|g" \
    -e "s|@APP_RELEASE_DATE@|${APP_RELEASE_DATE}|g" \
    "$TEMPLATE" > "$OUTPUT"

echo "Generated: ${OUTPUT}"
