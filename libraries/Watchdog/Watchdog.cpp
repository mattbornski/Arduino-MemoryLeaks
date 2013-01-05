#include <MemoryFree.h>
#include "Watchdog.h"
#include <avr/wdt.h>

// Remove any configured watchdog so we can complete the boot process without
// getting reset.
void _wdt_clear(void) {
  MCUSR = 0;
  wdt_disable();
}

void softReset() {
  wdt_enable(WDTO_15MS);
  for (;;) {}
}

int __watchdog_bytes = -1;
void softResetMem(int bytes) {
  __watchdog_bytes = bytes;
}

void softResetTimeout() {
  // TODO add argument for configurable timeout length, and handle appropriately.
  wdt_enable(WDTO_8S);
}

void heartbeat() {
  wdt_reset();
  if (__watchdog_bytes >= 0) {
    int bytesFree = freeMemory();
    if (bytesFree < __watchdog_bytes) {
      softReset();
    }
  }
}