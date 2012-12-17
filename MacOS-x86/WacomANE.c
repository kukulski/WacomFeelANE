

#import <AppKit/AppKit.h>
#include "WacomANE.h"
#include <stdio.h>
#include <string.h>

#include <WacomMultiTouch/WacomMultiTouch.h>

#define nil NULL

typedef struct __attribute__((__packed__)) {
	long type;
	long tablet;
	long tool;

    // we could totally get by with floats instead of doubles
    double x;
	double y;
	long z;
	long buttons;
	double pressure;
	double tiltX;
	double tiltY;
	double rotation;
	double tangentialPressure;
} penPacket;


typedef struct __attribute__((__packed__)) {
    int							FingerID;
    float							X;
    float							Y;
    float							Width;
    float							Height;
    unsigned short				Sensitivity;
    float							Orientation;
    char							Confidence;
    char                        TouchState;
} packedPacket;


FREContext gCtx;

FREObject gMouseBuffer;


size_t gLen;
packedPacket packedBuffer[20];
int ids[40];

penPacket gPenPacket;


void MyAttachCallback(WacomMTCapability deviceInfo, void *userInfo);
void MyDetachCallback(int deviceID, void *userInfo);
int MyFingerCallback(WacomMTFingerCollection *packet, void *unused);

void wacomInit() {
    WacomMTError err = WacomMTInitialize(WACOM_MULTI_TOUCH_API_VERSION);
    if(err !=  WMTErrorSuccess ) return;
 }

void wacomStop() {
 
    int count = WacomMTGetAttachedDeviceIDs(ids,sizeof(ids));
    for(int i = 0; i < count; i++) {
        MyDetachCallback(ids[i],nil);
    }
}


void wacomFinalize() {
    WacomMTQuit();
}

void wacomStart() {
  
    WacomMTRegisterAttachCallback(MyAttachCallback, nil);
    WacomMTRegisterDetachCallback(MyDetachCallback, nil);

    
    int ids[100];
    int count = WacomMTGetAttachedDeviceIDs(ids,sizeof(ids));
    for(int i = 0; i < count; i++) {
        WacomMTCapability buf;
        WacomMTGetDeviceCapabilities(ids[i], &buf);
        MyAttachCallback(buf,nil);
    }
}


void MyAttachCallback(WacomMTCapability deviceInfo, void *userInfo)
{
    WacomMTRegisterFingerReadCallback(deviceInfo.DeviceID, nil,  WMTProcessingModeNone, MyFingerCallback, nil);
}

void MyDetachCallback(int deviceId, void *userInfo)
{
    WacomMTRegisterFingerReadCallback(deviceId, nil,  WMTProcessingModeNone, nil, nil);
}


int MyFingerCallback(WacomMTFingerCollection *packet, void *unused) {
    
    for(int i=0; i < packet->FingerCount; i++) {
        WacomMTFinger *from = &packet->Fingers[i];
        packedPacket *to = &packedBuffer[i];
        
        
        to->FingerID = from->FingerID;
        to->X = from->X;
        to->Y = from->Y;
        to->Width = from->Width;
        to->Height = from->Height;
        to->Sensitivity = from->Sensitivity;
        to->Orientation = from->Orientation;
        to->Confidence = from->Confidence;
        to->TouchState = from->TouchState;
    }

    gLen = packet->FingerCount * sizeof(packedPacket);
    
    return WMTErrorSuccess;
}

id _eventMonitor;

FREObject FEELE_sendEvent(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    const uint8_t *outVal;
    uint32_t outlen = 255;
    FREGetObjectAsUTF8(argv[0], &outlen, &outVal);
    
    FREDispatchStatusEventAsync(ctx, outVal, NULL);
    return NULL;
}

void setNumberProperty(FREObject obj, const char *name, double value) {
    FREObject temp, exception;
    FRENewObjectFromDouble(value,  &temp);
    FRESetObjectProperty(obj, (const uint8_t *)name, temp, &exception);
}

