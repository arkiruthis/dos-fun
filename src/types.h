#ifndef TYPES_H
#define TYPES_H

// We are working with a signed 32-bit fixed-point value which is configured
// to offer 23 bits of integer (and 1 bit for sign) and 8 bits of mantissa
// [24:8]
typedef signed int fix;

#endif // TYPES_H