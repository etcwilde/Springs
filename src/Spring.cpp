#include "Spring.hpp"

// Debug
#include <atlas/core/Log.hpp>
#include <string>

// Linear Spring Implementation

Spring::Spring() :
        mPoints({atlas::math::Vector(0, 10, 0), atlas::math::Vector(0, 6, 0)}),
        mVelocity({atlas::math::Vector(0.f), atlas::math::Vector(0.f)}),
        mForce({atlas::math::Vector(0.f), atlas::math::Vector(0.f)}),
        mMass({10.f, 1.f}),
        mLength(4.f),
        mDampen(0.15f),
        mK(4.f),
        mPaused(false)
{
        USING_ATLAS_GL_NS;
        USING_ATLAS_CORE_NS;
        USING_ATLAS_MATH_NS;

        // Create Vao
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao); // Use this vertex array object
        glGenBuffers(1, &mVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo); // Use this vertex buffer object
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector) * 2, mPoints.data(), GL_DYNAMIC_DRAW);

        // Create Shader Programs
        const std::string shader_dir = generated::ShaderPaths::getShaderDirectory();
        std::vector<ShaderInfo> shaders
        {
                { GL_VERTEX_SHADER, shader_dir + "grid.vs.glsl"},
                { GL_FRAGMENT_SHADER, shader_dir + "grid.fs.glsl"}
        };

        mShaders.push_back(ShaderPointer(new Shader));
        mShaders[0]->compileShaders(shaders);
        mShaders[0]->linkShaders();

        // Get uniform variables
        GLuint varID;
        varID = mShaders[0]->getUniformVariable("MVP");
        mUniforms.insert(UniformKey("MVP", varID));
        varID = mShaders[0]->getUniformVariable("color");
        mUniforms.insert(UniformKey("color", varID));

        // Set the attribute pointer
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0); // Disconnect the vertex array object
        mShaders[0]->disableShaders();
}

Spring::~Spring()
{
        glDeleteVertexArrays(1, &mVao);
        glDeleteBuffers(1, &mVbo);
}

void Spring::updateGeometry(atlas::utils::Time const& t)
{
        if(mPaused) return;
        USING_ATLAS_MATH_NS;
        const Vector g = Vector(0, -9.087, 0);

        Vector F = Vector(0.f);
        Vector s = Vector(0.f);
        // External Forces acting on the spring
        F = g + mForce[1] - mDampen * mVelocity[1];

        Vector d = mPoints[1] - mPoints[0];
        float x = glm::length(d);
        F += mK * (mLength - x) * glm::normalize(d);

        Vector a = F / mMass[1];

        s = mVelocity[1] * t.deltaTime + 0.5f * a * t.deltaTime * t.deltaTime;
        mVelocity[1] = mVelocity[1] + a * t.deltaTime;
        mPoints[1] = mPoints[1] + s;

        USING_ATLAS_CORE_NS;

        // Upload point data to the GPU
        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);

        // Only need to update the one point
        glBufferSubData(GL_ARRAY_BUFFER,
                        sizeof(Vector),
                        sizeof(Vector),
                        &mPoints[1][0]);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Spring::renderGeometry(
                atlas::math::Matrix4 proj,
                atlas::math::Matrix4 view)
{
        USING_ATLAS_MATH_NS;
        mShaders[0]->enableShaders();
        glBindVertexArray(mVao);
        Matrix4 mvp = proj * view * mModel;
        glUniformMatrix4fv(mUniforms["MVP"], 1, GL_FALSE, &mvp[0][0]);
        GLfloat color[] = {0.1, 0.4, 0.7};
        glUniform3fv(mUniforms["color"], 1, color);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        mShaders[0]->disableShaders();
}

void Spring::resetGeometry()
{
        USING_ATLAS_MATH_NS;
        mPoints = {Vector(0, 10, 0), Vector(0, -1, 0)};
        mVelocity = {Vector(0.f), Vector(0.f)};
        mForce = {Vector(0.f), Vector(0.f)};
        mLength = 1.f;
        // I think we need to re-upload the position of the first vertex
}

void Spring::moveFixed(atlas::math::Vector vec)
{
        USING_ATLAS_MATH_NS;
        mPoints[0] += vec;

        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferSubData(GL_ARRAY_BUFFER,
                0, sizeof(Vector), &mPoints[0][0]);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}


// Angular Spring Implementation

