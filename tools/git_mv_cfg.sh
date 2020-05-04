#!/bin/bash
for file in `git ls-files | grep .*_cfg_default.h`; do git mv $file `echo $file | sed -e 's/_cfg_default/_cfg_template/'` ; done
