make cleanall
if [ -e  imdir ] ;then
rm -r imdir
fi
mkdir imdir
/data/coverity/cov-sa-linux-5.5.1/bin/cov-build --dir imdir  make all -j8
