#pragma once

#include <memory>

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#if defined(grapher_EXPORTS)
#define GRAPHER_API __declspec(dllexport)
#elif defined(BUILD_GRAPHER_SHARED_LIBS)
#define GRAPHER_API __declspec(dllimport)
#else
#define GRAPHER_API
#endif
#else
#define GRAPHER_API
#endif

#include <imguipack.h>

#include <ezlibs/ezCnt.hpp>
#include <ezlibs/ezGraph.hpp>

struct GRAPHER_API BaseStyle {
    bool debugMode = false;
    nd::Style style;
};

class BaseGraph;
typedef std::shared_ptr<BaseGraph> BaseGraphPtr;
typedef std::weak_ptr<BaseGraph> BaseGraphWeak;

class BaseLink;
typedef std::shared_ptr<BaseLink> BaseLinkPtr;
typedef std::weak_ptr<BaseLink> BaseLinkWeak;
typedef ez::cnt::DicoVector<ez::Uuid, BaseLinkWeak> BaseLinkWeakCnt;
typedef ez::cnt::DicoVector<ez::Uuid, BaseLinkPtr> BaseLinkPtrCnt;

class BaseNode;
typedef std::shared_ptr<BaseNode> BaseNodePtr;
typedef std::weak_ptr<BaseNode> BaseNodeWeak;

class BaseSlot;
typedef std::shared_ptr<BaseSlot> BaseSlotPtr;
typedef std::weak_ptr<BaseSlot> BaseSlotWeak;

class GRAPHER_API IDrawDebugInfos {
public:
    virtual void drawDebugInfos() = 0;
};

#ifdef USE_IMGUI_NODE_EDITOR

#include <istream>
#include <ostream>

// for ez::xml::getAttribute<nd::Pinid>()

static inline std::istream& operator>>(std::istream& vIn, nd::PinId& vType) {
    uintptr_t v;
    if (vIn >> v) {
        vType = static_cast<nd::PinId>(v);
    }
    return vIn;
}

static inline std::ostream& operator<<(std::ostream& vOut, const nd::PinId& vType) {
    vOut << static_cast<uintptr_t>(vType);
    return vOut;
}

#include <ezlibs/ezXml.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#endif
