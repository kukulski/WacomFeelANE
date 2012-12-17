

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



FREObject gMouseBuffer;

FREContext gCtx;
int gFingerCount;
WacomMTFinger gFingerBuffer[20];
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
    WacomMTRegisterFingerReadCallback(deviceInfo.DeviceID, nil,  WMTProcessingModeObserver, MyFingerCallback, nil);
}

void MyDetachCallback(int deviceId, void *userInfo)
{
    WacomMTRegisterFingerReadCallback(deviceId, nil,  WMTProcessingModeNone, nil, nil);
}


int MyFingerCallback(WacomMTFingerCollection *packet, void *unused) {
    
    gFingerCount = packet->FingerCount;
    memcpy(gFingerBuffer, packet->Fingers, gFingerCount * sizeof(WacomMTFinger));
    
    
    FREDispatchStatusEventAsync(gCtx, (const uint8_t *)"touch", (const uint8_t *)"");
    
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
                     (NSMouseMovedMask|NSTabletProximityMask|NSTabletPointMask|
                      NSLeftMouseDownMask|NSLeftMouseUpMask|NSLeftMouseDraggedMask)
                                                          handler:^(NSEvent *e) {
                     NSEvent *result = e;
                     FREObject asObj;
                     FREGetContextActionScriptData(ctx, &asObj);
                     FREObject toolMap = getToolMap(ctx);
                     FREObject pen = getPenObject(ctx);
                      
                     CGEventRef event = e.CGEvent;
                     CGEventType type = CGEventGetType(event);
                     setIntProperty(pen, "type",type);
                     
                      // ground out if not a tablet event
                      if(type != kCGEventTabletPointer && type != kCGEventTabletProximity) {
                          int64_t subtypeVal = CGEventGetIntegerValueField(event, kCGMouseEventSubtype);
                          if(subtypeVal != kCGEventMouseSubtypeTabletPoint) 
                              return result;
                      }
                                                              
             
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
                     
                     FREDispatchStatusEventAsync(ctx, (const uint8_t *)"pen", (const uint8_t *)"");
                       return result;
                     }];

    [_eventMonitor retain];
    
    gCtx = ctx;
    wacomInit();
    wacomStart();
    
    
    FREObject retVal;
	FRENewObjectFromUTF8(3, (const uint8_t *)"foo",&retVal);

    return retVal;
 
}


FREObject FEELE_getTouchData(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    
    FREObject countVectorPair = getTouchArray(ctx);
    FREObject touches = getElement(countVectorPair, 1);
    
    setIntElement(countVectorPair, 0, gFingerCount);
    for(int i = 0; i < gFingerCount; i++) {
        FREObject touch = getElement(touches,i);
        FREObject onAirDesktop = getProperty(touch,"onAirDesktop");
        FREObject size = getProperty(touch,"size");

        
        WacomMTFinger *finger = &gFingerBuffer[i];
      
        setIntProperty(touch, "id", finger->FingerID);
        setIntProperty(touch, "confidence", finger->Confidence);
        setIntProperty(touch, "state", finger->TouchState);
        setIntProperty(touch, "sensitivity",finger->Sensitivity);
        
        setNumberProperty(onAirDesktop, "x", finger->X);
        setNumberProperty(onAirDesktop, "y", finger->Y);
   
        setNumberProperty(size, "x", finger->Width);
        setNumberProperty(size, "y", finger->Height);
    }
    return countVectorPair;
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
    reg(func,i++,"getTouchData",FEELE_getTouchData);
    reg(func,i++,"getPenData",FEELE_getPenData);
    reg(func,i++,"init",FEELE_init);
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

