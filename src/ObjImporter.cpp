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

        return true;
    }

}
