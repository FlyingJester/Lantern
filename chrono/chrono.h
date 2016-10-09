#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void Lightning_MicrosecondsSleep(unsigned microseconds);
unsigned Lightning_GetMicrosecondsTime();
unsigned Lightning_GetMillisecondsTime();

#ifdef __cplusplus
}
#endif
