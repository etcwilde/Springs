#ifndef __SPRING_HPP
#define __SPRING_HPP

#include <atlas/utils/Geometry.hpp>

#include <glm/vec2.hpp>

#include <array>
#include <vector>
#include <cmath>

#include "ShaderPaths.hpp"

class Spring : public atlas::utils::Geometry
{
        public:
                Spring();
                ~Spring();

                void updateGeometry(atlas::utils::Time const& t) override;
                void renderGeometry(atlas::math::Matrix4 proj,
                                atlas::math::Matrix4 view) override;

                void resetGeometry() override;

                void moveFixed(atlas::math::Vector);

        private:

                std::array<atlas::math::Vector, 2> mPoints;
                std::array<atlas::math::Vector, 2> mVelocity;
                std::array<atlas::math::Vector, 2> mForce;
                std::array<float, 2> mMass;

                float mLength;
                float mDampen;
                float mK;

                bool mPaused;

                GLuint mVao;
                GLuint mVbo;
};


class AngularSpring : public atlas::utils::Geometry
{
        public:
                AngularSpring();
                ~AngularSpring();

                void updateGeometry(atlas::utils::Time const& t) override;
                void renderGeometry(atlas::math::Matrix4 proj,
                                atlas::math::Matrix4 view) override;

                void stepGeometry(atlas::utils::Time const& t);

        private:
                bool mPaused;
                float mLength;  // Length of the stick
                float mDampen;
                float mK;

                float mMass;

                glm::vec2 mRest;
                glm::vec2 mVelocity;
                glm::vec2 mPosition;

                GLuint mVao;
                GLuint mVbo;
};


#endif//__SPRING_HPP
