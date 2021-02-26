;------------------------------------------------------------------------
;
;  File:    plot_ptcl.pro
;
;  Purpose:
;    IDL procedure to read in q-q (coordinate components, i.e. plots in 
;      configuration space)
;      particle data from a .txt file generated
;      by the XOOPIC gui.
;
;  Overview:
;    The XOOPIC .txt files generally have some lines of text.  You
;      need to delete any such text lines, leaving only the data.
;      Also, the particle data files can have data for multiple particle
;      species (order of data corresponds to the order of the species
;      definitions in the XOOPIC *.inp input file).  Be sure to delete
;      all but the desired species.
;
;    Argument list:
;
;    base_name  -> a string that specifies the input
;                  data file (base_name.txt) and is also used to name 
;                  output files.
;    X1_Label     -> a string that specifies the coordinate component, e.g. "x".
;    X2_Label -> a string that specifies the component of the velocity.
;    ScaleFlag  -> a flag to specify the scaling for length, i.e. of q:
;                        0 -> no scaling
;                        1 -> length scale = c/omega_0 (the laser frequency)
;                        2 -> length scale = lambda_p (the plasma wavelength)
;    ScaleInput -> this is irrelevant for ScaleFlag = 0,
;                          or omega_0 for ScaleFlag = 1,
;          plasma density in [1/cm^3] for ScaleFlag = 2
;
;  Version: $Id: plot_ptcl.pro 2032 2003-04-29 16:40:51Z busby $
;
;  Copyright (c) 2000 by Tech-X Corporation.  All rights reserved.
;
;  Change log:
;  1.00 (Bruhwiler 11-07-2000) Initial code.
;  1.01 (Bruhwiler 11-01-2001) Minor changes to limits, autoscaling
;
;------------------------------------------------------------------------


pro plot_ptcl, base_name, datasetname, X1_Label,  X2_Label, ScaleFlag, ScaleInput

; *****************************************************************
; Names for files and labels
; *****************************************************************

; Specify the text, restore and postscript file names
text_file     = base_name + '.txt'
h5_file       = base_name + '.h5'
restore_file  = base_name + '.dat'
momentum_file = base_name + '.ps'
energy_file   = base_name + '_e.ps'

;
; Specify the number of points to plot
;
num_points = 1000000


; Read data from HDF5 file

file_id = H5F_OPEN(h5_file)

;; Read in x1 values 
dataset_id = H5D_OPEN(file_id,datasetname)
data = H5D_READ(dataset_id)
H5D_CLOSE, dataset_id

H5F_CLOSE, file_id

z =data(0,*)
r =data(1,*)


print, ' '
print, 'z, r are the columns of the raw data:'
help,z
help,r
nz = n_elements(z)
nr = n_elements(r)
print, 'z(0) z(1) z(', nz-1, ') = ', z(0),z(1),z(nz-1)
print, 'r(0) r(1) r(', nr-1, ') = ', r(0),r(1),r(nr-1)

if (nz eq nr) then begin
  num_particles = nz
  print, ' '
  print, 'number of particles   = ', num_particles
endif else begin
  print, ' '
  print, '********************************************'
  print, 'WARNING:  Problems in parsing of the data --'
  print, ' nz = ', nz
  print, ' nr = ', nr
  print, 'These two numbers should be the same!
  print, '********************************************'
endelse

;
; *****************************************************************
; Scale the data so it corresponds to the desired units and set the labels
; *****************************************************************
;
q1_label = '!3' + X1_Label
q2_label = '!3' + X2_Label
if ( ScaleFlag eq 1 ) then begin
  ;
  ; scale the length using as a c/omega_0 as a length scale
  ; 
  omega_0 = ScaleInput
  lengthScale = 3.0e8/omega_0
  z = z/lengthScale
  r = r/lengthScale
endif else if (ScaleFlag eq 2) then begin
  ;
  ; scale lengths to the plasma wavelength lambda_p
  ; 
  density = ScaleInput
  lambda_p = 3.0e+08 / ( 9000.0 * sqrt(density) )
  z = z / lambda_p
  r = r / lambda_p
endif else begin
  ;
  ; this is the default case of no scaling 
  ;
  x_label = '!3' + X1_Label + ' (m)'
  y_label = '!3' + X2_Label + ' (m)'
endelse

