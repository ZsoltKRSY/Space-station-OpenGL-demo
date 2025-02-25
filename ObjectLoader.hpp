#ifndef ObjectLoader_hpp
#define ObjectLoader_hpp

#include <glm.hpp> //core glm functionality
#include <gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Shader.hpp"
#include "Model3D.hpp"

#include <iostream>

namespace gps {
    class ObjectLoader
    {
    public:
        ObjectLoader();
        void resetMovingObjectsPosition();
        void initObjects();
        void initUniforms(gps::Shader shaderTexture, gps::Shader shaderColorOnly, gps::Shader depthMapShader,
            gps::Window myWindow, gps::Camera myCamera,
            glm::mat4& projection, glm::vec3 lightDir, glm::vec3 lightColor, glm::mat4 dirLightSpaceTrMatrix);

        //render objects
        void render_moon(gps::Shader shader, glm::mat4 view, bool depthPass);
        void render_space_station(gps::Shader shaderTex, gps::Shader shaderColor, glm::mat4 view, bool depthPass);
        void render_space_station_windows(gps::Shader shader, glm::mat4 view, bool depthPass);
        void render_holograms(gps::Shader shader, glm::mat4 view, bool depthPass);
        void render_shuttle(gps::Shader shaderTex, gps::Shader shaderColor, glm::mat4 view, bool depthPass);
        void render_spaceship1(gps::Shader shaderTex, gps::Shader shaderColor, glm::mat4 view, bool depthPass);
        void render_spaceship2(gps::Shader shaderTex, glm::mat4 view, bool depthPass);
        void render_spaceship3(gps::Shader shaderTex, glm::mat4 view, bool depthPass);
        void render_spaceship4(gps::Shader shaderTex, glm::mat4 view, bool depthPass);
        void render_satellite(gps::Shader shader, glm::mat4 view, bool depthPass);
        void render_vulcan(gps::Shader shader, glm::mat4 view, bool depthPass);
        void render_negh(gps::Shader shader, glm::mat4 view, bool depthPass);
        void render_equinox(gps::Shader shader, glm::mat4 view, bool depthPass);

        //draw + animate objects/lights
        void drawObjects(gps::Shader shaderTex, gps::Shader shaderColor, gps::Camera myCamera, glm::mat4 view, bool depthPass);
        void objectsMovement(gps::Camera myCamera, float deltaTime);
        void operateLights(gps::Shader shaderTexture, gps::Shader shaderColorOnly, float deltaTime, float counterCoeff);

    private:
        glm::mat4 model;
        glm::mat3 normalMatrix;

        glm::mat4 getTransformedModel(glm::vec3 objectLocation, glm::vec3 objectRotation, glm::mat4 objectModel);
        glm::mat4 getInverseTransformedModel(glm::vec3 objectLocation, glm::vec3 objectRotation, glm::mat4 objectModel);
        void operateDoors(gps::Camera myCamera, float deltaTime);

        //space station
        gps::Model3D space_station;
        gps::Model3D space_station_COLOR_ONLY;
        glm::vec4 space_station_COLOR_ONLY_color = glm::vec4(0.036f, 0.056f, 0.054f, 1.0f);
        gps::Model3D space_station_windows;
        glm::vec4 space_station_windows_color = glm::vec4(0.15f, 0.18f, 0.29f, 0.2f);
        float window_color_factor = 0.25f;
        glm::vec3 corridor_spotlight1_location = glm::vec3(-34.181667f, 3.996974f, -36.415077f);
        glm::vec3 corridor_spotlight2_location = glm::vec3(-34.181667f, 3.996974f, -22.892504f);
        glm::vec3 corridor_spotlight3_location = glm::vec3(-34.181667f, 3.996974f, 22.89250f);
        glm::vec3 corridor_spotlight4_location = glm::vec3(-34.181667f, 3.996974f, 36.415077f);
        glm::vec3 corridor_spotlight_color = glm::vec3(1.0f, 1.0f, 1.0f);

        //doors
        gps::Model3D doors_lower[4];
        gps::Model3D doors_upper[4];
        glm::vec3 doors_location[4] = { glm::vec3(-34.17646f, 0.548385f, -49.03632f), glm::vec3(-34.17646f, 0.0f, 11.23378f),
                                        glm::vec3(-34.17646f, 0.0f, 49.17196f), glm::vec3(-34.17646f, 0.0f, -11.63370f) };
        glm::vec3 doors_relative_location[4] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };
        glm::vec3 doors_movement = glm::vec3(0.0f, 3.0f, 0.0f);

        //holograms
        gps::Model3D hologram1, hologram2, hologram3;
        //gps::Model3D holograms_static;
        glm::vec4 holograms_static_color = glm::vec4(0.36f, 0.91f, 0.69f, 0.75f);
        glm::vec3 hologram1_location = glm::vec3(-34.17646f, 0.0f, 0.0f);
        glm::vec3 hologram2_location = glm::vec3(-34.17646f, 0.0f, 60.70209f);
        glm::vec3 hologram3_location = glm::vec3(-34.17646f, 0.0f, -60.51411f);
        glm::vec3 hologram1_2_3_rotation = glm::vec3(90.0f, 0.0f, 0.0f);
        GLfloat hologram1_2_3_angle = 0.0f;
        glm::vec4 holograms_color = glm::vec4(0.11f, 0.91f, 0.33f, 0.3f);
        float holograms_color_factor = 1.0f;

