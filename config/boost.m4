dnl ######################################################################
dnl
dnl File:	boost.m4
dnl
dnl Purpose:	Determine the locations of boost includes and libraries.
dnl
dnl
dnl Version: $Id: boost.m4 2127 2006-12-19 01:32:46Z mmiah $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl Includes functions from txsearch.m4
builtin(include,config/txsearch.m4)

dnl ######################################################################
dnl
dnl find the location of the boost library
dnl
dnl ######################################################################

case "$hosttype" in

*-apple-darwin*)
  TX_LOCATE_PKG(
    [boost],
    [/usr/local/boost:/opt/boost/:/home/research/mmiah/tools/boost_1_33_1/build-storage2],
    [boost],
    [boost_date_time,boost_filesystem,boost_iostreams,boost_python,boost_regex,boost_serialization,boost_signals,boost_wserialization],
    [include/boost-1_33_1],
    [lib])
    ;;
*)
  TX_LOCATE_PKG(
    [boost],
    [/usr/local/boost:/opt/boost/:/home/research/mmiah/tools/boost_1_33_1/build-storage2],
    [boost],
    [boost_date_time-gcc,boost_filesystem-gcc,boost_iostreams-gcc,boost_python-gcc,boost_regex-gcc,boost_serialization-gcc,boost_signals-gcc,boost_wserialization-gcc],
    [include/boost-1_33_1],
    [lib])
    ;;

esac

builtin(include,config/python.m4)

