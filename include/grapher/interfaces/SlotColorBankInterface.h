#pragma once

#include <string>
#include <cstdint>

#include <grapher/baseDefs.h>

struct ImVec4;
typedef uint32_t ImU32;

class GRAPHER_API SlotColorBankInterface {
public:
    virtual bool getSlotColor(const std::string& vBaseSlotType, ImVec4& vOutColor) const = 0;
    virtual bool getSlotColor(const std::string& vBaseSlotType, ImU32& vOutColor) const = 0;
    virtual void addSlotColor(const std::string& vBaseSlotType, const ImVec4& vSlotColor) = 0;
};
