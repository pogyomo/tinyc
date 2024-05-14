#ifndef TINYC_PANIC_H_
#define TINYC_PANIC_H_

// Report an error which is caused by user input and finish program.
void panic(const char* format, ...);

// Report an error which is caused by internal logic and finish program.
void panic_internal(const char* format, ...);

#endif  // TINYC_PANIC_H_
