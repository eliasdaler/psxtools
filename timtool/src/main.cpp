#include <cassert>

#include "TimFile.h"
#include "TimFileRead.h"
#include "TimFileWrite.h"

int main(int argc, char* argv[])
{
    const auto timFile = readTimFile(argv[1]);
    writeTimFile(timFile, argv[2]);

#if 0
    auto timFile2 = readTIM(argv[2]);
    assert(timFile.pmode == timFile2.pmode);
    assert(timFile.hasCLUT == timFile.hasCLUT);
#endif
}
