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

#include "ramses-client-api/Camera.h"

#include <chrono>
#include <thread>
#include <unordered_set>
#include <iostream>


namespace obj2ramses
{
    class SceneStateEventHandler : public ramses::RendererEventHandlerEmpty
    {
    public:
        SceneStateEventHandler(ramses::RamsesRenderer& renderer, ramses::Camera& camera)
            : m_renderer(renderer), m_camera(camera)
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

        virtual void displayCreated(ramses::displayId_t displayId, ramses::ERendererEventResult result) override
        {
            if (result == ramses::ERendererEventResult_OK)
            {
                m_lastDisplayCreated = displayId;
            }
        }

        virtual void windowClosed(ramses::displayId_t) override
        {
            m_windowWasClosed = true;
        }

        virtual void keyEvent(ramses::displayId_t displayId, ramses::EKeyEvent eventType, uint32_t keyModifiers, ramses::EKeyCode keyCode) override
        {

            switch(keyCode)
            {
                case ramses::EKeyCode_W:
                m_camera.translate(1.0f, 0.0f, 0.0f);  break;

                case ramses::EKeyCode_S:
                m_camera.translate(-1.0f, 0.0f, 0.0f); break;

                case ramses::EKeyCode_A:
                m_camera.translate(0.0f, -1.0f, 0.0f); break;

                case ramses::EKeyCode_D:
                m_camera.translate(0.0f, 1.0f, 0.0f);  break;
            }
        }

        virtual void mouseEvent(ramses::displayId_t displayId, ramses::EMouseEvent eventType, int32_t mousePosX, int32_t mousePosY) override
        {
            int32_t deltaX = mousePosX - m_MouseLastX;
            int32_t deltaY = mousePosY - m_MouseLastY;
            float sensitivity = 0.1f;

            float pitch = sensitivity * deltaX;
            float yaw = sensitivity * deltaY;

            m_camera.rotate(pitch, yaw, 0.0f);

            m_MouseLastX = mousePosX;
            m_MouseLastY = mousePosY;
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
        ramses::Camera& m_camera;
        ramses::displayId_t m_lastDisplayCreated = ramses::InvalidDisplayId;
        bool m_windowWasClosed = false;

        int32_t m_MouseLastX = 0;
        int32_t m_MouseLastY = 0;
    };
}

#endif
