#include "Scene.hpp"

#include <string>

#include <atlas/core/Log.hpp>
#include <atlas/core/GLFW.hpp>

Scene::Scene() :
        mDragging(false),
        mPaused(true),
        mPrevTime(0.0)
{
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
Scene::~Scene() { }

void Scene::mousePressEvent(int b, int a, int m, double x, double y)
{
        USING_ATLAS_CORE_NS;
        Log::log(Log::SeverityLevel::DEBUG, "Mouse Press Event: (" + std::to_string(x) + ", " + std::to_string(y) + ")");

        USING_ATLAS_MATH_NS;
        if(b == GLFW_MOUSE_BUTTON_MIDDLE)
        {
                if(a == GLFW_PRESS)
                {
                        mDragging = true;
                        Camera::CameraMovements movements;
                        switch(m)
                        {
                                case GLFW_MOD_CONTROL:
                                        movements = Camera::CameraMovements::DOLLY;
                                        break;
                                case GLFW_MOD_SHIFT:
                                        movements = Camera::CameraMovements::TRACK;
                                        break;
                                default:
                                        movements= Camera::CameraMovements::TUMBLE;
                                        break;
                        }
                        mCamera.mouseDown(Point2(x, y), movements);
                }
                else
                {
                        mDragging = false;
                        mCamera.mouseUp();
                }
        }

}

void Scene::mouseMoveEvent(double x, double y)
{
        USING_ATLAS_MATH_NS;
        if(mDragging) mCamera.mouseDrag(Point2(x, y));
}

void Scene::scrollEvent(double x, double y)
{
        USING_ATLAS_CORE_NS;
        USING_ATLAS_MATH_NS;
        Log::log(Log::SeverityLevel::DEBUG, "Mouse Scroll Event: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        mCamera.mouseScroll(Point2(x, y));
}

void Scene::keyPressEvent(int key, int scancode, int action, int modes)
{
        if (action == GLFW_PRESS)
        {
                if (key == GLFW_KEY_SPACE)
                {
                        mPaused = !mPaused;
                }
        }
}

void Scene::updateScene(double time)
{
        if(!mPaused)
        {
                mTime.deltaTime = static_cast<float>(time) - mTime.currentTime;
                mTime.totalTime += static_cast<float>(time);
                mTime.currentTime = static_cast<float>(time);
                mTime.deltaTime *= 2.f;
                mSpring.updateGeometry(mTime);
        }
}

void Scene::renderScene()
{
        const float grey = 0.631;
        glClearColor(grey, grey, grey, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        mView = mCamera.getCameraMatrix();
        mSpring.renderGeometry(mProjection, mView);
        mGrid.renderGeometry(mProjection, mView);
}

