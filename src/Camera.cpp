#include "Camera.hpp"

struct Camera::CameraImpl
{
        CameraImpl() :
                dolly(100.f),
                tumbleVector(45.f),
                trackVector(0.f),
                lastPos(0.f),
                movement(Camera::CameraMovements::IDLE)
        {
                resetMatrix();
        }

        void resetAll()
        {
                USING_ATLAS_MATH_NS;
                dolly = 100.f;
                trackVector = Vector(0.f);
                tumbleVector = Vector(45.f);
                resetMatrix();
        }

        void resetMatrix()
        {
                USING_ATLAS_MATH_NS;
                dollyMatrix = glm::translate(Matrix4(1.f), Vector(0.f, 0.f, -1.f * dolly));
                trackMatrix = Matrix4(1.f);
                tumbleMatrix =
                        glm::rotate(Matrix4(1.f), glm::radians(tumbleVector.x), Vector(1, 0, 0)) *
                        glm::rotate(Matrix4(1.f), glm::radians(tumbleVector.y), Vector(1, 0, 0));
        }

        float dolly;
        atlas::math::Vector tumbleVector;
        atlas::math::Vector trackVector;
        atlas::math::Point2 lastPos;
        Camera::CameraMovements movement;

        atlas::math::Matrix4 dollyMatrix;
        atlas::math::Matrix4 tumbleMatrix;
        atlas::math::Matrix4 trackMatrix;
};

Camera::Camera() : mImpl(new CameraImpl) {}
Camera::~Camera() {}

void Camera::mouseDown(atlas::math::Point2 const& p, Camera::CameraMovements m)
{
        mImpl->movement = m;
        mImpl->lastPos = p;
}

void Camera::mouseDrag(atlas::math::Point2 const& p)
{
        USING_ATLAS_MATH_NS;
        Point2 delta = p - mImpl->lastPos;
        switch(mImpl->movement)
        {
                case Camera::CameraMovements::DOLLY:
                        mImpl->dolly -= 0.08f * (delta.y + delta.x);
                        mImpl->dollyMatrix =
                                glm::translate(Matrix4(1.f), Vector(0, 0, -1.f * mImpl->dolly));
                        break;
                case Camera::CameraMovements::TUMBLE:
                        mImpl->tumbleVector.x += 0.25f * delta.y;
                        mImpl->tumbleVector.y += 0.25f * delta.x;
                        // Tumbles with Euler Angles -- Replacing this with quats
                        // would be good
                        mImpl->tumbleMatrix =
                                glm::rotate(Matrix4(1.f), glm::radians(mImpl->tumbleVector.x), Vector(1, 0, 0)) *
                                glm::rotate(Matrix4(1.f), glm::radians(mImpl->tumbleVector.y), Vector(0, 1, 0));
                        break;
                case Camera::CameraMovements::TRACK:
                        mImpl->trackVector.x += 0.05f * delta.x;
                        mImpl->trackVector.y -= 0.05f * delta.y;
                        mImpl->trackMatrix = glm::translate(Matrix4(1.f), mImpl->trackVector);
                        break;

                case Camera::CameraMovements::IDLE:
                        break;
        }
        mImpl->lastPos = p;
}

void Camera::mouseScroll(atlas::math::Point2 const& p)
{
        USING_ATLAS_MATH_NS;
        mImpl->dolly -= p.y;
        mImpl->dollyMatrix = glm::translate(Matrix4(1.f), Vector(0, 0, -1.f * mImpl->dolly));
}

void Camera::mouseUp()
{
        mImpl->movement = Camera::CameraMovements::IDLE;
}

void Camera::resetCamera()
{
        mImpl->resetAll();
}

atlas::math::Matrix4 Camera::getCameraMatrix()
{
        return mImpl->dollyMatrix * mImpl->trackMatrix * mImpl->tumbleMatrix;
}
