#ifndef __CAMERA_HPP
#define __CAMERA_HPP

#include <atlas/utils/Camera.hpp>

#include <memory>

class Camera : public atlas::utils::Camera
{
        public:
                Camera();
                ~Camera();

                void mouseDown(atlas::math::Point2 const& pt, atlas::utils::Camera::CameraMovements movement) override;
                void mouseDrag(atlas::math::Point2 const& pt) override;
                void mouseScroll(atlas::math::Point2 const& pt);
                void mouseUp() override;
                void resetCamera() override;

                atlas::math::Matrix4 getCameraMatrix() override;

        private:

                struct CameraImpl;
                std::unique_ptr<CameraImpl> mImpl;
};

#endif//__CAMERA_HPP
