#!/bin/env python
#
# File: rmake.py
#
# Purpose:	Take advantage of all machines on a cluster for
#		making.


libdirs = ["vpbase", "vpfluid", "vpionize", "vpmultif", "vptrol", "vpvardt", "vpbndry", "vpptcl", "vpcollide", "vpem", "vpio", "vpmsg", "vpstd", "vorpal"]
nodes = ["node12", "node13", "node14", "node15"]
freenodes = nodes
# A dictionary holds the nodes and start times for a given pid
pidnodes = {}

while len(libdirs):

# If no free nodes, then poll (using ps) until have one
  if len(freenodes) == 0:

# At end, notes where we are building

# From pid of completed job, get node

# If job failed, then kill all jobs and quit
# remove from dictionary
# Record the time


# Now have a free node, so launch next job, remove from libdirs
# If launching vorpal, then make only vorpal.o
rsh node13 make -C "/home/research/cary/projects/vorpal-mpi-full-opteron/" + libdirs[0] -j 3


while len(pidnodes) > 0:
# Now poll remaining jobs until all completed

# Finally, launch the build of vorpal


