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
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

#include "ramses-client.h"
#include "ObjGeometry.h"

namespace obj2ramses
{
    ObjImporter::ObjImporter(ramses::RamsesClient& client, ramses::Scene& scene)
        : m_client(client)
        , m_scene(scene)
    {
    }

    /**
     * @brief Imports geometry from a .obj file, loading it into the importer.
     *
     * @param objFile
     * @return false if the file cannot be opened e.g. if the stream's bad bit is set.
     */
    bool ObjImporter::importFromFile(const std::string& objFile)
    {
        // Import OBJ data and put into ramses scene
        std::ifstream f{objFile};
        string line;

        if (f.bad())
            return false;

        while (std::getline(f, line)) {
            std::istringstream iss{line};
            vector<string> tokens = this->tokenize(line,' ');

            string dtype = tokens[0];

            if (dtype == "v") {

                vertex3f v;
                v.x = std::stof(tokens[1]);
                v.y = std::stof(tokens[2]);
                v.z = std::stof(tokens[3]);
                this->vertices.push_back(v);

            } else if (dtype == "vt") {
                tex_coord_3f vt;
                vt.u = std::stof(tokens[1]);

                if (tokens.size() >= 3 ){
                    vt.v = std::stof(tokens[2]);
                }

                if(tokens.size() >= 4){
                    vt.w = std::stof(tokens[3]);
                }

                this->tex_coords.push_back(vt);


            } else if (dtype == "vn") {
                vertex_normal_3f vn;
                vn.x = std::stof(tokens[1]);
                vn.y = std::stof(tokens[2]);
                vn.z = std::stof(tokens[3]);
                this->normals.push_back(vn);

            } else if (dtype == "f") {
                bool has_double_slash = line.find("//") != std::string::npos;
                bool has_slash = line.find("/") != std::string::npos;
                face f;
                vector<unsigned> v, vt, vn;

                if(has_double_slash){
                    auto pos = line.find("//");
                    line.erase(pos); /* get rid of it and keep processing */
                }

                if (has_slash) {
                    vector<string> face_tokens = this->tokenize(line, ' ');
                    face f;
                    f.len = face_tokens.size() - 1; /* do not count the 'f' char */

                    for(int i = 1; i < face_tokens.size(); ++i){
                        vector<string> face_items = this->tokenize(face_tokens[i], '/');

                        f.v.push_back(std::stoul(face_items[0]));

                        if (face_items.size() > 1)
                            f.vt.push_back(has_double_slash ? -1 : std::stoul(face_items[1]));

                        if(face_items.size() > 2) {
                            auto vn_index = (has_double_slash) ? 1 : 2;
                            f.vn.push_back(std::stoul(face_items[vn_index]));
                        }

                    }

                    this->faces.push_back(f);


                }

            } else {
                std::cerr << "Skipping: This code can only handle v, vt, vn and f, but got this instead: " << dtype << std::endl;
            }
        }

        // TODO remove after obj can be imported
        createDummyScene();

        return true;
    }

    std::vector<string>
    ObjImporter::tokenize(string line, char delim)
    {
        std::istringstream iss{line};

        if (delim == ' '){
            vector<string> tokens(std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>());
            return tokens;
        }

        std::string item;
        std::vector<std::string> tokens;
        while (std::getline(iss, item, delim))
        {
            tokens.push_back(item);
        }

        return tokens;
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
