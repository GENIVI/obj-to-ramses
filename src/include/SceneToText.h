//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef OBJ2RAMSES_SCENETOTEXT
#define OBJ2RAMSES_SCENETOTEXT

#include <sstream>
#include <set>

namespace ramses
{
    class Scene;
    class RamsesClient;
    class Node;
}

namespace obj2ramses
{
    using NodeSet = std::set<ramses::Node const*>;

    class SceneToText
    {
    public:
        SceneToText(bool printTransformations);

        void printToStream(const ramses::Scene& scene, const ramses::RamsesClient& client, std::ostringstream& stream) const;

    private:

        NodeSet collectRootNodes(const ramses::Scene& scene) const;
        void printSubtree(std::ostringstream& stream, const ramses::Node& rootNode, uint32_t indentation) const;
        void printTransformations(std::ostringstream& stream, const ramses::Node& rootNode, uint32_t indentation) const;
        void printRenderPasses(std::ostringstream& stream, const ramses::Scene& scene) const;

        bool m_printTransformations;
    };
}

#endif