AngularSpring::AngularSpring() :
        mPaused(false),
        mLength(50.f),
        mDampen(6.5f),
        mK(0.01f),
        mMass(10.f),
        mRest(glm::vec2(glm::radians(45.f), glm::radians(45.f))),
        mVelocity(0.f),
        mPosition(glm::vec2(glm::radians(45.f), glm::radians(45.f)))
{
        USING_ATLAS_GL_NS;
        USING_ATLAS_CORE_NS;
        USING_ATLAS_MATH_NS;

        // Create Vertex Array object
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        // Create Vertex buffer object
        glGenBuffers(1, &mVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);

        const std::array<glm::vec3, 2> points
        {
                glm::vec3(0.f),
                glm::vec3(mLength, mPosition)
        };

        glBufferData(GL_ARRAY_BUFFER, 2* sizeof(Vector), points.data(), GL_DYNAMIC_DRAW);

        // Create Shaders
        const std::string shader_dir = generated::ShaderPaths::getShaderDirectory();
        std::vector<ShaderInfo> shaders
        {
                {GL_VERTEX_SHADER, shader_dir + "angular.vs.glsl"},
                {GL_FRAGMENT_SHADER, shader_dir + "grid.fs.glsl"}
        };

        mShaders.push_back(ShaderPointer(new Shader));
        mShaders[0]->compileShaders(shaders);
        mShaders[0]->linkShaders();

        // Uniform variables
        GLuint varID;
        varID = mShaders[0]->getUniformVariable("MVP");
        mUniforms.insert(UniformKey("MVP", varID));
        varID = mShaders[0]->getUniformVariable("color");
        mUniforms.insert(UniformKey("color", varID));

        // Set attribute pointer -- Position Vectors
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0); // Disconnect
        mShaders[0]->disableShaders();
}

AngularSpring::~AngularSpring()
{
        glDeleteVertexArrays(1, &mVao);
        glDeleteBuffers(1, &mVbo);
}

void AngularSpring::updateGeometry(atlas::utils::Time const& t)
{
        // Okay, be all springy in the spherical coordinate system
        if(mPaused) return;
        USING_ATLAS_MATH_NS;
        USING_ATLAS_CORE_NS;

        Vector F = Vector(0.f);
        // Gravity - dampening
        F = glm::vec3(-9.807, 3.14159265382, 0) - glm::vec3(mLength, mVelocity.x * mDampen, mVelocity.y * mDampen);


        // Internal force

        // Finternal = -K * x
        F += glm::vec3(1, -mK * (mRest.x - mPosition.x), -mK * (mRest.y -  mPosition.y));
        glm::vec3 a = glm::vec3(F.x / mMass, F.y, F.z);
        glm::vec3 s = glm::vec3(1.f,
                        mVelocity.x * t.deltaTime + 0.5f * a.y * t.deltaTime * t.deltaTime,
                        mVelocity.y * t.deltaTime + 0.5f * a.z * t.deltaTime * t.deltaTime);

        mVelocity = glm::vec2(mVelocity.x * t.deltaTime, mVelocity.y * t.deltaTime);
        mPosition = glm::vec2(mPosition.x + s.y, mPosition.y + s.z);

        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);

        const std::array<glm::vec3, 2> coords { glm::vec3(0.f), glm::vec3(1.f, mPosition) };

        glBufferSubData(GL_ARRAY_BUFFER,
                        0, sizeof(coords),
                        coords.data());
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);




        // "acceleration of angle" + "velocity of angle" + "velocity of angle"
        // ml^2 d_theta^2/d_t^2 + c d_t

        // Normal spring
        // F = -kx  ( spring constant * displacement)
        // F = -k * delta_theta (spring constant * displacement)


        //F = mK * (mAngle - mRestAngle)

}



void AngularSpring::renderGeometry(atlas::math::Matrix4 proj,
                atlas::math::Matrix4 view)
{
        USING_ATLAS_MATH_NS;
        mShaders[0]->enableShaders();
        glBindVertexArray(mVao);
        Matrix4 mvp = proj * view * mModel;
        glUniformMatrix4fv(mUniforms["MVP"], 1, GL_FALSE, &mvp[0][0]);
        GLfloat color[] = {0.1, 0.5, 0.8};
        glUniform3fv(mUniforms["color"], 1, color);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        mShaders[0]->disableShaders();
}

void AngularSpring::resetGeometry()
{

}
