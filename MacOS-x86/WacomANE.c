

#import <AppKit/AppKit.h>
#include "WacomANE.h"
#include <stdio.h>
#include <string.h>
#include "FREHelpers.h"

#include <WacomMultiTouch/WacomMultiTouch.h>

#define nil NULL


FREObject gMouseBuffer;

FREContext gCtx;

typedef struct {
    int count;
    WacomMTFinger fingers[10];
} fingerPacket;

const int kRingBufferLength = 100;

int writeIdx = 0;
int readIdx = 0;
fingerPacket buffers[kRingBufferLength];

id _eventMonitor;

int ids[40];

typedef struct __attribute__((__packed__)) {
	long type;
	long tablet;
	long tool;
    
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
penPacket gPenPacket;




FREObject FEELE_sendEvent(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    const uint8_t *outVal;
    uint32_t outlen = 255;
    FREGetObjectAsUTF8(argv[0], &outlen, &outVal);
    
    FREDispatchStatusEventAsync(ctx, outVal, NULL);
    return NULL;
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



void makeEventMonitor();
void handleTappedEvent(NSEvent *e);
void wacomInit();
void wacomStart();
void wacomStop();
void wacomFinalize();





FREObject FEELE_init(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    
    FRESetContextActionScriptData(ctx, argv[0]);
    gCtx = ctx;
    
    makeEventMonitor();
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
    
    
    fingerPacket *p = &buffers[readIdx];
    readIdx = (readIdx+1)% kRingBufferLength;
    int count = p->count;
    
    setIntElement(countVectorPair, 0, count);
    for(int i = 0; i < count; i++) {
        FREObject touch = getElement(touches,i);
        FREObject onAirDesktop = getProperty(touch,"onAirDesktop");
        FREObject size = getProperty(touch,"size");

        
        WacomMTFinger *finger = &p->fingers[i];
      
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

//----------- event tap support


void makeEventMonitor() {
    
    _eventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:
                     (NSMouseMovedMask|NSTabletProximityMask|NSTabletPointMask|
                      NSLeftMouseDownMask|NSLeftMouseUpMask|NSLeftMouseDraggedMask)
                                                          handler:^(NSEvent *e) { handleTappedEvent(e);
                                                              return e;
                                                          }];
    [_eventMonitor retain];
}


void handleTappedEvent(NSEvent *e) {
    FREObject asObj;
    FREGetContextActionScriptData(gCtx, &asObj);
    FREObject toolMap = getToolMap(gCtx);
    FREObject pen = getPenObject(gCtx);
    
    CGEventRef event = e.CGEvent;
    CGEventType type = CGEventGetType(event);
    setIntProperty(pen, "type",type);
    
    // ground out if not a tablet event
    if(type != kCGEventTabletPointer && type != kCGEventTabletProximity) {
        int64_t subtypeVal = CGEventGetIntegerValueField(event, kCGMouseEventSubtype);
        if(subtypeVal != kCGEventMouseSubtypeTabletPoint)
            return;
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
    
    FREDispatchStatusEventAsync(gCtx, (const uint8_t *)"pen", (const uint8_t *)"");
}

//------------------  touch API support

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
    char strbuf[128];
    snprintf(strbuf,sizeof(strbuf),"%lf,%lf,%lf,%lf",deviceInfo.LogicalOriginX,deviceInfo.LogicalOriginY,deviceInfo.LogicalWidth, deviceInfo.LogicalHeight);
    FREDispatchStatusEventAsync(gCtx, (const uint8_t *)"device", (const uint8_t *)strbuf);
    
    WacomMTRegisterFingerReadCallback(deviceInfo.DeviceID, nil,  WMTProcessingModeObserver, MyFingerCallback, nil);
}

void MyDetachCallback(int deviceId, void *userInfo)
{
    WacomMTRegisterFingerReadCallback(deviceId, nil,  WMTProcessingModeNone, nil, nil);
}


int MyFingerCallback(WacomMTFingerCollection *packet, void *unused) {
    
    
    buffers[writeIdx].count = packet->FingerCount;
    memcpy(buffers[writeIdx].fingers, packet->Fingers, packet->FingerCount * sizeof(WacomMTFinger));
    
    writeIdx = (writeIdx + 1 ) % kRingBufferLength;
    
    FREDispatchStatusEventAsync(gCtx, (const uint8_t *)"touch", (const uint8_t *)"");
    
    return WMTErrorSuccess;
}



