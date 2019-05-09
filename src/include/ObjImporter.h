//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef OBJ2RAMSES_OBJIMPORTER
#define OBJ2RAMSES_OBJIMPORTER

#include <string>
#include <array>

#include "ObjGeometry.h"

using std::string;
using std::vector;

using obj2ramses::ObjGeometry::vertex3f;
using obj2ramses::ObjGeometry::tex_coord_3f;
using obj2ramses::ObjGeometry::vertex_normal_3f;
using obj2ramses::ObjGeometry::face;

namespace ramses
{
    class RamsesClient;
    class Scene;
}

namespace obj2ramses
{
    class ObjImporter
    {
    public:

        ObjImporter(ramses::RamsesClient& client, ramses::Scene& scene);

        bool importFromFile(const std::string& objFile);

        void asRamsesScene();

    private:
        void createDummyScene();

        ramses::RamsesClient& m_client;
        ramses::Scene& m_scene;

        vector<vertex3f> vertices;
        vector<tex_coord_3f> tex_coords;
        vector<vertex_normal_3f> normals;
        vector<face> faces;

        std::vector<string> tokenize(string line, char delim = ' ');

        int computeIndexCount();
        vector<uint16_t> getIndexArray();
        vector<float> getVertexArray();

    };
}

#endif
