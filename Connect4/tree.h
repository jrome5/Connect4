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
    std::vector<std::shared_ptr<TreeNode>> children;
    NodeData data;
    int depth = 0;

    TreeNode(const NodeData& node_data)
    {
        data = node_data;
    }

    void remove()
    { 
        if (children.size())
        {
            for (auto& child : children)
            {
                child->remove();
                child.reset();
            }
        }
        parent.reset();
    }

    std::shared_ptr<TreeNode> addChild(const int action, bool add_parent=true)
    {
        NodeData data;
        data.action = action;
        auto child = std::make_shared<TreeNode>(data);
        if (add_parent)
        {
            child->parent = std::shared_ptr<TreeNode>(shared_from_this());
        }
        child->depth = this->depth+1;
        child->data.players_turn = !(this->data.players_turn);
        connect4::copyState(child->data.current_state, this->data.current_state);
        connect4::dropCoin(child->data.current_state, child->data.action, connect4::getCoin(child->data.players_turn));
        child->data.terminal = isNodeTerminal(child);
        children.emplace_back(child);
        return child;
    }

    bool isNodeTerminal(const std::shared_ptr<TreeNode>& node)
    {
        const char coin = connect4::getCoin(node->data.players_turn);
        return connect4::checkWinner(node->data.current_state, coin);
    }

    std::shared_ptr<TreeNode> getChild(const int action)
    {
        for (auto& child : children)
            if (child->data.action == action)
                return child;
    }
};