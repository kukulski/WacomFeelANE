#!/bin/bash

TARGET=WacomANE

rm -f $TARGET.ane


FLEX_SDK=/Applications/Adobe\ Flash\ Builder\ 4.7/eclipse/plugins/com.adobe.flash.compiler_4.7.0.349722/AIRSDK
#FLEX_SDK=/Applications/Adobe\ Flash\ Builder\ 4.7/sdks/4.6.0
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

cp as3-library/MacOS-x86/bin/$TARGET.swc build/mac_lib.swc
unzip -o -q build/mac_lib.swc library.swf
mv library.swf build/mac

#mkdir -p build/default
#DEFAULT_LIB=as3-library/default/bin/"$TARGET"_default.swc
#echo "default lib:" $DEFAULT_LIB
#cp $DEFAULT_LIB build/default_lib.swc
#unzip -o -q build/default_lib.swc library.swf
#mv library.swf build/default

rm build/*.swc

"$ADT" -package \
	-target ane $TARGET build/extension.xml \
	-swc build/$TARGET.swc  \
	-platform MacOS-x86 -C build/mac . 



#"$ADT" -package \
#	-target ane $TARGET build/extension.xml \
#	-swc build/$TARGET.swc  \
#	-platform MacOS-x86 -C build/mac . \
#	-platform default -C build/default .

if [ -f ./$TARGET.ane ];
then
    echo "SUCCESS"
	rm -rf build
else
    echo "FAILED"
fi

