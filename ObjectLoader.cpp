#include "ObjectLoader.hpp"

namespace gps {

    ObjectLoader::ObjectLoader()
    {
        model = glm::mat4(1.0f);
        normalMatrix = glm::mat3(1.0f);
    }

    void ObjectLoader::resetMovingObjectsPosition() {
        spaceship1_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        spaceship2_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        spaceship3_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        spaceship4_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        vulcan_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        negh_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        equinox_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void ObjectLoader::initObjects() {
        space_station.LoadModel("objects/space_station_MAIN/space_station.obj");
        space_station_COLOR_ONLY.LoadModel("objects/space_station_MAIN/space_station_COLOR_ONLY.obj");
        space_station_windows.LoadModel("objects/space_station_MAIN/space_station_windows.obj");
        doors_lower[0].LoadModel("objects/doors/door1_lower.obj");
        doors_upper[0].LoadModel("objects/doors/door1_upper.obj");
        doors_lower[1].LoadModel("objects/doors/door2_lower.obj");
        doors_upper[1].LoadModel("objects/doors/door2_upper.obj");
        doors_lower[2].LoadModel("objects/doors/door3_lower.obj");
        doors_upper[2].LoadModel("objects/doors/door3_upper.obj");
        doors_lower[3].LoadModel("objects/doors/door4_lower.obj");
        doors_upper[3].LoadModel("objects/doors/door4_upper.obj");
        hologram1.LoadModel("objects/holograms/hologram1.obj");
        hologram2.LoadModel("objects/holograms/hologram2.obj");
        hologram3.LoadModel("objects/holograms/hologram3.obj");
        spaceship1.LoadModel("objects/spaceship1/spaceship1.obj");
        spaceship1_window.LoadModel("objects/spaceship1/spaceship1_window.obj");
        spaceship2.LoadModel("objects/spaceship2_3_4/spaceship2.obj");
        spaceship3.LoadModel("objects/spaceship2_3_4/spaceship3.obj");
        spaceship4.LoadModel("objects/spaceship2_3_4/spaceship4.obj");
        satellite.LoadModel("objects/satellite/satellite.obj");
        shuttle.LoadModel("objects/shuttle/shuttle.obj");
        shuttle_windows.LoadModel("objects/shuttle/shuttle_windows.obj");
        vulcan_ship.LoadModel("objects/vulcan/vulcan_ship.obj");
        vulcan_cylinder.LoadModel("objects/vulcan/vulcan_cylinder.obj");
        negh.LoadModel("objects/negh/negh.obj");
        equinox.LoadModel("objects/equinox/equinox.obj");
        moon.LoadModel("objects/moon/moon.obj");
    }

    void ObjectLoader::initUniforms(gps::Shader shaderTexture, gps::Shader shaderColorOnly, gps::Shader depthMapShader,
                                    gps::Window myWindow, gps::Camera myCamera,
                                    glm::mat4 &projection, glm::vec3 lightDir, glm::vec3 lightColor, glm::mat4 dirLightSpaceTrMatrix) {
        //initialize matrices + set uniforms for shaderTexture
        shaderTexture.useShaderProgram();

        // create projection matrix
        projection = glm::perspective(glm::radians(55.0f),
            (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
            0.1f, 2000.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderTexture.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        //set the light direction (direction towards the light)
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));

        //set light color
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        //set hologram locations and color
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "hologram1LightLoc"), 1, glm::value_ptr(hologram1_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "hologram2LightLoc"), 1, glm::value_ptr(hologram2_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "hologram3LightLoc"), 1, glm::value_ptr(hologram3_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "hologram1_2_3LightColor"), 1, glm::value_ptr(holograms_color));

