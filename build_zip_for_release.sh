#!/bin/bash

cd ..
zip -r rg351_m8c.zip ./rg351_m8c -x '*.git*' -x '**/*.git*'
cd -

