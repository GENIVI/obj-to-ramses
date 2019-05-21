//  -------------------------------------------------------------------------
//  Copyright (C) 2019 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#include "ObjImporter.h"

#include "ramses-client-api/Scene.h"
#include "ramses-framework-api/RamsesFramework.h"
#include "ramses-client-api/RamsesClient.h"
#include "ramses-client-api/ResourceFileDescriptionSet.h"
#include "ramses-client-api/ResourceFileDescription.h"
#include "ramses-client-api/Camera.h"
#include "ramses-client-api/LocalCamera.h"
#include "ramses-client-api/PerspectiveCamera.h"
#include "ramses-client-api/RenderPass.h"

#include "Utils/RamsesLogger.h"
#include "RendererEventHandler.h"
#include "SceneToText.h"
#include <iostream>

int main(int argc, char* argv[])
{
    ramses::RamsesFrameworkConfig config(argc, argv);
    config.setRequestedRamsesShellType(ramses::ERamsesShellType_Console);  //needed for automated test of examples
    ramses::RamsesFramework framework(config);
    ramses::RamsesClient client("obj2ramses", framework);

    // Suppress ramses logs
    ramses_internal::GetRamsesLogger().setLogLevelForAppenderType(ramses_internal::ELogAppenderType::Console, ramses_internal::ELogLevel::Error);

    // Create a renderer for visualization
    // TODO: make renderer optional
    ramses::RendererConfig rendererConfig(argc, argv);
    ramses::RamsesRenderer renderer(framework, rendererConfig);
    renderer.startThread();

    ramses::DisplayConfig displayConfig;
    uint32_t screenWidth = 1200U;
    uint32_t screenHeight = 480U;
    displayConfig.setWindowRectangle(0, 0, 1280U, 480U);
    //displayConfig.setPerspectiveProjection(19.f, 1200U/480U, 0.1f, 1500.f);
    const ramses::displayId_t display = renderer.createDisplay(displayConfig);

    framework.connect();

    // TODO make scene id and name configurable over cmd line
    ramses::sceneId_t sceneId = 123u;
    const char* sceneName = "The Scene";
    ramses::SceneConfig sceneConfig;
    ramses::Scene* scene = client.createScene(sceneId, sceneConfig, sceneName);

    obj2ramses::ObjImporter objImporter(client, *scene);
    objImporter.importFromFile("res/suzanne.obj");

    // every scene needs a render pass with camera
    const char* CAMERA_NAME = "Default Camera";

    ramses::PerspectiveCamera* camera = scene->createPerspectiveCamera(CAMERA_NAME);
    camera->setFrustum(19.f, 1280.f/480.f, 0.1f, 1500.f);
    camera->setViewport(1280U, 480U, 1280U, 480U);
    ramses::RenderPass* renderPass = scene->createRenderPass("my render pass");
    renderPass->setClearFlags(ramses::EClearFlags_None);
    renderPass->setCamera(*camera);

    ramses::RenderGroup* renderGroup = objImporter.getRamsesRenderGroup();
    renderPass->addRenderGroup(*renderGroup);



    ramses::status_t status = client.validate();

    if (ramses::StatusOK != status)
    {
        std::cout << "Validation test for imported scene failed: " << client.getValidationReport(ramses::EValidationSeverity_Info);
        return 1;
    }

    // TODO make configurable
    obj2ramses::SceneToText sceneToText(true);
    std::ostringstream outputStream;
    sceneToText.printToStream(*scene, client, outputStream);
    std::cout << outputStream.str();

    scene->publish();
    scene->flush();

    // Needed for visual debugging tools
    renderer.setSkippingOfUnmodifiedBuffers(false);

    obj2ramses::SceneStateEventHandler eventHandler(renderer, *camera);

    eventHandler.waitForPublication(sceneId);

    renderer.subscribeScene(sceneId);
    renderer.flush();
    eventHandler.waitForSubscription(sceneId);

    renderer.mapScene(display, sceneId);
    renderer.flush();
    eventHandler.waitForMapped(sceneId);

    renderer.showScene(sceneId);
    renderer.flush();

    while (!eventHandler.windowWasClosed())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        renderer.dispatchEvents(eventHandler);
    }

    return 0;
}
