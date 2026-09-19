#include <string.h>
#include <zlib.h>

int Unzip(unsigned char* src, unsigned char* dst) {
    z_stream zs;
    int ret;

    memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, 16 + 15) != Z_OK) {
        return -1;
    }
    zs.next_in = src;
    zs.avail_in = 0xFFFFFFFF;
    zs.next_out = dst;
    zs.avail_out = 0x7FFFFFFF;
    ret = inflate(&zs, Z_FINISH);
    inflateEnd(&zs);
    if (ret != Z_STREAM_END) {
        return -1;
    }
    return (int)zs.total_out;
}
