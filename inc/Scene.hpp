#ifndef __SCENE_HPP
#define __SCENE_HPP

#include <atlas/utils/Scene.hpp>
#include <atlas/core/Log.hpp>

#include "Camera.hpp"
#include "Grid.hpp"
#include "Spring.hpp"

class LinearScene : public atlas::utils::Scene
{
        public:
                LinearScene();
                ~LinearScene();

                // Event Handlers
                void mousePressEvent(int b, int a, int m, double x, double y) override;
                void mouseMoveEvent(double x, double y) override;
                void scrollEvent(double x, double y) override;

                void keyPressEvent(int key, int scancode, int action, int mods) override;

                void updateScene(double time) override;
                void renderScene() override;

        private:

                bool mDragging;
                bool mPaused;
                bool mPrevTime;

                Camera mCamera;
                Grid mGrid;
                Spring mSpring;

};

class AngularScene : public atlas::utils::Scene
{
        public:
                AngularScene();
                ~AngularScene();

                // Events
                void mousePressEvent(int b, int a, int m, double x, double y) override;
                void mouseMoveEvent(double x, double y) override;
                void scrollEvent(double x, double y) override;
                void keyPressEvent(int key, int scancode, int action, int modes) override;

                // Rendering
                void updateScene(double time) override;
                void renderScene() override;
        private:
                bool mDragging;
                bool mPaused;
                bool mPrevTime;

                Camera mCamera;
                Grid mGrid;
                AngularSpring mSpring;
};

#endif//__SCENE_HPP
