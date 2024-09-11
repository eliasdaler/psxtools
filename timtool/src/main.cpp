#include <cassert>

#include "TimFile.h"
#include "TimFileCreator.h"
#include "TimFileRead.h"
#include "TimFileWrite.h"

int main(int argc, char* argv[])
{
    // const auto tim = readTimFile(argv[1]);
    const auto config = TimConfig{
        .inputImage = argv[1],
        .clutDX = 0,
        .clutDY = 483,
        .pixDX = 512,
        .pixDY = 0,
        .direct15Bit = true,
    };
    const auto tim = createTimFile(config);
    writeTimFile(tim, argv[2]);

#if 0
    auto timFile2 = readTIM(argv[2]);
    assert(timFile.pmode == timFile2.pmode);
    assert(timFile.hasCLUT == timFile.hasCLUT);
#endif
}