; *****************************************************************
; Here we cull the particles, if necessary.
; *****************************************************************

if (num_particles le num_points) then begin
  print, ' '
  print, 'num_particles <= num_points --'
  print, '  num_particles = ', num_particles
  print, '  num_points    = ', num_points
  print, 'All of the particles will be plotted.'

  num_points = num_particles
  zi = z;
  ri = r;
endif else begin
  print, ' '
  print, 'Particles are being culled --'
  print, '  Only ', num_points, ' particles will be plotted.'

  indices = findgen( num_points )
  indices = indices * (num_particles-1) / num_points
  zi = z(indices)
  ri = r(indices)
endelse

;
; *****************************************************************
; set the min and max values of the data
; *****************************************************************
;

h_autoscale = 1
v_autoscale = 1

if (h_autoscale eq 1 ) then begin 
  ;
  ; autoscale from the data
  ;
  z_min = min(zi)
  z_max = max(zi)
endif else begin
  ;
  ; here the user of this script could overwrite autoscale via changing
  ; the min and max functions to specific values
  ;
  z_min = 0.
  z_max = 1.44e-3
endelse

if (v_autoscale eq 1 ) then begin 
  ;
  ; autoscale from the data
  ;
  r_min = min(ri)
  r_max = max(ri)
endif else begin
  ;
  ; here the user of this script could overwrite autoscale via changing
  ; the min and max functions to specific values
  ;
  r_min = 0
  r_max = 2.4e-4
endelse

set_plot, 'PS'

; *****************************************************************
; Simple loop for rendering momentum plot on screen and to a file
; *****************************************************************

; Get a new window
;window_number = !d.window + 1
print, ' '
;print, 'Momentum plot will appear in window ', window_number
;window, window_number
ptcl_i = 0
ptcl_jump:

; Load in the STD GAMMA-II color table
loadct, 5

; *****************************************************************
; Specify a font that looks great for printing (crappy on screen),
;   or else one that looks OK on the screen (also OK for printing).
; To get greek fonts on the screen (with Hershey fonts), you must
;   specify !4, which maps "c" to "gamma".
; To get greek fonts in the file (with postscript fonts), you must
;   specify !9, which maps "g" to "gamma".
; The hack for x_label is similar.
; *****************************************************************

if (!d.name eq 'PS') then begin
  !p.font=1
  !p.charsize=1.6
  !p.charthick=1.5
  if ( ScaleFlag eq 1 ) then begin
    x_label = q1_label + ' (c/!9w!3!d0!N)'
    y_label = q2_label + ' (c/!9w!3!d0!N)'
  endif else if (ScaleFlag eq 2) then begin
    x_label = q1_label + ' / !9l!3!dp'
    y_label = q2_label + ' / !9l!3!dp' 
  endif
  print, ' '
  print, 'Writing the momentum plot to file ' + momentum_file
endif else begin
  !p.font=-1
  !p.charsize=2.0
 ; device,decomposed=0
  if ( ScaleFlag eq 1 ) then begin
    x_label = q1_label + ' (c/!9w!3!d0!N)'
    y_label = q2_label + ' (c/!9w!3!d0!N)'
  endif else if (ScaleFlag eq 2) then begin
    x_label = q1_label + ' / !4k!3!dp'
    y_label = q2_label + ' / !4k!3!dp' 
  endif
  print, ' '
  print, 'Rendering the momentum plot to the screen...'
endelse
  
plot, zi, ri, xtitle=x_label, ytitle=y_label, psym=3, $
      xrange=[z_min,z_max], yrange=[r_min,r_max], /xs, /ys

; rendering to screen is done.  here we render to ps file
if (ptcl_i eq 0) then begin
  ptcl_i = 1
;  set_plot, 'PS', /copy
  device, filename = momentum_file, /color, bits_per_pixel=8
  goto, ptcl_jump  
endif

; here rendering to file is done.  set back to screen defaults
if (ptcl_i eq 1) then begin
  device, /close
;  set_plot, 'X'
  !p.font=-1
  !p.charsize=2.0
  ;device,decomposed=0
endif

; *****************************************************************
; Final clean-up
; *****************************************************************

; Save so IDL doesn't have to repeatedly parse the ASCII file
save, data, filename = restore_file

; Stop here so that all variables will still be in scope.
;stop

; All done.
end
