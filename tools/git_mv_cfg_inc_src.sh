#!/bin/bash
mkdir -p cfg
mkdir -p inc
mkdir -p src
for f in *_cfg_template.h; do git mv "${f}" "cfg/${f/_template.h/.h}"; done;
for f in *.h; do git mv "${f}" "inc/${f}"; done;
for f in *.c; do git mv "${f}" "src/${f}"; done;