void setIntProperty(FREObject obj, const char *name, int value) {
    FREObject temp, exception;
    FRENewObjectFromInt32(value, &temp);
    FRESetObjectProperty(obj, (const uint8_t *)name, temp, &exception);
}

void setIntElement(FREObject obj, int index, int value) {
    FREObject temp;
    FRENewObjectFromInt32(value, &temp);
    FRESetArrayElementAt(obj,index, temp);
}


FREObject getExchangeObject(FREContext ctx) {
    FREObject buffer;
    FREGetContextActionScriptData(ctx, &buffer);
    return buffer;
}

void setProperty(FREObject obj, const char *name, FREObject val) {
    FREObject ignored;
    FRESetObjectProperty(obj,(const uint8_t *)name, val, &ignored);
}

FREObject getProperty(FREObject obj, const char *name) {
    FREObject rval, exception;
    FREGetObjectProperty(obj,
                         (const uint8_t *)name, &rval, &exception);
    return rval;
}
FREObject getElement(FREObject obj, int idx) {
    FREObject rval;
    FREGetArrayElementAt(obj,idx, &rval);
    return rval;
}

FREObject getPenObject(FREContext ctx) {
    return getProperty(getExchangeObject(ctx), "pen");
}
FREObject getToolMap(FREContext ctx) {
    return getProperty(getExchangeObject(ctx), "toolMap");
}

FREObject getTouchArray(FREContext ctx){
    FREObject rval, exception;
    FREGetObjectProperty(getExchangeObject(ctx),
                         (const uint8_t *)"touches", &rval, &exception);
    return rval;
}

void setFloatEventProperty(FREObject obj, const char *name, CGEventRef event, CGEventField fieldname ) {

    double val = CGEventGetDoubleValueField(event,fieldname);
    setNumberProperty(obj,name,val);
}

FREObject FEELE_init(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    
    FRESetContextActionScriptData(ctx, argv[0]);
    
//    wacomInit();
    
//	FREObject retVal;
//	FRENewObjectFromBool(1,&retVal);

	
    
    _eventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:
                     (NSTabletPointMask | NSTabletProximityMask |NSMouseMovedMask)
                                                          handler:^(NSEvent *e) {
                     NSEvent *result = e;
                     FREObject asObj;
                     FREGetContextActionScriptData(ctx, &asObj);
                     FREObject toolMap = getToolMap(ctx);
                     FREObject pen = getPenObject(ctx);
                      
                     CGEventRef event = e.CGEvent;
                     CGEventType type = CGEventGetType(event);
                     setIntProperty(pen, "type",type);
                     
             
                     int tablet = CGEventGetIntegerValueField(event, kCGTabletProximityEventSystemTabletID);
                     
                     if(type == kCGEventTabletProximity) {
                        int tool = CGEventGetIntegerValueField(event, kCGTabletProximityEventPointerType);
                        setIntElement(toolMap, tablet, tool);
                     }
                     CGPoint location = CGEventGetLocation(event);
                     
                     

                     gPenPacket.x = location.x;
                      gPenPacket.y = location.y;

                      gPenPacket.z = CGEventGetIntegerValueField(event, kCGTabletEventPointZ);
                      gPenPacket.buttons =  CGEventGetIntegerValueField(event, kCGTabletEventPointButtons);
                      gPenPacket.pressure = CGEventGetDoubleValueField(event, kCGTabletEventPointPressure);
                      gPenPacket.tiltX = CGEventGetDoubleValueField(event, kCGTabletEventTiltX);
                      gPenPacket.tiltY = CGEventGetDoubleValueField(event, kCGTabletEventTiltY);
                      gPenPacket.rotation = CGEventGetDoubleValueField(event, kCGTabletEventRotation);
                      gPenPacket.tangentialPressure = CGEventGetDoubleValueField(event, kCGTabletEventTangentialPressure);
                      gPenPacket.tablet = tablet;
                     
                                    char buf[100];
                     snprintf(buf, sizeof(buf), "event location %f,%f",location.x, location.y);
                     FREDispatchStatusEventAsync(ctx, (const uint8_t *)"aMousefl", (const uint8_t *)buf);
                     
                     FRESetContextActionScriptData(ctx, pen);
                     return result;
                     }];

    [_eventMonitor retain];
    
    
    FREObject retVal;
	FRENewObjectFromUTF8(3, (const uint8_t *)"foo",&retVal);

    return retVal;
    
    
    
    
    
    
    
}


