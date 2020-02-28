#pragma once
#include<vector>
#include "connect4.h"
#ifndef TREE_H
#define TREE_H

typedef connect4::Board state;

struct NodeData
{
    int visited = 0;
    int action = 0;
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

    TreeNode(NodeData node_data)
    {
        data = node_data;
    }

    ~TreeNode() {
        //std::cout << "Treetus deletus";
    };

    std::shared_ptr<TreeNode> addChild(NodeData data)
    {
        auto n = std::make_shared<TreeNode>(data);
        n->parent = std::shared_ptr<TreeNode>(shared_from_this());
        n->hasParent = true;
        n->depth = depth + 1;
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

class Tree
{
    std::shared_ptr<TreeNode> root;

public:
    Tree(NodeData root_value)
    {
        root->data = root_value;
    }

    Tree()
    {
    }

    void InsertNode(std::shared_ptr<TreeNode>& parent, std::shared_ptr<TreeNode>& childNode)
    {
        childNode->parent = parent;
        childNode->hasParent = true;
        childNode->depth = parent->depth + 1;
        parent->children.push_back(childNode);
    }

    void setRoot(std::shared_ptr<TreeNode>& node)
    {
        root = node;
        root->depth = 0;
    }

    std::shared_ptr<TreeNode>& getRoot()
    {
        return root;
    }
};
#endif // !TREE_H