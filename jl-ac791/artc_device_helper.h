//
// Created by Pi on 2025/06/11.
//

#ifndef artc_device_helper_h
#define artc_device_helper_h

#include "event/device_event.h"

bool ARTCWaitSDCardReady(int timeoutInSeconds);

void ARTCSetupWifi(void);
bool ARTCIsWifiConnected();
bool ARTCWaitWifiConnected(int timeoutInSeconds);

#endif /* artc_device_helper_h */