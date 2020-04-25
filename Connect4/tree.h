#pragma once
#include "connect4.h"
#include <iostream>
#include <vector>
#include <mutex>

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
        child->data.current_state = this->data.current_state;
        child->data.current_state.dropCoin(child->data.action, connect4::getCoin(child->data.players_turn));
        child->data.terminal = isNodeTerminal();
        children.emplace_back(child);
        return child;
    }

    bool isNodeTerminal()
    {
        const char coin = connect4::getCoin(data.players_turn);
        return data.current_state.checkWinner(coin);
    }

    std::shared_ptr<TreeNode> getChild(const int action)
    {
        for (auto& child : children)
            if (child->data.action == action)
                return child;
    }
};

struct MutexNode : std::enable_shared_from_this<MutexNode>
{
    std::mutex mutex;
    std::shared_ptr<MutexNode> parent;
    std::vector<std::shared_ptr<MutexNode>> children;
    NodeData data;
    int depth = 0;

    MutexNode(const NodeData& node_data)
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

    std::shared_ptr<MutexNode> addChild(const int action, bool add_parent = true)
    {
        NodeData data;
        data.action = action;
        auto child = std::make_shared<MutexNode>(data);
        if (add_parent)
        {
            child->parent = std::shared_ptr<MutexNode>(shared_from_this());
        }
        child->depth = this->depth + 1;
        child->data.players_turn = !(this->data.players_turn);
        child->data.current_state = this->data.current_state;
        child->data.current_state.dropCoin(child->data.action, connect4::getCoin(child->data.players_turn));
        child->data.terminal = isNodeTerminal();
        children.emplace_back(child);
        return child;
    }

    bool isNodeTerminal()
    {
        const char coin = connect4::getCoin(data.players_turn);
        return data.current_state.checkWinner(coin);
    }

    std::shared_ptr<MutexNode> getChild(const int action)
    {
        for (auto& child : children)
            if (child->data.action == action)
                return child;
    }
};