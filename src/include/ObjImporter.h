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

    private:
        ramses::RamsesClient& m_client;
        ramses::Scene& m_scene;
    };
}

#endif
