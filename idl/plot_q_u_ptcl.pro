;------------------------------------------------------------------------
;
;  File:    plot_q_u_ptcl.pro
;
;  Purpose:
;    IDL procedure to read in q-u (coordinate component---velocity component)
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
;    XLabel     -> a string that specifies the coordinate component, e.g. "x".
;    VComponent -> a string that specifies the component of the velocity.
;    ScaleFlag  -> a flag to specify the scaling for length, i.e. of q:
;                        0 -> no scaling
;                        1 -> length scale = c/omega_0 (the laser frequency)
;                        2 -> length scale = lambda_p (the plasma wavelength)
;    ScaleInput -> this is irrelevant for ScaleFlag = 0,
;                          or omega_0 for ScaleFlag = 1,
;          plasma density in [1/cm^3] for ScaleFlag = 2
;
;  Version: $Id: plot_q_u_ptcl.pro 2071 2003-11-07 22:06:15Z bruhwile $
;
;  Copyright (c) 2000-2001 by Tech-X Corporation.  All rights reserved.
;
;  Change log:
;  1.00 (Bruhwiler 11-07-2000) Initial code.
;  1.01 (Bruhwiler 11-01-2001) Minor changes to limits, autoscaling
;
;------------------------------------------------------------------------


pro plot_q_u_ptcl, base_name, XLabel, VComponent, ScaleFlag, ScaleInput

; *****************************************************************
; Names for files and labels
; *****************************************************************

; Specify the text, restore and postscript file names
text_file     = base_name + '.txt'
restore_file  = base_name + '.dat'
momentum_file = base_name + '.ps'
energy_file   = base_name + '_e.ps'

; *****************************************************************
; Specify whether you want the gamma and/or energy plots and
;    how many particles should be plotted
; *****************************************************************

;
; set gammaPlot = 0  to skip the momentum plot
; set energyPlot = 0 to skip the energy plot
;
gammaPlot = 1
energyPlot = 1

;
; Specify the number of points to plot
;
num_points = 200000

; *****************************************************************
; Parse the ascii data file or restore from the binary IDL file
; *****************************************************************

; If the restore file exists, then use it.
; Otherwise, parse the text file:
spawn, "ls " + restore_file, check_file, /sh
print, 'The "check"   file is: ', check_file
print, 'The "restore" file is: ', restore_file
if (check_file(0) eq restore_file) then begin
  print,'Reading from the restore file: ' + restore_file + ' ...'
  restore, filename = restore_file
; NOTE: there is no need to reprocess all of the data
endif else begin
  print,'Parsing the text file: ' + text_file + ' ...'
  data=read_ascii(text_file)
endelse

print, ' '
print, 'Here is the size and shape of the raw data:'
help,data.field1

; *****************************************************************
; Initial manipulation of the data
; *****************************************************************

; Load the raw data into individual arrays.
z =data.field1(0,*)
r =data.field1(1,*)

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

; *****************************************************************
; Here we calculate the energy.
; *****************************************************************

; Note that r is gamma*velocity in m/s
; Thus, r/c = beta*gamma
c_light = 2.99792458e+08
me_c_sq = 510999.06
;
; the energy is calculated for electrons only
; change the mass to calculate for other particles
;
energy = (-1. + sqrt( 1. + (r/c_light)^2 ) ) * me_c_sq

;
; *****************************************************************
; Scale the data so it corresponds to the desired units
; *****************************************************************
;
q_label = '!3' + XLabel
if ( ScaleFlag eq 1 ) then begin
  ;
  ; scale the length using as a c/omega_0 as a length scale
  ; 
  omega_0 = ScaleInput
  lengthScale = 3.0e8/omega_0
  z = z/lengthScale
endif else if (ScaleFlag eq 2) then begin
  ;
  ; scale lengths to the plasma wavelength lambda_p
  ; 
  density = ScaleInput
  lambda_p = 3.0e+08 / ( 9000.0 * sqrt(density) )
  z = z / lambda_p
endif else begin
  ;
  ; this is the default case of no scaling 
  ;
endelse

;
; specify an exponent to extract from the momentum data
;
r_exponent = 8
e_exponent = 5

if ( r_exponent ne 0 ) then r = r / 10.^r_exponent
if ( e_exponent ne 0 ) then energy = energy / 10.^e_exponent

;
; set the nonmutable part of the labels
;
; Here, we specify the x, y and z axis labels.  Unfortunately, the
;   x, y labels must be handled differently for screen vs postscript.
;   See below for the actual implementation.  
;

if ( e_exponent ne 0 ) then begin
  e_label = '!3Kinetic Energy ( 10!u' + strtrim(string(e_exponent),2) + '!N eV)'
endif else begin
  e_label = '!3Kinetic Energy (eV)'
endelse

if ( r_exponent ne 0 ) then begin
  v_label = '!3v!d'+VComponent + '!N ( 10!u' + strtrim(string(r_exponent),2) + '!N m/s)'
endif else begin
  v_label = '!3v!d'+VComponent + '!N (m/s)'
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
  ei = energy