        //set vulcan light source and color
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "vulcanLight1Loc"), 1, glm::value_ptr(vulcan_light1_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "vulcanLight2Loc"), 1, glm::value_ptr(vulcan_light2_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "vulcanLightColor"), 1, glm::value_ptr(vulcan_light_color));

        //set negh light source and color
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "neghLight1Loc"), 1, glm::value_ptr(negh_light1_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "neghLight2Loc"), 1, glm::value_ptr(negh_light2_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "neghLight3Loc"), 1, glm::value_ptr(negh_light3_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "neghLight4Loc"), 1, glm::value_ptr(negh_light4_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "neghLightColor"), 1, glm::value_ptr(negh_light_color));
        glUniform1i(glGetUniformLocation(shaderTexture.shaderProgram, "neghLightIsOn"), negh_light_isOn);

        //set corridot spotlight source and color
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "corridorLight1Loc"), 1, glm::value_ptr(corridor_spotlight1_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "corridorLight2Loc"), 1, glm::value_ptr(corridor_spotlight2_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "corridorLight3Loc"), 1, glm::value_ptr(corridor_spotlight3_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "corridorLight4Loc"), 1, glm::value_ptr(corridor_spotlight4_location));
        glUniform3fv(glGetUniformLocation(shaderTexture.shaderProgram, "corridorLightColor"), 1, glm::value_ptr(corridor_spotlight_color));

        //set direction light space transformation matrix
        glUniformMatrix4fv(glGetUniformLocation(shaderTexture.shaderProgram, "dirLightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(dirLightSpaceTrMatrix));

        //set fog check variable
        glUniform1i(glGetUniformLocation(shaderTexture.shaderProgram, "isFog"), true);

        //set uniforms for shaderColorOnly
        shaderColorOnly.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "hologram1LightLoc"), 1, glm::value_ptr(hologram1_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "hologram2LightLoc"), 1, glm::value_ptr(hologram2_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "hologram3LightLoc"), 1, glm::value_ptr(hologram3_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "hologram1_2_3LightColor"), 1, glm::value_ptr(holograms_color));

        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "vulcanLight1Loc"), 1, glm::value_ptr(vulcan_light1_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "vulcanLight2Loc"), 1, glm::value_ptr(vulcan_light2_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "vulcanLightColor"), 1, glm::value_ptr(vulcan_light_color));

        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "neghLight1Loc"), 1, glm::value_ptr(negh_light1_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "neghLight2Loc"), 1, glm::value_ptr(negh_light2_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "neghLight3Loc"), 1, glm::value_ptr(negh_light3_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "neghLight4Loc"), 1, glm::value_ptr(negh_light4_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "neghLightColor"), 1, glm::value_ptr(negh_light_color));
        glUniform1i(glGetUniformLocation(shaderColorOnly.shaderProgram, "neghLightIsOn"), negh_light_isOn);

        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "corridorLight1Loc"), 1, glm::value_ptr(corridor_spotlight1_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "corridorLight2Loc"), 1, glm::value_ptr(corridor_spotlight2_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "corridorLight3Loc"), 1, glm::value_ptr(corridor_spotlight3_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "corridorLight4Loc"), 1, glm::value_ptr(corridor_spotlight4_location));
        glUniform3fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "corridorLightColor"), 1, glm::value_ptr(corridor_spotlight_color));

        glUniform1f(glGetUniformLocation(shaderColorOnly.shaderProgram, "transparentFactor"), window_color_factor);

        glUniformMatrix4fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "dirLightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(dirLightSpaceTrMatrix));

        glUniform1i(glGetUniformLocation(shaderColorOnly.shaderProgram, "isFog"), true);

        //set uniforms for depthMapShader
        depthMapShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "dirLightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(dirLightSpaceTrMatrix));
    }


    //auxiliary model transformation functions
    glm::mat4 ObjectLoader::getTransformedModel(glm::vec3 objectLocation, glm::vec3 objectRotation, glm::mat4 objectModel) {
        objectModel = glm::rotate(objectModel, -glm::radians(objectRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        objectModel = glm::rotate(objectModel, -glm::radians(objectRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        objectModel = glm::rotate(objectModel, -glm::radians(objectRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        objectModel = glm::translate(objectModel, -objectLocation);

        return objectModel;
    }

    glm::mat4 ObjectLoader::getInverseTransformedModel(glm::vec3 objectLocation, glm::vec3 objectRotation, glm::mat4 objectModel) {
        objectModel = glm::translate(objectModel, objectLocation);
        objectModel = glm::rotate(objectModel, glm::radians(objectRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        objectModel = glm::rotate(objectModel, glm::radians(objectRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        objectModel = glm::rotate(objectModel, glm::radians(objectRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

        return objectModel;
    }



    //render objects
    void ObjectLoader::render_moon(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        moon.Draw(shader);
    }
    
    void ObjectLoader::render_space_station(gps::Shader shaderTex, gps::Shader shaderColor, glm::mat4 view, bool depthPass) {
        shaderTex.useShaderProgram();

        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderTex.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shaderTex.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        space_station.Draw(shaderTex);

        shaderColor.useShaderProgram();

        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderColor.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shaderColor.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform4fv(glGetUniformLocation(shaderColor.shaderProgram, "fColor"), 1, glm::value_ptr(space_station_COLOR_ONLY_color));
        }

        space_station_COLOR_ONLY.Draw(shaderColor);

        for (int i = 0; i < 4; ++i) {
            model = glm::mat4(1.0f);

            model = glm::translate(model, doors_relative_location[i]);

            glUniformMatrix4fv(glGetUniformLocation(shaderTex.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            if (!depthPass) {
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
                glUniformMatrix3fv(glGetUniformLocation(shaderTex.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
            }

            doors_upper[i].Draw(shaderColor);

            model = glm::mat4(1.0f);

            model = glm::translate(model, -doors_relative_location[i]);

            glUniformMatrix4fv(glGetUniformLocation(shaderTex.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            if (!depthPass) {
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
                glUniformMatrix3fv(glGetUniformLocation(shaderTex.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
            }

            doors_lower[i].Draw(shaderColor);
        }
    }

    void ObjectLoader::render_space_station_windows(gps::Shader shader, glm::mat4 view, bool depthPass) {
        if (!depthPass) {
            shader.useShaderProgram();

            model = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform4fv(glGetUniformLocation(shader.shaderProgram, "fColor"), 1, glm::value_ptr(space_station_windows_color));
            space_station_windows.Draw(shader);
        }
    }

    void ObjectLoader::render_holograms(gps::Shader shader, glm::mat4 view, bool depthPass) {
        if (!depthPass) {
            shader.useShaderProgram();

            glUniform1f(glGetUniformLocation(shader.shaderProgram, "transparentFactor"), holograms_color_factor);
            glUniform4fv(glGetUniformLocation(shader.shaderProgram, "fColor"), 1, glm::value_ptr(holograms_color));
            glDisable(GL_CULL_FACE);

            model = glm::mat4(1.0f);

            model = getInverseTransformedModel(hologram1_location, hologram1_2_3_rotation, model);
            model = glm::rotate(model, glm::radians(hologram1_2_3_angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = getTransformedModel(hologram1_location, hologram1_2_3_rotation, model);

            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            hologram1.Draw(shader);

            model = glm::mat4(1.0f);

            model = getInverseTransformedModel(hologram2_location, hologram1_2_3_rotation, model);
            model = glm::rotate(model, -glm::radians(hologram1_2_3_angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = getTransformedModel(hologram2_location, hologram1_2_3_rotation, model);

            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            hologram2.Draw(shader);

            model = glm::mat4(1.0f);

            model = getInverseTransformedModel(hologram3_location, hologram1_2_3_rotation, model);
            model = glm::rotate(model, -glm::radians(hologram1_2_3_angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = getTransformedModel(hologram3_location, hologram1_2_3_rotation, model);

            glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            hologram3.Draw(shader);

            glUniform1f(glGetUniformLocation(shader.shaderProgram, "transparentFactor"), window_color_factor);
            glEnable(GL_CULL_FACE);
        }
    }

    void ObjectLoader::render_shuttle(gps::Shader shaderTex, gps::Shader shaderColor, glm::mat4 view, bool depthPass) {
        shaderTex.useShaderProgram();

        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderTex.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shaderTex.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        shuttle.Draw(shaderTex);

        shaderColor.useShaderProgram();

        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderColor.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shaderColor.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform4fv(glGetUniformLocation(shaderColor.shaderProgram, "fColor"), 1, glm::value_ptr(shuttle_windows_color));
        }

        shuttle_windows.Draw(shaderColor);
    }

    void ObjectLoader::render_spaceship1(gps::Shader shaderTex, gps::Shader shaderColor, glm::mat4 view, bool depthPass) {
        shaderTex.useShaderProgram();

        model = glm::mat4(1.0f);

        model = getInverseTransformedModel(spaceship1_location, spaceship1_rotation, model);
        model = glm::translate(model, spaceship1_relativeLocation);
        model = getTransformedModel(spaceship1_location, spaceship1_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shaderTex.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shaderTex.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        spaceship1.Draw(shaderTex);

        shaderColor.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(shaderColor.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shaderColor.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform4fv(glGetUniformLocation(shaderColor.shaderProgram, "fColor"), 1, glm::value_ptr(spaceship1_window_color));
        }

        spaceship1_window.Draw(shaderColor);
    }

    void ObjectLoader::render_spaceship2(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);

        model = getInverseTransformedModel(spaceship2_location, spaceship2_rotation, model);
        model = glm::translate(model, spaceship2_relativeLocation);
        model = getTransformedModel(spaceship2_location, spaceship2_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        spaceship2.Draw(shader);
    }

    void ObjectLoader::render_spaceship3(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);

        model = getInverseTransformedModel(spaceship3_location, spaceship3_rotation, model);
        model = glm::translate(model, spaceship3_relativeLocation);
        model = getTransformedModel(spaceship3_location, spaceship3_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        spaceship3.Draw(shader);
    }

    void ObjectLoader::render_spaceship4(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);

        model = getInverseTransformedModel(spaceship4_location, spaceship4_rotation, model);
        model = glm::translate(model, spaceship4_relativeLocation);
        model = getTransformedModel(spaceship4_location, spaceship4_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        spaceship4.Draw(shader);
    }

    void ObjectLoader::render_satellite(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);

        model = glm::rotate(model, satellite_angle, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        satellite.Draw(shader);
    }

    void ObjectLoader::render_vulcan(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);

        model = getInverseTransformedModel(vulcan_location, vulcan_rotation, model);
        model = glm::translate(model, vulcan_relativeLocation);
        model = getTransformedModel(vulcan_location, vulcan_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        vulcan_ship.Draw(shader);

        model = getInverseTransformedModel(vulcan_location, vulcan_rotation, model);
        model = glm::rotate(model, glm::radians(vulcan_cylinder_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = getTransformedModel(vulcan_location, vulcan_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        vulcan_cylinder.Draw(shader);
    }

    void ObjectLoader::render_negh(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);

        model = getInverseTransformedModel(negh_location, negh_rotation, model);
        model = glm::translate(model, negh_relativeLocation);
        model = getTransformedModel(negh_location, negh_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        negh.Draw(shader);
    }

    void ObjectLoader::render_equinox(gps::Shader shader, glm::mat4 view, bool depthPass) {
        shader.useShaderProgram();

        model = glm::mat4(1.0f);

        model = getInverseTransformedModel(equinox_location, equinox_rotation, model);
        model = glm::translate(model, equinox_relativeLocation);
        model = getTransformedModel(equinox_location, equinox_rotation, model);
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }

        equinox.Draw(shader);
    }

    //draw objects
    void ObjectLoader::drawObjects(gps::Shader shaderTex, gps::Shader shaderColor, gps::Camera myCamera, glm::mat4 view, bool depthPass) {
        //render opaque objects
        render_moon(shaderTex, view, depthPass);

        render_space_station(shaderTex, shaderColor, view, depthPass);

        render_spaceship2(shaderTex, view, depthPass);

        render_spaceship3(shaderTex, view, depthPass);

        render_spaceship4(shaderTex, view, depthPass);

        render_satellite(shaderTex, view, depthPass);

        render_vulcan(shaderTex, view, depthPass);

        render_negh(shaderTex, view, depthPass);

        render_equinox(shaderTex, view, depthPass);

        //transparent objects
        glEnable(GL_BLEND);
        glm::vec3 myCameraPosition = myCamera.getCameraPosition();
        float distanceShuttle = glm::distance(myCameraPosition, shuttle_location);
        float distanceHolograms[3] = { glm::distance(myCameraPosition, hologram1_location),
           glm::distance(myCameraPosition, hologram2_location), glm::distance(myCameraPosition, hologram3_location) };

        render_spaceship1(shaderTex, shaderColor, view, depthPass);

        if (distanceHolograms[0] < 15.0f || distanceHolograms[1] < 15.0f || distanceHolograms[2] < 15.0f) {
            render_shuttle(shaderTex, shaderColor, view, depthPass);

            render_holograms(shaderColor, view, depthPass);

            render_space_station_windows(shaderColor, view, depthPass);
        }
        else {
            render_holograms(shaderColor, view, depthPass);

            render_space_station_windows(shaderColor, view, depthPass);

            render_shuttle(shaderTex, shaderColor, view, depthPass);
        }

        glDisable(GL_BLEND);
    }

    void ObjectLoader::operateDoors(gps::Camera myCamera, float deltaTime) {
        glm::vec3 myCameraPosition = myCamera.getCameraPosition();

        for (int i = 0; i < 4; ++i) {
            if (glm::distance(myCameraPosition, doors_location[i]) < 8.0f) {
                if (doors_relative_location[i].y < 1.3f) {
                    doors_relative_location[i] += deltaTime * doors_movement;
                }
            }
            else {
                if (doors_relative_location[i].y > 0.0f) {
                    doors_relative_location[i] -= deltaTime * doors_movement;
                }
            }
        }
    }

    void ObjectLoader::objectsMovement(gps::Camera myCamera, float deltaTime) {
        //doors
        operateDoors(myCamera, deltaTime);

        //holograms
        if (hologram1_2_3_angle >= 359.0f)
            hologram1_2_3_angle = 0.0f;
        else
            hologram1_2_3_angle += 0.25f;

        //spaceship1
        if (glm::length(spaceship1_relativeLocation) > 1000.0f) {
            spaceship1_relativeLocation = glm::vec3(0.0f, 400.0f, 0.0f);
        }
        else {
            spaceship1_relativeLocation += deltaTime * spaceship1_movement;
        }

        //spaceship2
        if (glm::length(spaceship2_relativeLocation) > 2000.0f) {
            spaceship2_relativeLocation = glm::vec3(0.0f);
            spaceship2_movement = glm::vec3(5.0f, 20.0f, 0.0f);
        }
        else {
            spaceship2_relativeLocation += deltaTime * spaceship2_movement;
            spaceship2_movement.y += 0.2f;
        }

        //spaceship3
        if (glm::length(spaceship3_relativeLocation) > 2000.0f) {
            spaceship3_relativeLocation = glm::vec3(0.0f);
            spaceship3_movement = glm::vec3(0.0f, 10.0f, 0.0f);
        }
        else {
            spaceship3_relativeLocation += deltaTime * spaceship3_movement;
            spaceship3_movement.y += 0.2f;
        }

        //spaceship4
        if (glm::length(spaceship4_relativeLocation) > 1000.0f) {
            spaceship4_relativeLocation = glm::vec3(0.0f);
        }
        else {
            spaceship4_relativeLocation += deltaTime * spaceship4_movement;
        }

        //satellite
        if (satellite_angle >= 359.0f)
            satellite_angle = 0.0f;
        else
            satellite_angle += 0.00001f;

        //vulcan
        if (vulcan_cylinder_angle >= 359.0f)
            vulcan_cylinder_angle = 0.0f;
        else
            vulcan_cylinder_angle += 0.1f;
        vulcan_relativeLocation += deltaTime * vulcan_movement;

        //negh
        negh_relativeLocation += deltaTime * negh_movement;

        //equinox
        equinox_relativeLocation += deltaTime * equinox_movement;
    }

    void ObjectLoader::operateLights(gps::Shader shaderTexture, gps::Shader shaderColorOnly, float deltaTime, float counterCoeff) {
        if (negh_lights_counter >= 100.0f) {
            negh_lights_counter = 0.0f;
            negh_light_isOn = !negh_light_isOn;

            shaderTexture.useShaderProgram();
            glUniform1i(glGetUniformLocation(shaderTexture.shaderProgram, "neghLightIsOn"), negh_light_isOn);

            shaderColorOnly.useShaderProgram();
            glUniform1i(glGetUniformLocation(shaderColorOnly.shaderProgram, "neghLightIsOn"), negh_light_isOn);
        }
        else {
            negh_lights_counter += deltaTime * counterCoeff;
        }
    }
}
