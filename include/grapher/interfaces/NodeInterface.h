#pragma once

#include <grapher/baseDefs.h>

#include <grapher/baseDefs.h>

class GRAPHER_API NodeInterface {
protected:
    BaseNodeWeak m_parentNode;  // node parent dans le cas d'un node enfant d'un graph

public:
    void setParentNode(BaseNodeWeak vBaseNodeWeak = BaseNodeWeak()) { m_parentNode = vBaseNodeWeak; }
    BaseNodeWeak getParentNode() { return m_parentNode; }
    virtual bool drawNodeWidget(const uint32_t& vFrame) = 0;
    virtual void beforeXmlLoading() {}
    virtual void afterXmlLoading() {}
};