FREObject FEELE_getdata(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    size_t length = gLen * sizeof(packedPacket);
    gCtx = ctx;
    FREObject buffer;
    FREObject exception;
    FRENewObject((const uint8_t *)"flash.utils.ByteArray", 0, nil, &buffer, &exception);
    
    FREObject lengthObj;
    FRENewObjectFromUint32(length, &lengthObj);
    FRESetObjectProperty(buffer, (const uint8_t *)"length", lengthObj, &exception);
  
    FREByteArray array;
    FREAcquireByteArray(buffer, &array);
  
    memcpy(array.bytes, packedBuffer, length);
    FREReleaseByteArray(buffer);
 
    return buffer;
}

FREObject FEELE_getPenData(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    
    // we may have to stash the pen data into the struct on our event
    // and copy it out to our rval in this call
    
    FREObject pen = getPenObject(ctx);
    
    
    FREObject onAirDesktop = getProperty(pen,"onAirDesktop");
    FREObject tilt = getProperty(pen,"tilt");
    FREObject toolMap = getToolMap(ctx);
    
    
    setNumberProperty(onAirDesktop, "x", gPenPacket.x);
    setNumberProperty(onAirDesktop, "y", gPenPacket.y);
    setIntProperty(pen,"z", gPenPacket.z);
    setIntProperty(pen,"buttons",gPenPacket.buttons);
    setNumberProperty(pen,"pressure", gPenPacket.pressure);
    setNumberProperty(tilt,"x", gPenPacket.tiltX);
    setNumberProperty(tilt,"y", gPenPacket.tiltY);
    setNumberProperty(pen,"rotation", gPenPacket.rotation);
    setNumberProperty(pen,"tangentialPressure", gPenPacket.tangentialPressure);
    setIntProperty(pen,"tablet", gPenPacket.tablet);
    setProperty(pen, "tool", getElement(toolMap, gPenPacket.tablet));
    
    return pen;
}

void reg(FRENamedFunction *store, int slot, const char *name, FREFunction fn) {
    store[slot].name = (const uint8_t*)name;
    store[slot].functionData = NULL;
    store[slot].function = fn;
}


void contextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions)
{
  *numFunctions = 3;
  FRENamedFunction* func = (FRENamedFunction*) malloc(sizeof(FRENamedFunction) * (*numFunctions));
  *functions = func;

    int i = 0;
    reg(func,i++,"getData",FEELE_getdata);
    reg(func,i++,"getPenData",FEELE_getPenData);
    reg(func,i++,"init",FEELE_init);
    
////    
////  func[0].name = (const uint8_t*) "touchStart";
////  func[0].functionData = NULL;
////  func[0].function = FEELE_start;
//
//    func[1].name = (const uint8_t*) "getData";
//    func[1].functionData = NULL;
//    func[1].function = FEELE_getdata;
//    
//    func[2].name = (const uint8_t*) "touchStop";
//    func[2].functionData = NULL;
//    func[2].function = FEELE_stop;
//    
//    func[3].name = (const uint8_t*) "sendEvent";
//    func[3].functionData = NULL;
//    func[3].function = FEELE_sendEvent;
//    
//    func[4].name = (const uint8_t*) "init";
//    func[4].functionData = NULL;
//    func[4].function = FEELE_init;
}


    


void contextFinalizer(FREContext ctx)
{
    wacomFinalize();
   return;
}

void WacomANEinitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
{
  *ctxInitializer = &contextInitializer;
  *ctxFinalizer = &contextFinalizer;
 *extData = NULL;

}

void WacomANEfinalizer(void* extData)
{
	FREContext nullCTX;
	nullCTX = 0;
	
    
    wacomStop();
	contextFinalizer(nullCTX);
	
	return;
}

