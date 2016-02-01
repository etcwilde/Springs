#include "Scene.hpp"

#include <string>

#include <atlas/core/Log.hpp>
#include <atlas/core/GLFW.hpp>

LinearScene::LinearScene() :
        mDragging(false),
        mPaused(true),
        mPrevTime(0.0)
{
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
LinearScene::~LinearScene() { }

void LinearScene::mousePressEvent(int b, int a, int m, double x, double y)
{
        USING_ATLAS_CORE_NS;
#ifdef PROG_DEBUG
        Log::log(Log::SeverityLevel::DEBUG, "Mouse Press Event: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
#endif

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

void LinearScene::mouseMoveEvent(double x, double y)
{
        USING_ATLAS_MATH_NS;
        if(mDragging) mCamera.mouseDrag(Point2(x, y));
}

void LinearScene::scrollEvent(double x, double y)
{
        USING_ATLAS_CORE_NS;
        USING_ATLAS_MATH_NS;
#ifdef PROG_DEBUG
        Log::log(Log::SeverityLevel::DEBUG, "Mouse Scroll Event: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
#endif
        mCamera.mouseScroll(Point2(x, y));
}

void LinearScene::keyPressEvent(int key, int scancode, int action, int modes)
{
        if (action == GLFW_PRESS)
        {
                if (key == GLFW_KEY_SPACE)
                {
                        mPaused = !mPaused;
                }
                else if (key == GLFW_KEY_R)
                {
                        mSpring.resetGeometry();
                }
                else
                {
                        USING_ATLAS_MATH_NS;
                        switch(key)
                        {
                                case GLFW_KEY_W:
                                        mSpring.moveFixed(Vector(1, 0, 0));
                                        break;
                                case GLFW_KEY_S:
                                        mSpring.moveFixed(Vector(-1, 0, 0));
                                        break;
                                case GLFW_KEY_A:
                                        mSpring.moveFixed(Vector(0, 0, -1));
                                        break;
                                case GLFW_KEY_D:
                                        mSpring.moveFixed(Vector(0, 0, 1));
                                        break;
                                case GLFW_KEY_Q:
                                        mSpring.changeLength(1.25);
                                        break;
                                case GLFW_KEY_E:
                                        mSpring.changeLength(0.25);
                                        break;
                                case GLFW_KEY_Z:
                                        mSpring.changeMass(0.5f);
                                        break;
                                case GLFW_KEY_X:
                                        mSpring.changeMass(-0.5f);
                                        break;


                        }
                }
        }
}

void LinearScene::updateScene(double time)
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

void LinearScene::renderScene()
{
        const float grey = 0.631;
        glClearColor(grey, grey, grey, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        mView = mCamera.getCameraMatrix();
        mSpring.renderGeometry(mProjection, mView);
        mGrid.renderGeometry(mProjection, mView);
}

AngularScene::AngularScene() :
        mDragging(false),
        mPaused(true),
        mPrevTime(0.f)
{
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
}

AngularScene::~AngularScene() { }

void AngularScene::mousePressEvent(int b, int a, int m, double x, double y)
{
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

void AngularScene::mouseMoveEvent(double x, double y)
{
        USING_ATLAS_MATH_NS;
        if(mDragging) mCamera.mouseDrag(Point2(x, y));
}

void AngularScene::scrollEvent(double x, double y)
{
        USING_ATLAS_MATH_NS;
        mCamera.mouseScroll(Point2(x, y));
}

void AngularScene::keyPressEvent(int key, int scancode, int action, int modes)
{
        if(action == GLFW_PRESS)
        {
                if(key == GLFW_KEY_SPACE) mPaused = !mPaused;
                else if (key == GLFW_KEY_S && modes == GLFW_MOD_CONTROL)
                {
                        mTime.deltaTime = 0.5f;
                        mTime.totalTime += 0.5f;
                        mSpring.stepGeometry(mTime);
                }
                else if (key == GLFW_KEY_R) mSpring.resetGeometry();
                else
                {
                        USING_ATLAS_MATH_NS;
                        switch(key)
                        {
                                case GLFW_KEY_Q:
                                        mSpring.changeRest(glm::vec3(1.25f, 0.f, 0.f));
                                        break;
                                case GLFW_KEY_E:
                                        mSpring.changeRest(glm::vec3(0.25, 0.f, 0.f));
                                        break;
                                case GLFW_KEY_W:
                                        mSpring.changeRest(glm::vec3(1.f, 1.1f, 0.f));
                                        break;
                                case GLFW_KEY_S:
                                        mSpring.changeRest(glm::vec3(1.f, -1.1f, 0.f));
                                        break;
                                case GLFW_KEY_A:
                                        mSpring.changeRest(glm::vec3(1.f, 0.f, 1.f));
                                        break;
                                case GLFW_KEY_D:
                                        mSpring.changeRest(glm::vec3(1.f, 0.f, -1.f));
                                        break;
                                case GLFW_KEY_Z:
                                        mSpring.changeMass(0.5f);
                                        break;
                                case GLFW_KEY_X:
                                        mSpring.changeMass(-0.5f);
                                        break;
                                case GLFW_KEY_F:
                                        mSpring.changeK(0.5f);
                                        break;
                                case GLFW_KEY_G:
                                        mSpring.changeK(-0.5f);
                                        break;
                        }
                }

        }
}

void AngularScene::updateScene(double time)
{
        if(!mPaused)
        {
                mTime.deltaTime = static_cast<float>(time) - mTime.currentTime;
                mTime.totalTime += static_cast<float>(time);
                mTime.currentTime= static_cast<float>(time);
                mTime.deltaTime /= 10000.f;

                mSpring.updateGeometry(mTime);
        }

}

void AngularScene::renderScene()
{
        const float grey = 0.631;
        glClearColor(grey, grey, grey, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        mView = mCamera.getCameraMatrix();
        mSpring.renderGeometry(mProjection, mView);
        mGrid.renderGeometry(mProjection, mView);
}
