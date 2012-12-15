#!/bin/bash

# set for debug
# set -xv

TARGET=WacomANE

rm -f $WacomANE

FLEX_SDK=/Applications/Adobe\ Flash\ Builder\ 4.7/sdks/4.6.0
ADT=$FLEX_SDK/bin/adt

echo $FLEX_SDK
echo $ADT

rm -rf mac
rm WacomMultitouch.swc 
mkdir -p mac

cp -r ./native-extension/MacOS-x86/build/Products/Release/WacomANE.framework mac
cp ./WacomMultiTouchANE-as3/extension.xml .
# cp ./MightyController/src/platformoptions.xml .
cp ./WacomMultiTouchANE-as3/bin/WacomMultitouch.swc .
unzip -o -q WacomMultitouch.swc library.swf
mv library.swf mac

"$ADT" -package \
	-target ane WacomANE extension.xml \
	-swc ./WacomMultiTouchANE-as3/bin/WacomMultitouch.swc  \
	-platform MacOS-x86 \
	-C mac .
#	library.swf libIOSMightyLib.a
#	-platformoptions platformoptions.xml

rm -f extension.xml
rm -f platformoptions.xml
rm -f MightyController.swc
rm -f libIOSMightyLib.a
rm -f library.swf

if [ -f ./$TARGET.ane ];
then
    echo "SUCCESS"
else
    echo "FAILED"
fi

