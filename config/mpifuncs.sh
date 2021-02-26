#!/bin/sh

grep " MPI_" */*.cpp */*.h */*.f */*.f90 2>/dev/null | grep '(' | sed 's/(.*$//' | sed 's/^.* //' | grep ^MPI_ | sort -u
