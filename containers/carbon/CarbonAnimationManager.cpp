#include "CarbonAnimationManager.h"

namespace EAGLEye
{
    namespace Containers
    {
        void CarbonAnimationManager::PushBlock(AnimationBlockHeader &header)
        {
            CarbonAnimationBlock block{};
            block.trees.resize(header.numTrees);
            block.numTrees = header.numTrees;

            m_blocks.emplace_back(block);
        }

        void CarbonAnimationManager::PushTree(AnimationBlockTree &tree)
        {
            CarbonAnimationTree animationTree{};
            animationTree.nodes.resize(tree.numNodes);

            getCurrentBlock().trees.emplace_back(animationTree);
        }

        void CarbonAnimationManager::PushNode(AnimationBlockNode &node)
        {
            CarbonAnimationNode animationNode{};
            getCurrentBlock().getCurrentTree().nodes.emplace_back(animationNode);
        }
    }
}