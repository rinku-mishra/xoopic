echo " rm -f configure config.cache Makefile.in */Makefile.in */*/Makefile.in"
rm -f configure config.cache Makefile.in */Makefile.in */*/Makefile.in
echo "cvs update"
cvs -z 9 update
echo "touching Makefile.am"
touch xg/Makefile.am otools/Makefile.am physics/Makefile.am advisor/Makfile.am
