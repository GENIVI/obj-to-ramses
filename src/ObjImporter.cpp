//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#include "ObjImporter.h"

#include <iostream>
#include <fstream>
#include "ramses-client.h"

namespace obj2ramses
{
    ObjImporter::ObjImporter(ramses::RamsesClient& client, ramses::Scene& scene)
        : m_client(client)
        , m_scene(scene)
    {
    }

    bool ObjImporter::importFromFile(const std::string& objFile)
    {
        // Import OBJ data and put into ramses scene
        std::ifstream objFileStream(objFile);

        std::string line;
        while (objFileStream) {
            std::getline(objFileStream, line);
            std::cout << line << "\n";
        }
        objFileStream.close();

        // TODO remove after obj can be imported
        createDummyScene();

        return true;
    }

    void ObjImporter::createDummyScene()
    {
        // every scene needs a render pass with camera
        ramses::Camera* camera = m_scene.createRemoteCamera("my camera");
        ramses::RenderPass* renderPass = m_scene.createRenderPass("my render pass");
        renderPass->setClearFlags(ramses::EClearFlags_None);
        renderPass->setCamera(*camera);
        ramses::RenderGroup* renderGroup = m_scene.createRenderGroup();
        renderPass->addRenderGroup(*renderGroup);

        // prepare triangle geometry: vertex position array and index array
        float vertexPositionsData[] = {
            -1.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            1.f, 1.f, 0.f};
        const ramses::Vector3fArray* vertexPositions = m_client.createConstVector3fArray(3, vertexPositionsData);
        uint16_t indexData[] = { 0, 1, 2};
        const ramses::UInt16Array* indices = m_client.createConstUInt16Array(3, indexData);

        std::string vertexShader = R"shader(
#version 300 es

in vec3 a_position;

void main()
{
    // z = -1.0, so that the geometry will not be clipped by the near plane of the camera
    gl_Position = vec4(a_position.xy, -1.0, 1.0);
}
)shader";

        std::string fragmentShader = R"shader(
#version 300 es

precision mediump float;
uniform vec4 color;
out vec4 FragColor;

void main(void)
{
    FragColor = color;
}

)shader";

        // create an appearance
        ramses::EffectDescription effectDesc;
        effectDesc.setVertexShader(vertexShader.c_str());
        effectDesc.setFragmentShader(fragmentShader.c_str());

        const ramses::Effect* effect = m_client.createEffect(effectDesc);
        ramses::Appearance* appearance = m_scene.createAppearance(*effect);

        // set vertex positions directly in geometry
        ramses::GeometryBinding* geometry = m_scene.createGeometryBinding(*effect);
        geometry->setIndices(*indices);
        ramses::AttributeInput positionsInput;
        effect->findAttributeInput("a_position", positionsInput);
        geometry->setInputBuffer(positionsInput, *vertexPositions);

        // create a mesh node to define the triangle with chosen appearance
        ramses::MeshNode* meshNode = m_scene.createMeshNode();
        meshNode->setAppearance(*appearance);
        meshNode->setGeometryBinding(*geometry);
        // mesh needs to be added to a render group that belongs to a render pass with camera in order to be rendered
        renderGroup->addMeshNode(*meshNode);

        ramses::UniformInput colorInput;
        effect->findUniformInput("color", colorInput);
        appearance->setInputValueVector4f(colorInput, 0.9f, 0.0f, 0.0f, 1.0);

    }

}
