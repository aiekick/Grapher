﻿#pragma once

#include <imguipack.h>

#include <grapher/baseDefs.h>

#include <grapher/interfaces/GuiInterface.h>
#include <grapher/interfaces/NodeInterface.h>

#include <ezlibs/ezGraph.hpp>
#include <ezlibs/ezXmlConfig.hpp>

#include <vector>
#include <functional>
#include <unordered_map>

class GRAPHER_API BaseGraph  //
    : public ez::Graph,
      public ez::xml::Config,
      public GuiInterface,
      public NodeInterface,
      public IDrawDebugInfos {
public:
    struct BaseGraphDatas : public ez::GraphDatas {
        bool showFlow = false;                      // will display the flow of of link types
        std::string flowType;                       // the type for show the flow with F key
        ImGuiKey showFlowKey = ImGuiKey_Backspace;  // the key who start the flow display
    };
    typedef void* UserDatas;
    typedef std::function<void(const BaseGraphWeak&, UserDatas)> BgRightClickActionFunctor;
    typedef std::function<void(const BaseGraphWeak&, const BaseNodeWeak&, UserDatas)> SelectNodeActionFunctor;
    typedef std::function<void(const BaseGraphWeak&, const BaseNodeWeak&, UserDatas)> SelectNodeAsTargetActionFunctor;
    typedef std::function<bool(const BaseGraphWeak&, const BaseSlotWeak&, UserDatas)> PrepareForCreateNodeFromSlotActionFunctor;
    typedef std::function<bool(const BaseGraphWeak&, const ez::xml::Node&, const ez::xml::Node&, UserDatas)> LoadNodeFromXmlFunctor;
    typedef std::function<void(const BaseGraphWeak&, const BaseNodeWeak&, const BaseSlotWeak&, const ImGuiMouseButton&, UserDatas)> SelectSlotActionFunctor;
    typedef std::function<bool(const BaseGraphWeak&, const BaseNodeWeak&, UserDatas)> IsNodeSelectedAsTargetActionFunctor;
    typedef std::function<void(const BaseGraphWeak&, const BaseNodeWeak&, const BaseSlotWeak&, UserDatas)> SelectSlotAsTargetActionFunctor; 
    typedef ez::Uuid LinkUuid;

public:  // Static
    static BaseGraphPtr create(const BaseStyle& vParentStyle, const BaseGraphDatas& vNodeDatas) {
        auto node_ptr = std::make_shared<BaseGraph>(vParentStyle, vNodeDatas);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        }
        return node_ptr;
    }

private:  // Common
    const BaseStyle& m_parentStyle;

private:  // Graph
    nd::EditorContext* m_pCanvas{nullptr};
    ImVec2 m_openPopupPosition;
    nd::NodeId m_contextMenuNodeId{};
    nd::PinId m_contextMenuSlotId{};
    nd::LinkId m_contextMenuLinkId{};
    BaseNodeWeak m_selectedNode;
    BaseLinkPtrCnt m_links;  // linkId, link // for search query
    BgRightClickActionFunctor m_BgRightClickActionFunctor{nullptr};
    SelectNodeActionFunctor m_SelectNodeActionFunctor{nullptr};
    SelectNodeAsTargetActionFunctor m_SelectNodeAsTargetActionFunctor{nullptr};
    PrepareForCreateNodeFromSlotActionFunctor m_PrepareForCreateNodeFromSlotActionFunctor{nullptr};
    LoadNodeFromXmlFunctor m_LoadNodeFromXmlFunctor{nullptr};
    SelectSlotActionFunctor m_SelectSlotActionFunctor{nullptr};
    IsNodeSelectedAsTargetActionFunctor m_IsNodeSelectedAsTargetActionFunctor{nullptr};
    SelectSlotAsTargetActionFunctor m_SelectSlotAsTargetActionFunctor{nullptr};
    std::vector<nd::NodeId> m_nodesToCopy;  // for copy/paste
    ImVec2 m_nodesCopyOffset;
    bool m_graphChanged{false};
    bool m_xmlLoading = false;
    class SlotNotifier {
    public:
        enum class ActionType { CONNECT = 0, DISCONNECT, Count };

    private:
        struct Action {
            BaseSlotWeak start;
            BaseSlotWeak end;
            ActionType type;
        };
        std::vector<Action> m_waitingActions;

    public:
        void addAction(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd, ActionType vAction);
        void consume();
        void clear();
    } m_slotNotifier;

