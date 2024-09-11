#include <cassert>

#include "TimFile.h"
#include "TimFileRead.h"
#include "TimFileWrite.h"

#include "Color.h"

int main(int argc, char* argv[])
{
    auto c = to16BitColor(Color32{.r = 74, .g = 57, .b = 41, .a = 0});
    assert(c == 0x14e9);

    const auto timFile = readTimFile(argv[1]);
    writeTimFile(timFile, argv[2]);

#if 0
    auto timFile2 = readTIM(argv[2]);
    assert(timFile.pmode == timFile2.pmode);
    assert(timFile.hasCLUT == timFile.hasCLUT);
#endif
}
