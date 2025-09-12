#pragma once
#include "rigtransportcommon_export.h"
#include <stdint.h>

extern "C"
{
/// @brief Find the functions in the current process and return an API index to use to call them.
/// @param functionList A list of function names to find.
/// @param functionCount The number of functions in the list.
/// @param outApiIdx The API index to use to call the functions.
/// @return An error code, or 0 on success.
RIGTRANSPORTCOMMON_EXPORT int32_t rigtransport_inprocess_findfunctions(const char* const* functionList, uint32_t functionCount, uint32_t* outApiIdx);
/// @brief Release the functions associated with the given API index.
/// @param apiIdx The API index to release.
/// @return An error code, or 0 on success.
RIGTRANSPORTCOMMON_EXPORT int32_t rigtransport_inprocess_releasefunctions(uint32_t apiIdx);
/// @brief Call a function in the current process.
/// If dataSize >= outDataSize, the function will write the data to the input buffer, which does not need to be freed.
/// @param apiIdx The API index returned by rigtransport_inprocess_findfunctions.
/// @param functionId The function index in the list passed to rigtransport_inprocess_findfunctions.
/// @param data The input data to pass to the function.
/// @param dataSize The size of the input data.
/// @param outData A pointer to a buffer to receive output data.
/// @param outDataSize A pointer to a variable to receive the size of the output data.
/// @return An error code, or 0 on success. If error code < 0, the outData will contain the error string.
RIGTRANSPORTCOMMON_EXPORT int32_t rigtransport_inprocess_callfunction(uint32_t apiIdx, uint32_t functionId, uint8_t* data, uint32_t dataSize, uint8_t** outData, uint32_t* outDataSize);
/// @brief Free memory allocated by rigtransport_inprocess_callfunction.
/// @param data The memory to free.
RIGTRANSPORTCOMMON_EXPORT void rigtransport_inprocess_free(uint8_t* data);
/// @brief Get a string representation of an error code.
/// @param errorCode The error code to get the string for.
/// @return A string representation of the error code. Do not free the returned string.
RIGTRANSPORTCOMMON_EXPORT const char* rigtransport_inprocess_geterrorstring(int32_t errorCode);
}
