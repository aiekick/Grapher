#pragma once

#include <imguipack.h>

#include <ezlibs/ezXmlConfig.hpp>
#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezCnt.hpp>

#include <grapher/baseDefs.h>
#include <grapher/interfaces/SlotColorBankInterface.h>

#include <unordered_map>
#include <map>

class GRAPHER_API BaseSlot  //
    : public ez::Slot,
      public ez::xml::Config,
      public IDrawDebugInfos {
    friend class BaseGraph;
    friend class BaseNode;
    friend class BaseLink;

public:
    struct BaseSlotDatas : ez::SlotDatas {
        float radius{2.5f};
        ImU32 color{IM_COL32(200, 200, 0, 255)};
        ImU32 hovered_color{IM_COL32(0, 200, 0, 255)};
        ImU32 inner_color{IM_COL32(0, 200, 0, 255)};
        ImVec2 slotPadding{0, 0};
        float slotIconSize{16.0f};
        bool highLighted{false};
        bool showWidget{false};
        bool hideName{false};
        bool connected{false};
        bool hidden{false};
        bool debugMode{false};
        std::string hoveredInfos;  // infox dipslayed when the slot is hovered
        BaseSlotDatas() = default;
        BaseSlotDatas(
            const std::string& vName,
            const std::string& vType,
            const ez::SlotDir vSlotDir,
            const SlotColorBankInterface* vSlotColorBankPtr = nullptr,
            ez::UserDatas vUserDatas = nullptr,
            const size_t& vMaxConnectionCount = 1U)  // cant be changed after definition from the
            : ez::SlotDatas(vName, vType, vSlotDir, vUserDatas) {
            if (vSlotColorBankPtr != nullptr) {
                if (vSlotColorBankPtr->getSlotColor(vType, color)) {
                    hovered_color = color;
                    inner_color = color;
                }
            }
        }
    };

public:  // Static
    template <typename T>
    static std::shared_ptr<T> create(const BaseStyle& vParentStyle, const BaseSlotDatas& vSlotDatas) {
        static_assert(std::is_base_of<BaseSlot, T>::value, "T must derive of BaseSlot");
        auto slot_ptr = std::make_shared<T>(vParentStyle, vSlotDatas);
        slot_ptr->m_setThis(slot_ptr);
        slot_ptr->setUuid(slot_ptr->getUuid());  // call the virtual setUuid for derived classes
        if (!slot_ptr->init()) {
            slot_ptr.reset();
        }
        return slot_ptr;
    }
    template <typename T>
    static std::shared_ptr<T> create(const BaseStyle& vParentStyle) {
        static_assert(std::is_base_of<BaseSlot, T>::value, "T must derive of BaseSlot");
        auto slot_ptr = std::make_shared<T>(vParentStyle);
        slot_ptr->m_setThis(slot_ptr);
        slot_ptr->setUuid(slot_ptr->getUuid());  // call the virtual setUuid for derived classes
        if (!slot_ptr->init()) {
            slot_ptr.reset();
        }
        return slot_ptr;
    }

private:  // Common
    const BaseStyle& m_parentStyle;

private:
    ImVec2 m_pos{};
    ImVec2 m_size{};
    nd::PinId m_pinID{};

    // un slot peut etre attach� a plusieurs autre slots,
    // donc plusieurs links
    BaseLinkWeakCnt m_links;

    ImGuiMouseButton m_doubleClickedButton = -1;

public:
    template <typename T>
    explicit BaseSlot(const BaseStyle& vParentStyle, const T& vDatas) : m_parentStyle(vParentStyle), ez::Slot(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<BaseSlotDatas, T>::value, "T must derive of BaseSlotDatas");
    }
    ~BaseSlot() override { unit(); }

    bool init() override;

    void setRadius(const float vRadius);
    void setColor(const ImVec4& vColor);
    void setPos(const ImVec2& vPos);
    virtual bool draw();

    bool isConnected() const;
    const BaseLinkWeakCnt& getLinks() const;

    bool isAnInput();
    bool isAnOutput();

    size_t getMaxConnectionCount() const;

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    void drawDebugInfos() override;

    void setUuid(const ez::Uuid vUUID) override;

    nd::PinId getPinID() { return m_pinID; }

    bool isMouseDoubleClicked(ImGuiMouseButton& vButton);

private:
    void m_drawInputWidget();
    void m_drawOutputWidget();

protected:
    ImVec2 m_getPos() { return m_pos; }
    ImVec2 m_getSize() { return m_size; }
    void m_drawSlot();
    virtual void m_drawBaseSlot(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor);
    virtual void m_drawHoveredSlotText(const ImVec2& vCenter, bool vConnected, ImU32 vColor, ImU32 vInnerColor);
    virtual size_t m_getMaxConnectionCount() const;
    ImGuiMouseButton m_getDoubleClickedMouseButton() const;
};