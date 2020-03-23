#pragma once
#include "connect4.h"
#include <iostream>
#include <vector>

typedef connect4::Board state;

struct NodeData
{
    int visited = 0;
    int action;
    state current_state;
    int reward = 0;
    bool terminal = false;
    bool fully_expanded = false;
    bool players_turn = false;
};

struct TreeNode : std::enable_shared_from_this<TreeNode>
{
    std::shared_ptr<TreeNode> parent;
    bool hasParent = false;
    std::vector<std::shared_ptr<TreeNode>> children;
    NodeData data;
    int depth = 0;

    TreeNode(const NodeData& node_data)
    {
        data = node_data;
    }

    std::shared_ptr<TreeNode> addChild(NodeData data)
    {
        auto n = std::make_shared<TreeNode>(data);
        n->parent = std::shared_ptr<TreeNode>(shared_from_this());
        n->hasParent = true;
        n->depth = n->parent->depth++;
        children.emplace_back(n);
        return n;
    }

    std::shared_ptr<TreeNode> getChild(const int action)
    {
        for (auto& child : children)
            if (child->data.action == action)
                return child;
    }
};