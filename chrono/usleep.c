#include "chrono.h"

#include <unistd.h>
#include <stdlib.h>

void Lightning_MicrosecondsSleep(unsigned microseconds){
    usleep(microseconds);
}
