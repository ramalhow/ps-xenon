#include "Log.hpp"

#include "CommonXbox.h"

namespace Log {
void GenericLog(const char* message) {
    char outputBuffer[512] = {};

	static_assert(sizeof(message) < 512, "the log is too big");

    wsprintf(outputBuffer, "%s:%u %s", __FILE__, __LINE__, message);

    OutputDebugString(outputBuffer); 
}

} // namespace Log