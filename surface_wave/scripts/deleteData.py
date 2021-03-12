#!/usr/bin/env python3
import os
import numpy as np

dir0 = "/scratch/data/outputs"
dir1 = "/scratch/data/outputs/phi/"
dir2 = "/scratch/data/outputs/density/"
dir3 = "/scratch/data/outputs/E_35/"
if os.path.exists(dir0):
	if os.path.exists(dir1):
		os.system('rm '+dir1+'*')
	if os.path.exists(dir2):
		os.system('rm '+dir2+'*')
	if os.path.exists(dir3):
		os.system('rm '+dir3+'*')
else:
	raise Exception("output directory does not exist")
