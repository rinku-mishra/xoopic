FUNCTION  temperature_2d,  x,  y,  v, weight,  dimx,  dimy,      $
                           XAXIS =xaxis,  YAXIS =yaxis,  $
                           XRANGE=xrange, YRANGE=yrange, $
                           VMEAN=vmean, DV_MAX=dv_max, N_MACRO=n_macro
;+
; PURPOSE:
;  	Determine the mean velocity and standard deviation of a quantity v(x,y)
;       as a function of the associated position x and y. 
;
; INPUT: 
;       x, y      : coordinates of each particle
;       v         : velocity of each particle
;       weight    : # of physical particles represented by each macro-particle
;       dimx, dimy: Dimensions of the result plane
;       
; KEYWORDS:
;       XAXIS, YAXIS: returns vectors with the axis labels of the result
;                     plane
;       XRANGE, YRANGE: 2 element vectors, determin the minimum and maximum
;                       position of the result plane. If not specified, the
;                       ranges are determined based on the input data. 
;  
;       VMEAN:        returns the mean value (drift velocity) at each location. 
;       DV_MAX:       returns the maximum of abs(V-Vmean)
;       N_MACRO:      returns the number of macro-particles used in each bin
;
;-

 n_macro = intarr(dimx,  dimy)
 n_ptcls = intarr(dimx,  dimy)
 vi      = dblarr(dimx,  dimy)
 vmean   = dblarr(dimx,  dimy)
 dv_max  = dblarr(dimx,  dimy)
 nx =  n_elements(x)

 IF NOT keyword_set(XRANGE) THEN BEGIN
     xrange =  [ min(x),  max(x)]
 ENDIF ELSE BEGIN
     ind =  where(x LT xrange[0])
     IF ind(0) NE -1 THEN x(ind)=xrange[0]
     ind =  where(x GT xrange[1])
     IF ind(0) NE -1 THEN x(ind)=xrange[1]
 ENDELSE

 IF NOT keyword_set(YRANGE) THEN BEGIN
    yrange =  [ min(y),  max(y)]
 ENDIF ELSE BEGIN
     ind =  where(y LT yrange[0])
     IF ind(0) NE -1 THEN y(ind)=yrange[0]
     ind =  where(y GT yrange[1])
     IF ind(0) NE -1 THEN y(ind)=yrange[1]
 ENDELSE

 eps =  1
 scx = (float(dimx)-eps) /float(xrange(1) - xrange(0))
 scy = (float(dimy)-eps) /float(yrange(1) - yrange(0))

 xaxis = lindgen(dimx) / scx + xrange(0)
 yaxis = lindgen(dimy) / scy + yrange(0)

; print, 'Determine mean velocities'

 n_ptcls = n_ptcls * 0.
 n_macro = n_macro * 0

 FOR i=0L,  nx-1 DO BEGIN
   indx =  floor(scx*(x(i)-xrange(0)))
   indy =  floor(scy*(y(i)-yrange(0)))
   vmean(indx,  indy) = vmean(indx,  indy)  + weight(i) * v(i)
   n_macro(indx, indy) = n_macro(indx,  indy)  + 1
   n_ptcls(indx, indy) = n_ptcls(indx,  indy)  + weight(i)

   dv_max(indx, indy) = 0.
 END
 ind = where(n_ptcls NE 0.) 
 vmean(ind) =  vmean(ind) / n_ptcls(ind)

; print,  'Determine standard deviations'

 n_ptcls = n_ptcls * 0.
 n_macro = n_macro * 0

 FOR i=0L,  nx-1 DO BEGIN
   indx =  floor(scx*(x(i)-xrange(0)))
   indy =  floor(scy*(y(i)-yrange(0)))
   vi(indx,  indy) = vi(indx,  indy)  + $
                     weight(i) * ( v(i) - vmean(indx,  indy) )^2
   n_macro(indx, indy) = n_macro(indx,  indy)  + 1
   n_ptcls(indx, indy) = n_ptcls(indx,  indy)  + weight(i)

   if ( abs( v(i)-vmean(indx,indy) ) GT dv_max(indx, indy) ) then begin
     dv_max(indx, indy) = abs( v(i)-vmean(indx, indy) )
   endif
 END
 
 ind =  where(n_ptcls GT 1.)
 vi(ind) =  sqrt( vi(ind) / (n_ptcls(ind)-1.) )

 return,  vi
END 
