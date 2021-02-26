# Extract BSD-compatible nm

if test -z "$NM"; then
  echo $ac_n "checking for BSD-compatible nm... $ac_c" 1>&6
  case "$NM" in
  [\\/]* | [A-Za-z]:[\\/]*) ;; # Let the user override the test with a path.
  *)
    IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR}"
    for ac_dir in $PATH /usr/ucb /usr/ccs/bin /bin; do
      test -z "$ac_dir" && ac_dir=.
      if test -f $ac_dir/nm || test -f $ac_dir/nm$ac_exeext; then
	# Check to see if the nm accepts a BSD-compat flag.
	# Adding the `sed 1q' prevents false positives on HP-UX, which says:
	#   nm: unknown option "B" ignored
	if ($ac_dir/nm -B /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	  NM="$ac_dir/nm -B"
	  break
	elif ($ac_dir/nm -p /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
	  NM="$ac_dir/nm -p"
	  break
	else
	  NM=${NM="$ac_dir/nm"} # keep the first match, but
	  continue # so that we can try to find one that supports BSD flags
	fi
      fi
    done
    IFS="$ac_save_ifs"
    test -z "$NM" && NM=nm
    ;;
  esac
  echo "$ac_t$NM" 1>&6
fi

#
# Tar compatiblity with AUTOMAKE 1.5 && 1.4
# AUTOMAKE 1.4 does not have this symbol defined
#
AMTAR=tar

