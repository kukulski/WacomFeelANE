#!/bin/bash

TARGET=WacomANE

rm -f $TARGET.ane

FLEX_SDK=/Applications/Adobe\ Flash\ Builder\ 4.7/sdks/4.6.0
ADT=$FLEX_SDK/bin/adt
RELEASE_DIR=MacOS-x86/Build/Products/Release
DEBUG_DIR=MacOS-x86/Build/Products/Debug
RELEASE_FW=$RELEASE_DIR/$TARGET.framework
DEBUG_FW=$DEBUG_DIR/$TARGET.framework
if [ $DEBUG_FW -nt $RELEASE_FW ]
then
    FRAMEWORK_DIR=$DEBUG_DIR
else
    FRAMEWORK_DIR=$RELEASE_DIR
fi

echo $FRAMEWORK_DIR

rm -rf build
mkdir -p build/mac

cp -r $FRAMEWORK_DIR/$TARGET.framework build/mac
cp as3-library/MacOS-x86/extension.xml build
cp as3-library/MacOS-x86/bin/$TARGET.swc build
unzip -o -q build/$TARGET.swc library.swf
mv library.swf build/mac

"$ADT" -package \
	-target ane $TARGET build/extension.xml \
	-swc build/$TARGET.swc  \
	-platform MacOS-x86 \
	-C build/mac .
#	library.swf libIOSMightyLib.a
#	-platformoptions platformoptions.xml


if [ -f ./$TARGET.ane ];
then
    echo "SUCCESS"
	rm -rf build
else
    echo "FAILED"
fi

