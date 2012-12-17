//
//  FREHelpers.h
//  WacomANE
//
//  Created by Timothy Kukulski on 12/17/12.
//  Copyright (c) 2012 Nick Kwiatkowski. All rights reserved.
//

#ifndef WacomANE_FREHelpers_h
#define WacomANE_FREHelpers_h

#include "FlashRuntimeExtensions.h"

void reg(FRENamedFunction *store, int slot, const char *name, FREFunction fn);

void setNumberProperty(FREObject obj, const char *name, double value);
void setIntProperty(FREObject obj, const char *name, int value);
void setIntElement(FREObject obj, int index, int value);
FREObject getExchangeObject(FREContext ctx);
void setProperty(FREObject obj, const char *name, FREObject val);
FREObject getProperty(FREObject obj, const char *name);
FREObject getElement(FREObject obj, int idx);

#endif