public:  // Normal
    template <typename T>
    explicit BaseGraph(const BaseStyle& vParentStyle, const T& vDatas)  //
        : m_parentStyle(vParentStyle), ez::Graph(std::make_shared<T>(vDatas)) {
        static_assert(std::is_base_of<BaseGraphDatas, T>::value, "T must derive of BaseGraphDatas");
    }
    ~BaseGraph() override;

    bool init() override;
    void unit() override;

    void clear() override;

    void setCurrentEditor() const;

    bool drawGraph();

    bool drawNodeWidget(const uint32_t& vFrame) override;
    bool drawWidgets() override;

    void zoomToContent() const;
    void navigateToContent() const;
    void zoomToSelection() const;
    void navigateToSelection() const;

    void setGraphChanged(bool vFlag);
    bool isGraphChanged() const;

    ImVec2 getMousePos() const;

    bool connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool disconnectSlot(const BaseSlotWeak& vSlot);
    bool disconnectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool disconnectLink(const BaseLinkWeak& vLink);

    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;

    void setBgRightClickActionFunctor(const BgRightClickActionFunctor& vFunctor);
    void bgRightClickAction(const BaseGraphWeak& vGraph, UserDatas vUserDatas);

    void setSelectNodeActionFunctor(const SelectNodeActionFunctor& vFunctor);
    void selectNodeAction(const BaseGraphWeak& vGraph, const BaseNodeWeak& vNode, UserDatas vUserDatas);

    void setSelectNodeAsTargetActionFunctor(const SelectNodeAsTargetActionFunctor& vFunctor);
    void selectNodeAsTargetAction(const BaseGraphWeak& vGraph, const BaseNodeWeak& vNode, UserDatas vUserDatas);

    void setIsNodeSelectedAsTargetActionFunctor(const IsNodeSelectedAsTargetActionFunctor& vFunctor);
    bool isNodeSelectedAsTargetAction(const BaseGraphWeak& vGraph, const BaseNodeWeak& vNode, UserDatas vUserDatas);

    void setPrepareForCreateNodeFromSlotActionFunctor(const PrepareForCreateNodeFromSlotActionFunctor& vFunctor);
    bool prepareForCreateNodeFromSlotAction(const BaseGraphWeak& vGraph, const BaseSlotWeak&, UserDatas vUserDatas);

    void setLoadNodeFromXmlFunctor(const LoadNodeFromXmlFunctor& vFunctor);
    bool loadNodeFromXml(const BaseGraphWeak& vGraph, const ez::xml::Node& vNode, const ez::xml::Node& vParent, UserDatas vUserDatas);

    void setSelectSlotActionFunctor(const SelectSlotActionFunctor& vFunctor);
    void selectSlotAction(
        const BaseGraphWeak& vGraph,
        const BaseNodeWeak& vNode,
        const BaseSlotWeak& vSlot,
        const ImGuiMouseButton& vBNutton,
        UserDatas vUserDatas);

    void setSelectSlotAsTargetActionFunctor(const SelectSlotAsTargetActionFunctor& vFunctor);
    void selectSlotAsTargetAction(const BaseGraphWeak& vGraph, const BaseNodeWeak& vNode, const BaseSlotWeak& vSlot, UserDatas vUserDatas);

    void drawDebugInfos() override;

    void beforeXmlLoading() override;
    void afterXmlLoading() override;

public:  // Template
    template <typename T>
    std::weak_ptr<T> createChildNode() {
        static_assert(std::is_base_of<BaseNode, T>::value, "T must derive of BaseNode");
        auto node_ptr = std::make_shared<T>(m_parentStyle);
        node_ptr->m_setThis(node_ptr);
        if (!node_ptr->init()) {
            node_ptr.reset();
        } else {
            if (m_addNode(node_ptr) != ez::RetCodes::SUCCESS) {
                node_ptr.reset();
            } else {
                setCurrentEditor();
                nd::SetNodePosition(node_ptr->m_nodeID, m_openPopupPosition);
            }
        }
        return node_ptr;
    }

    template <typename T>
    std::weak_ptr<T> cloneChildNode(const std::weak_ptr<T>& vNodeToClone, const ImVec2& vNewPos) {
        static_assert(std::is_base_of<BaseNode, T>::value, "T must derive of BaseNode");
        auto node_ptr = vNodeToClone.lock();
        if (node_ptr != nullptr) {
            auto duplicated_node_ptr = node_ptr->clone();
            if (m_addNode(duplicated_node_ptr) != ez::RetCodes::SUCCESS) {
                duplicated_node_ptr.reset();
            } else {
                setCurrentEditor();
                nd::SetNodePosition(duplicated_node_ptr->m_nodeID, m_openPopupPosition);
            }
            return duplicated_node_ptr;
        }
        return {};
    }

private:  // Graph
    void m_drawPopups();
    void m_drawBgContextMenuPopup();
    void m_drawLinks();

    static void m_drawLabel(const char* vLabel, ImU32 vColor);

    void m_doCreateLinkOrNode();
    void m_doDeleteLinkOrNode();
    void m_doSelectedLinkOrNode();
    void m_doGetSelectedNodeSlot(const BaseNodeWeak& vSlot);

    void m_doShorcutsOnNode();

    // create node with a specific slot type in mind
    void m_doCreateNodeFromSlot(const BaseSlotWeak& vSlot);

    void m_copySelectedNodes();
    void m_pasteNodesAtMousePos();
    void m_duplicateSelectedNodes(const ImVec2& vOffset);
    void m_duplicateNode(uint32_t vNodeId, const ImVec2& vOffsetPos);

    // finders
    BaseNodeWeak m_findNode(nd::NodeId vId);
    BaseNodeWeak m_findNodeByName(const std::string& vName);
    BaseLinkWeak m_findLink(nd::LinkId vId);
    BaseLinkWeak m_findLinkConnectedToSlot(nd::PinId vId);
    BaseSlotWeak m_findSlotById(nd::PinId vId);

    bool m_addVisualLink(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);
    bool m_breakVisualLink(const BaseSlotWeak& vStart, const BaseSlotWeak& vEnd);
    bool m_breakVisualLink(const BaseLinkWeak& vLink);
    bool m_breakVisualLinkConnectedToSlot(const BaseSlotWeak& vSlot);

    bool m_connectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool m_disconnectSlots(const BaseSlotWeak& vFrom, const BaseSlotWeak& vTo);
    bool m_disconnectSlot(const BaseSlotWeak& vSlot);
    bool m_disconnectLink(const BaseLinkWeak& vLink);

    bool m_delNode(const BaseNodeWeak& vNode);
};
