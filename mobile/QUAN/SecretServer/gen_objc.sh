mkdir -p objc
cp *.jce objc/
cd objc
/usr/local/taf/bin/jce2oc SecretBase.jce
cd ..
