/**
 * @file      hardware.h
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */

#ifndef PROJECT_HEADERS_HARDWARE_H_
#define PROJECT_HEADERS_HARDWARE_H_
 /*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
namespace USBDM {

enum ErrorCode {
   E_NO_ERROR = 0,   // No error
   E_TOO_SMALL,      // Value too small
   E_TOO_LARGE,      // Value too large
};

/** Last error set by USBDM code */
extern ErrorCode errorCode;

/** Set error code */
inline static ErrorCode setErrrCode(ErrorCode err) {
   errorCode = err;
   return errorCode;
}

/**
 * Get error message from error code or last
 * error if not provided
 *
 * @param  err Error code
 */
const char *getErrorMessage(ErrorCode err = errorCode);

} // End namespace USBDM

#include "pin_mapping.h"

#endif /* PROJECT_HEADERS_HARDWARE_H_ */
