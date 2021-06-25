#ifndef _SHUTDOWNMANAGER_H
#define _SHUTDOWNMANAGER_H

#include <stdbool.h>

void ShutdownManager_waitForShutdown();
void ShutdownManager_triggerShutdown();
bool ShutdownManager_isShuttingDown();

#endif