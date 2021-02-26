
foreach i ( *.cpp *.C *.h )
  rcs -i -t-"Initial revision" -apeterm,kc,johnv,venkates,cooperd $i
  ci -u $i
end