endif else begin
  print, ' '
  print, 'Particles are being culled --'
  print, '  Only ', num_points, ' particles will be plotted.'

  indices = findgen( num_points )
  indices = indices * (num_particles-1) / num_points
  zi = z(indices)
  ri = r(indices)
  ei = energy(indices)
endelse

;
; *****************************************************************
; set the min and max values of the data
; *****************************************************************
;
autoscale = 1
if (autoscale eq 1 ) then begin 
  ;
  ; autoscale from the data
  ;
  z_min = min(zi)
  z_max = max(zi)
  r_min = min(ri)
  r_max = max(ri)
  e_min = min(ei)
  e_max = max(ei)
endif else begin
  ;
  ; here the user of this script could overwrite autoscale via changing
  ; the min and max functions to specific values
  ;
  z_min =  0.0
  z_max =  5.3
  r_min =  0.0
  r_max =  2.0
  e_min =  0.0
  e_max =  1.0
endelse

z_min =  0.0
r_min =  0.0
e_min =  0.0


if (gammaPlot ne 0) then begin

; *****************************************************************
; Simple loop for rendering momentum plot on screen and to a file
; *****************************************************************

; Get a new window
window_number = !d.window + 1
print, ' '
print, 'Momentum plot will appear in window ', window_number
window, window_number, RETAIN=2
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
  !p.font=0
  !p.charsize=1.6
  !p.charthick=1.5
  if ( ScaleFlag eq 1 ) then begin
    x_label = q_label + ' (c/!9w!3!d0!N)'
  endif else if (ScaleFlag eq 2) then begin
    x_label = q_label + ' / !9l!3!dp'
  endif
  y_label = '!9g!3'+v_label
  print, ' '
  print, 'Writing the momentum plot to file ' + momentum_file
endif else begin
  !p.font=-1
  !p.charsize=2.0
  device,decomposed=0
  if ( ScaleFlag eq 1 ) then begin
    x_label = q_label + ' (c/!9w!3!d0!N)'
  endif else if (ScaleFlag eq 2) then begin
    x_label = q_label + ' / !4k!3!dp'
  endif
  y_label = '!4c!3'+v_label
  print, ' '
  print, 'Rendering the momentum plot to the screen...'
endelse
  
plot, zi, ri, xtitle=x_label, ytitle=y_label, psym=3 $
    , xrange=[z_min,z_max], yrange=[r_min,r_max], /xs, /ys

; rendering to screen is done.  here we render to ps file
if (ptcl_i eq 0) then begin
  ptcl_i = 1
  set_plot, 'PS', /copy
  device, filename = momentum_file, /color, bits_per_pixel=8
  goto, ptcl_jump  
endif

; here rendering to file is done.  set back to screen defaults
if (ptcl_i eq 1) then begin
  device, /close
  set_plot, 'X'
  !p.font=-1
  !p.charsize=2.0
  device,decomposed=0
endif

; end of gammaPlot loop
endif



if (energyPlot ne 0) then begin

; *****************************************************************
; Simple loop for rendering energy plot on screen and to a file
; *****************************************************************

; Get a new window
window_number = !d.window + 1
print, ' '
print, 'Energy plot will appear in window ', window_number
window, window_number, RETAIN=2
energy_i = 0
energy_jump:

; Load in the STD GAMMA-II color table
loadct, 5

; Specify a font that looks great for printing (crappy on screen),
;   or else one that looks OK on the screen (also OK for printing).
if (!d.name eq 'PS') then begin
  !p.font=1
  !p.charsize=1.6
  !p.charthick=1.5
  if ( ScaleFlag eq 1 ) then begin
    x_label = q_label + ' (c/!9w!3!d0!N)'
  endif else if (ScaleFlag eq 2) then begin
    x_label = q_label + ' / !9l!3!dp'
  endif
  print, ' '
  print, 'Writing the energy plot to file ' + energy_file
endif else begin
  !p.font=-1
  !p.charsize=2.0
  device,decomposed=0
  if ( ScaleFlag eq 1 ) then begin
    x_label = q_label + ' (c/!9w!3!d0!N)'
  endif else if (ScaleFlag eq 2) then begin
    x_label = q_label + ' / !4k!3!dp'
  endif
  print, ' '
  print, 'Rendering the energy plot to the screen...'
endelse

plot, zi, ei, xtitle=x_label, ytitle=e_label, psym=3, $
      xrange=[z_min,z_max], yrange=[e_min,e_max], /xs

; rendering to screen is done.  here we render to ps file
if (energy_i eq 0) then begin
  energy_i = 1
  set_plot, 'PS', /copy
  device, filename = energy_file, /color, bits_per_pixel=8
  goto, energy_jump  
endif

; here rendering to file is done.  set back to screen defaults
if (energy_i eq 1) then begin
  device, /close
  set_plot, 'X'
  !p.font=-1
  !p.charsize=2.0
  device,decomposed=0
endif

; end of energyPlot loop
endif

; *****************************************************************
; Final clean-up
; *****************************************************************

; Save so IDL doesn't have to repeatedly parse the ASCII file
save, data, filename = restore_file

; Stop here so that all variables will still be in scope.
; stop

; All done.
end
