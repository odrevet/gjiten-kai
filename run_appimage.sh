#!/bin/bash
BIN_DIR="$(dirname "$(readlink -f "${0}")")"
BASE_DIR=$(readlink -f "$BIN_DIR/../..")
export XDG_DATA_DIRS=$BASE_DIR/usr/share/:/usr/local/share/:/usr/share/:$XDG_DATA_DIRS
export PATH=$BIN_DIR:$PATH
$BIN_DIR/gjitenkai_bin "$@"
