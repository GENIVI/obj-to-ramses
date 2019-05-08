//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#include "SceneToText.h"

#include <assert.h>

#include "ramses-client-api/Scene.h"
#include "ramses-client-api/SceneObjectIterator.h"
#include "ramses-utils.h"
#include "ramses-client-api/Node.h"
#include "ramses-client-api/RenderPass.h"
#include "ramses-client-api/PerspectiveCamera.h"

namespace obj2ramses
{

    SceneToText::SceneToText(bool printTransformations)
        : m_printTransformations(printTransformations)
    {
    }

    void SceneToText::printToStream(const ramses::Scene& scene, const ramses::RamsesClient& /*client*/, std::ostringstream& stream) const
    {
        stream << "Scene '" << scene.getName() << "' [id:" << scene.getSceneId() << "]\n";

        NodeSet rootNodes = collectRootNodes(scene);

        for (auto& rootNode : rootNodes)
        {
            printSubtree(stream, *rootNode, 0);
        }

        stream << "\nRenderPass setup:\n";
        printRenderPasses(stream, scene);
    }

    void SceneToText::printRenderPasses(std::ostringstream& stream, const ramses::Scene& scene) const
    {
        ramses::SceneObjectIterator renderPassIterator(scene, ramses::ERamsesObjectType_RenderPass);

        const ramses::RamsesObject* passAsObject = renderPassIterator.getNext();
        while (nullptr != passAsObject)
        {
            ramses::RenderPass const * renderPass = ramses::RamsesUtils::TryConvert<ramses::RenderPass>(*passAsObject);
            assert(nullptr != renderPass);

            stream << "RenderPass " << renderPass->getName() << "\n";

            if (nullptr != renderPass->getCamera() && renderPass->getCamera()->isOfType(ramses::ERamsesObjectType_LocalCamera))
            {
                ramses::LocalCamera const * localCamera = ramses::RamsesUtils::TryConvert<ramses::LocalCamera>(*renderPass->getCamera());

                std::string cameraType =
                    (ramses::ERamsesObjectType_PerspectiveCamera == localCamera->getType()) ?
                    "Perspective Camera:" :
                    "Orthographic Camera:";

                stream << "  " << cameraType << "\n    Planes: [" <<
                    "Left: " << localCamera->getLeftPlane() <<
                    " Right: " << localCamera->getRightPlane() <<
                    " Bot: " << localCamera->getBottomPlane() <<
                    " Top: " << localCamera->getTopPlane() << "]\n";
                stream << "    Viewport: ["
                    << localCamera->getViewportX() << ", "
                    << localCamera->getViewportY() << ", "
                    << localCamera->getViewportWidth() << ", "
                    << localCamera->getViewportHeight() << "]\n";

                ramses::PerspectiveCamera const * perspCamera = ramses::RamsesUtils::TryConvert<ramses::PerspectiveCamera>(*localCamera);
                if (nullptr != perspCamera)
                {
                    stream << "    FoV: " << perspCamera->getVerticalFieldOfView() << "; Asp. Ratio: " << perspCamera->getAspectRatio();
                }
            }
            stream << "\n";

            passAsObject = renderPassIterator.getNext();
        }
    }

    void SceneToText::printSubtree(std::ostringstream& stream, const ramses::Node& rootNode, uint32_t indentation) const
    {
        stream << std::string(indentation, ' ');

        const std::string rootNodePrefix = (0 == indentation) ? "Root" : "";

        if (ramses::ERamsesObjectType_Node == rootNode.getType())
        {
            stream << rootNodePrefix << "Node '" << rootNode.getName() << "'\n";
        }
        else if (ramses::ERamsesObjectType_MeshNode == rootNode.getType())
        {
            stream << rootNodePrefix << "MeshNode '" << rootNode.getName() << "'\n";
        }
        else if (rootNode.isOfType(ramses::ERamsesObjectType_Camera))
        {
            stream << rootNodePrefix << "Camera '" << rootNode.getName() << "'\n";
        }

        printTransformations(stream, rootNode, indentation);
        for (uint32_t i = 0; i < rootNode.getChildCount(); ++i)
            printSubtree(stream, *rootNode.getChild(i), indentation+1);
    }

    void SceneToText::printTransformations(std::ostringstream& stream, const ramses::Node& node, uint32_t indentation) const
    {
        if (m_printTransformations)
        {
            float sx, sy, sz;
            float tx, ty, tz;
            float rx, ry, rz;
            node.getScaling(sx, sy, sz);
            node.getTranslation(tx, ty, tz);
            node.getRotation(rx, ry, rz);
            if (sx != 1.0f || sy != 1.0f || sz != 1.0f || tx != 0.0f || ty != 0.0f || tz != 0.0f || rx != 0.0f || ry != 0.0f || rz != 0.0f)
            {
                stream << std::string(indentation, ' ');

                if (sx != 1.0f || sy != 1.0f || sz != 1.0f)
                    stream << "S[" << sx << "," << sy << "," << sz << "] ";
                if (tx != 0.0f || ty != 0.0f || tz != 0.0f)
                    stream << "T[" << tx << "," << ty << "," << tz << "] ";
                if (rx != 0.0f || ry != 0.0f || rz != 0.0f)
                    stream << "R[" << rx << "," << ry << "," << rz << "] ";

                stream << "\n";
            }
        }
    }

    NodeSet SceneToText::collectRootNodes(const ramses::Scene& scene) const
    {
        NodeSet rootNodes;
        ramses::SceneObjectIterator nodeIterator(scene, ramses::ERamsesObjectType_Node);

        const ramses::RamsesObject* nodeAsObject = nodeIterator.getNext();
        while (nullptr != nodeAsObject)
        {
            ramses::Node const * node = ramses::RamsesUtils::TryConvert<ramses::Node>(*nodeAsObject);
            assert(nullptr != node);
            if (nullptr == node->getParent())
                rootNodes.insert(node);

            nodeAsObject = nodeIterator.getNext();
        }

        return rootNodes;
    }
}