        //shuttle
        gps::Model3D shuttle;
        gps::Model3D shuttle_windows;
        glm::vec3 shuttle_location = glm::vec3(-69.87289f, -8.82985f, -86.62903f);
        glm::vec4 shuttle_windows_color = glm::vec4(0.11f, 0.13f, 0.18f, 0.7f);

        //spaceship1
        gps::Model3D spaceship1;
        gps::Model3D spaceship1_window;
        glm::vec3 spaceship1_location = glm::vec3(48.90971f, 6.77355f, 247.61014f);
        glm::vec3 spaceship1_rotation = glm::vec3(90.0f, 6.1253f, 0.0f);
        glm::vec3 spaceship1_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 spaceship1_movement = glm::vec3(0.0f, -40.0f, 0.0f);
        glm::vec4 spaceship1_window_color = glm::vec4(0.11f, 0.13f, 0.18f, 0.7f);

        //spaceship2
        gps::Model3D spaceship2;
        glm::vec3 spaceship2_location = glm::vec3(38.47719f, 1.88973f, -183.35263f);
        glm::vec3 spaceship2_rotation = glm::vec3(89.501f, -12.704f, 5.6872f);
        glm::vec3 spaceship2_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 spaceship2_movement = glm::vec3(5.0f, 20.0f, 0.0f);

        //spaceship3
        gps::Model3D spaceship3;
        glm::vec3 spaceship3_location = glm::vec3(-106.49129f, -2.67510f, 411.74261f);
        glm::vec3 spaceship3_rotation = glm::vec3(90.0f, -180.0f, 0.0f);
        glm::vec3 spaceship3_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 spaceship3_movement = glm::vec3(0.0f, 10.0f, 0.0f);

        //spaceship4
        gps::Model3D spaceship4;
        glm::vec3 spaceship4_location = glm::vec3(-138.18718f, 13.43824f, 285.84692f);
        glm::vec3 spaceship4_rotation = glm::vec3(90.0f, -180.0f, 0.0f);
        glm::vec3 spaceship4_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 spaceship4_movement = glm::vec3(0.0f, 40.0f, 0.0f);

        //satellite
        gps::Model3D satellite;
        GLfloat satellite_angle = 0.0f;

        //vulcan_dkyr
        gps::Model3D vulcan_ship;
        gps::Model3D vulcan_cylinder;
        glm::vec3 vulcan_location = glm::vec3(97.27872f, -1.68203f, 128.12694f);
        glm::vec3 vulcan_rotation = glm::vec3(89.804f, -7.4911f, 2.6422f);
        GLfloat vulcan_cylinder_angle = 0.0f;
        glm::vec3 vulcan_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 vulcan_movement = glm::vec3(0.0f, -1.5f, 0.0f);
        glm::vec3 vulcan_light1_location = glm::vec3(106.67035f, 6.11393f, 54.59740f);
        glm::vec3 vulcan_light2_location = glm::vec3(97.28089f, -0.36717f, 126.72747);
        glm::vec3 vulcan_light_color = glm::vec3(1.0f, 1.0f, 1.0f);

        //negh
        gps::Model3D negh;
        glm::vec3 negh_location = glm::vec3(-202.67383f, -24.53480f, -226.03316f);
        glm::vec3 negh_rotation = glm::vec3(90.0f, 4.5156f, -6.7994f);
        glm::vec3 negh_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 negh_movement = glm::vec3(0.0f, 3.5f, 0.0f);
        glm::vec3 negh_light1_location = glm::vec3(-137.42393f, -8.59155f, -167.36633f);
        glm::vec3 negh_light2_location = glm::vec3(-132.79286f, -19.90234f, -167.36633f);
        glm::vec3 negh_light3_location = glm::vec3(-217.88655f, -14.28489f, -162.86459f);
        glm::vec3 negh_light4_location = glm::vec3(-219.64703f, -26.29395f, -162.86459f);
        glm::vec3 negh_light_color = glm::vec3(1.0f, 0.0f, 0.0f);
        float negh_lights_counter = 0.0f;
        bool negh_light_isOn = true;

        //equinox
        gps::Model3D equinox;
        glm::vec3 equinox_location = glm::vec3(-333.12082f, -39.33855f, 15.97041f);
        glm::vec3 equinox_rotation = glm::vec3(83.143f, -6.5434, -6.2516);
        glm::vec3 equinox_relativeLocation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 equinox_movement = glm::vec3(0.0f, 2.0f, 0.0f);

        //moon
        gps::Model3D moon;
    };
}

#endif /* ObjectLoader_hpp */