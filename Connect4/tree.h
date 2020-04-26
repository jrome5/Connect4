#pragma once
#include "connect4.h"
#include <iostream>
#include <vector>
#include <unordered_map>

typedef connect4::Board state;

struct KeyHasher
{
    size_t operator()(const state& k) const noexcept
    {
       return std::hash<std::string>{}(k.getDataAsString());
    }
};

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
    std::vector<std::shared_ptr<TreeNode>> parents;
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
        children.clear();
        parents.clear();
    }

    std::shared_ptr<TreeNode> addChildTransposition(std::unordered_map<state, std::shared_ptr<TreeNode>, KeyHasher>& state_map, const int action)
    {
        state new_state = this->data.current_state;
        const bool players_turn = !this->data.players_turn;
        new_state.dropCoin(action, connect4::getCoin(players_turn));
        auto state_it = state_map.find(new_state);
        if (state_it != state_map.end())
        {
            auto node = state_it->second;
            node->parents.emplace_back(std::shared_ptr<TreeNode>(shared_from_this()));
            this->children.emplace_back(node);
            return node;
        }
        else
        {
            //create new node
            auto child = addChild(action);
            //add new node to map
            state_map.emplace(child->data.current_state, child);
            return child;
        }
    }

    std::shared_ptr<TreeNode> addChild(const int action, bool add_parent=true)
    {
        NodeData data;
        data.action = action;
        auto child = std::make_shared<TreeNode>(data);
        if (add_parent)
        {
            child->parents.emplace_back(std::shared_ptr<TreeNode>(shared_from_this()));
        }
        child->depth = this->depth+1;
        child->data.players_turn = !(this->data.players_turn);
        child->data.current_state = this->data.current_state;
        child->data.current_state.dropCoin(child->data.action, connect4::getCoin(child->data.players_turn));
        child->data.terminal = isNodeTerminal(child);
        children.emplace_back(child);
        return child;
    }

    bool isNodeTerminal(const std::shared_ptr<TreeNode>& node)
    {
        const char coin = connect4::getCoin(node->data.players_turn);
        return node->data.current_state.checkWinner(coin);
    }

    std::shared_ptr<TreeNode> getChild(const int action)
    {
        for (auto& child : children)
            if (child->data.action == action)
                return child;
    }
};