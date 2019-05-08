//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#ifndef OBJ2RAMSES_RENDEREREVENTHANDLER_H_
#define OBJ2RAMSES_RENDEREREVENTHANDLER_H_

#include "ramses-renderer-api/RamsesRenderer.h"
#include "ramses-renderer-api/DisplayConfig.h"
#include "ramses-renderer-api/IRendererEventHandler.h"

#include <chrono>
#include <thread>
#include <unordered_set>


namespace obj2ramses
{
    class SceneStateEventHandler : public ramses::RendererEventHandlerEmpty
    {
    public:
        SceneStateEventHandler(ramses::RamsesRenderer& renderer)
            : m_renderer(renderer)
        {
        }

        virtual void scenePublished(ramses::sceneId_t sceneId) override
        {
            m_publishedScenes.insert(sceneId);
        }

        virtual void sceneUnpublished(ramses::sceneId_t sceneId) override
        {
            m_publishedScenes.erase(sceneId);
        }

        virtual void sceneSubscribed(ramses::sceneId_t sceneId, ramses::ERendererEventResult result) override
        {
            if (ramses::ERendererEventResult_OK == result)
            {
                m_subscribedScenes.insert(sceneId);
            }
        }

        virtual void sceneUnsubscribed(ramses::sceneId_t sceneId, ramses::ERendererEventResult result) override
        {
            if (ramses::ERendererEventResult_FAIL != result)
            {
                m_subscribedScenes.erase(sceneId);
            }
        }

        virtual void sceneMapped(ramses::sceneId_t sceneId, ramses::ERendererEventResult result) override
        {
            if (ramses::ERendererEventResult_OK == result)
            {
                m_mappedScenes.insert(sceneId);
            }
        }

        virtual void sceneUnmapped(ramses::sceneId_t sceneId, ramses::ERendererEventResult result) override
        {
            if (ramses::ERendererEventResult_FAIL != result)
            {
                m_mappedScenes.erase(sceneId);
            }
        }

        virtual void displayCreated(ramses::displayId_t displayId, ramses::ERendererEventResult result)
        {
            if (result == ramses::ERendererEventResult_OK)
            {
                m_lastDisplayCreated = displayId;
            }
        }

        virtual void windowClosed(ramses::displayId_t)
        {
            m_windowWasClosed = true;
        }

        void waitForDisplayCreation(const ramses::displayId_t display)
        {
            while (m_lastDisplayCreated != display)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                m_renderer.dispatchEvents(*this);
            }
        }

        void waitForPublication(const ramses::sceneId_t sceneId)
        {
            waitForSceneInSet(sceneId, m_publishedScenes);
        }

        void waitForSubscription(const ramses::sceneId_t sceneId)
        {
            waitForSceneInSet(sceneId, m_subscribedScenes);
        }

        void waitForMapped(const ramses::sceneId_t sceneId)
        {
            waitForSceneInSet(sceneId, m_mappedScenes);
        }

        bool windowWasClosed() const
        {
            return m_windowWasClosed;
        }

    private:
        typedef std::unordered_set<ramses::sceneId_t> SceneSet;

        void waitForSceneInSet(const ramses::sceneId_t sceneId, const SceneSet& sceneSet)
        {
            while (sceneSet.find(sceneId) == sceneSet.end())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                m_renderer.dispatchEvents(*this);
            }
        }

        SceneSet m_publishedScenes;
        SceneSet m_subscribedScenes;
        SceneSet m_mappedScenes;

        ramses::RamsesRenderer& m_renderer;
        ramses::displayId_t m_lastDisplayCreated = ramses::InvalidDisplayId;
        bool m_windowWasClosed = false;
    };
}

#endif
