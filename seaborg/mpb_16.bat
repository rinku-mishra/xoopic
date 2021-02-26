#LL WWW Script Generator. Richard Gerber, NERSC
#@ class = regular    
#@ shell = /usr/bin/csh
#@ node = 1
#@ tasks_per_node = 16 
#@ network.MPI = csss,not_shared,us 
#@ wall_clock_limit = 2:00:00
#@ notification = always
#@ job_type = parallel
#@ output = mpb_16.out
#@ error = $(host).$(jobid).$(stepid).err
#@ queue
echo start
date
./xoopic -i mpb.inp -s 200
echo end
date

exit
