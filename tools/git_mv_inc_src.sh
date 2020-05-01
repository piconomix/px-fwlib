#!/bin/bash
mkdir -p inc
mkdir -p src
for f in *_template.h; do git mv "${f}" "inc/${f/_template.h/.h}"; done;
for f in *.h; do git mv "${f}" "inc/${f}"; done;
for f in *.c; do git mv "${f}" "src/${f}"; done;
