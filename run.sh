#!/bin/bash
BIN_DIR=bin
BASE_DIR=.
export XDG_DATA_DIRS=$BASE_DIR/share/:/usr/local/share/:/usr/share/:$XDG_DATA_DIRS
export PATH=$BIN_DIR:$PATH
$BIN_DIR/gjitenkai "$@"
