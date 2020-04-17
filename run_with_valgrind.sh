#!/bin/bash

exec env DOODLEBUGS_QUIET=1 DOODLEBUGS_MAX_TICKS=1000 DOODLEBUGS_NONINTERACTIVE=1 valgrind --tool=memcheck --leak-check=full --track-origins=yes --gen-suppressions=all --leak-resolution=med --vgdb=no q1/cmake-build/q1
