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
    float reward = 0.0f;
    bool terminal = false;
    bool fully_expanded = false;
    bool players_turn = false;
};

struct TreeNode
{
    TreeNode* parent;
    bool hasParent = false;
    std::vector<TreeNode*> children;
    NodeData data;
    int depth = 0;
};

class Tree
{
    TreeNode* root;

public:
    Tree(NodeData root_value)
    {
        root = new TreeNode();
        root->data = root_value;
    }

    Tree()
    {
        root = new TreeNode();
    }

    TreeNode* CreateNode(NodeData data)
    {
        TreeNode* node = new TreeNode;
        node->data = data;
        return node;
    }

    void InsertNode(TreeNode* parent, TreeNode* childNode)
    {
        parent->children.push_back(childNode);
        childNode->parent = parent;
        childNode->hasParent = true;
        childNode->depth = parent->depth + 1;
    }

    void setRoot(TreeNode* node)
    {
        root = node;
        root->depth = 0;
    }

    TreeNode& getRoot()
    {
        return *root;
    }
};
#endif // !TREE_H