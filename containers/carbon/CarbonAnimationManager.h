#ifndef EAGLEYE_CARBONANIMATIONMANAGER_H
#define EAGLEYE_CARBONANIMATIONMANAGER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include <array>

namespace EAGLEye
{
    namespace Containers
    {
        struct PACK AnimationBlockHeader
        {
            BYTE pad[8];
            uint numTrees;
            BYTE rest[12];
        };

        struct PACK AnimationBlockTree
        {
            uint numNodes;
        };

        struct PACK AnimationBlockNode
        {
            BYTE data[120];
        };

        struct CarbonAnimationNode
        {

        };

        class CarbonAnimationTree
        {
        public:
            std::vector<CarbonAnimationNode> nodes;

            auto& getCurrentNode()
            {
                assert(!nodes.empty());

                return nodes[nodes.size() - 1];
            }
        };

        class CarbonAnimationBlock
        {
        public:
            std::vector<CarbonAnimationTree> trees;
            uint numTrees;

            auto& getCurrentTree()
            {
                assert(!trees.empty());

                return trees[trees.size() - 1];
            }
        };

        /**
         * Kinda crossing the line here... but...
         */
        class CarbonAnimationManager
        {
        public:
            CarbonAnimationManager() = default;

            ~CarbonAnimationManager() = default;

            void PushBlock(AnimationBlockHeader &header);

            void PushTree(AnimationBlockTree &tree);

            void PushNode(AnimationBlockNode& node);
        private:
            std::vector<CarbonAnimationBlock> m_blocks;

            auto &getCurrentBlock()
            {
                assert(!m_blocks.empty());

                return m_blocks[m_blocks.size() - 1];
            }
        };
    }
}

#endif //EAGLEYE_CARBONANIMATIONMANAGER_H
