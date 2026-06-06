#include "pch.h"
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include "getAPKpackagename.h"

static uint16_t readLE16(const uint8_t* p) {
    return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}
static uint32_t readLE32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
        (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

static std::string getStringFromPool(const uint8_t* poolData, uint32_t poolSize,
    uint32_t stringIndex) {
    if (stringIndex == 0xFFFFFFFF) return "";

    uint32_t stringCount = readLE32(poolData + 8);
    if (stringIndex >= stringCount) return "";

    uint32_t stringsStart = readLE32(poolData + 16);
    uint32_t flag = readLE32(poolData + 28);

    const uint8_t* offsets = poolData + 28 + 4 + flag;
    uint32_t offset = readLE32(offsets + stringIndex * 4);

    const uint8_t* strData = poolData + stringsStart + offset;

    if ((flag >> 8) == 0) {
        uint16_t len16 = readLE16(strData);
        if (len16 & 0x8000) {
            len16 = ((len16 & 0x7FFF) << 16) | readLE16(strData + 2);
            strData += 4;
        }
        else {
            strData += 2;
        }
        std::string out;
        out.reserve(len16);
        for (uint16_t i = 0; i < len16; ++i) {
            uint16_t ch = readLE16(strData);
            strData += 2;
            if (ch < 0x80) {
                out.push_back(static_cast<char>(ch));
            }
            else if (ch < 0x800) {
                out.push_back(static_cast<char>(0xC0 | (ch >> 6)));
                out.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
            }
            else {
                out.push_back(static_cast<char>(0xE0 | (ch >> 12)));
                out.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
            }
        }
        return out;
    }
    else {
        uint8_t len8 = strData[0];
        strData += 1;
        if (len8 & 0x80) {
            len8 = ((len8 & 0x7F) << 8) | strData[0];
            strData += 1;
        }
        std::string out(reinterpret_cast<const char*>(strData), len8);
        return out;
    }
}

std::string getPackageName(const std::string& axmlPath) {
    std::ifstream file(axmlPath, std::ios::binary | std::ios::ate);
    if (!file) return "";

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        return "";

    if (size < 8) return "";

    uint16_t type = readLE16(buffer.data());
    if (type != 0x0003) return "";

    uint32_t chunkSize = readLE32(buffer.data() + 4);
    if (chunkSize > static_cast<uint32_t>(size)) return "";

    size_t pos = 8;
    const uint8_t* stringPoolData = nullptr;
    uint32_t stringPoolSize = 0;

    while (pos + 8 <= chunkSize) {
        uint16_t subType = readLE16(buffer.data() + pos);
        uint32_t subSize = readLE32(buffer.data() + pos + 4);
        if (subSize < 8 || pos + subSize > chunkSize) break;

        if (subType == 0x0001) {
            stringPoolData = buffer.data() + pos;
            stringPoolSize = subSize;
        }
        pos += subSize;
    }

    if (!stringPoolData) return "";

    pos = 8;
    while (pos + 8 <= chunkSize) {
        uint16_t subType = readLE16(buffer.data() + pos);
        uint32_t subSize = readLE32(buffer.data() + pos + 4);
        if (subSize < 8 || pos + subSize > chunkSize) break;

        if (subType == 0x0100) {
            const uint8_t* tagData = buffer.data() + pos;
            uint32_t nameIndex = readLE32(tagData + 20);
            std::string tagName = getStringFromPool(stringPoolData, stringPoolSize, nameIndex);
            if (tagName == "manifest") {
                uint16_t attrCount = readLE16(tagData + 16);
                const uint8_t* attrBase = tagData + 36;

                for (uint16_t i = 0; i < attrCount; ++i) {
                    const uint8_t* attr = attrBase + i * 20;
                    uint32_t nsIndex = readLE32(attr + 0);
                    uint32_t nameIdx = readLE32(attr + 4);
                    std::string ns = getStringFromPool(stringPoolData, stringPoolSize, nsIndex);
                    std::string attrName = getStringFromPool(stringPoolData, stringPoolSize, nameIdx);

                    if (attrName == "package" && ns.empty()) {
                        uint32_t valueStrIdx = readLE32(attr + 8);
                        return getStringFromPool(stringPoolData, stringPoolSize, valueStrIdx);
                    }
                }
                return "";
            }
        }
        pos += subSize;
    }
    return "";
}