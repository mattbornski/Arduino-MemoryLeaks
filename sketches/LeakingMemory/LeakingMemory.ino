#include <MemoryFree.h>

// In this contrived test case, leak size granularity can affect whether
// the symptom (failure to perform task) appears or not.  If the leak size
// is much greater than the memory required to actually perform the task,
// then instead of failing to perform the task due to allocation problems,
// we'll fail to leak the last bit of memory due to allocation problems.
// Leak size is in bytes.
#define LEAK_SIZE 8
// Just for the benefit of the humans following along at home.  In ms.
#define ITERATION_DELAY 500
// Non-serial feedback
#define LED_PIN 13

unsigned long iteration;

void setup() {
  // Use LED for non-serial feedback about what's happening
  pinMode(LED_PIN, OUTPUT);
  // Hold LED solid for a second
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  // Thrash LED
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
  digitalWrite(LED_PIN, LOW);
  
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  unsigned long serialBegin = millis();
  while (!Serial && (millis() - serialBegin) < 5000) {
    // Wait for serial port to connect. Needed for Leonardo only.
    // I only give you 5 seconds to get it together though, because
    // otherwise the Leonardo is not very useful for embedded applications.
    ;
  }
  
  iteration = 0;
  
  Serial.println("Hello world");
}

boolean leak(int bytes) {
  // Leak heap.  You can't really leak stack.
  // Just for fun let's fragment it too.
  char *fragment = (char *)malloc(bytes);
  char *leak = NULL;
  if (fragment != NULL) {
    leak = (char *)malloc(bytes);
    free(fragment);
  }
  return (leak != NULL);
}

boolean performTask() {
  // Perform something which requires both stack and heap allocations
  char *reference = (char *)malloc(128);
  strcpy(reference, "foo0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999");
  char foo[128];
  for (int i = 0; i <= strlen(reference); i++) {
    foo[i] = reference[i];
  }
  boolean success = (strcmp(foo, reference) == 0);
  free(reference);
  return success;
}

void loop() {
  Serial.print("Iteration #");
  Serial.println(++iteration);
  
  // Leak some memory
  Serial.print("Leaking ");
  Serial.print(LEAK_SIZE);
  Serial.print(" bytes: ");
  if (leak(LEAK_SIZE)) {
    Serial.println("Succeeded!");
  } else {
    Serial.println("Failed!");
  }
  
  // Attempt to perform a task which requires the allocation of memory
  Serial.print("Attempting to perform our memory-using task: ");
  if (performTask()) {
    Serial.println("Succeeded!");
  } else {
    Serial.println("Failed!");
  }
  
  Serial.print("Free memory: ");
  Serial.print(freeMemory());
  Serial.println(" bytes");
  Serial.println("");
  
  // Pulse LED on
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  
  delay(ITERATION_DELAY);
}
