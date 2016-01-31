#include "Spring.hpp"

// Debug
#include <atlas/core/Log.hpp>
#include <string>

// Linear Spring Implementation

Spring::Spring() :
        mPoints({atlas::math::Vector(0, 10, 0), atlas::math::Vector(0, 6, 0)}),
        mVelocity({atlas::math::Vector(0.f), atlas::math::Vector(0.f)}),
        mForce({atlas::math::Vector(0.f), atlas::math::Vector(0.f)}),
        mMass({10.f, 10.f}),
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

        // Upload reset vertex data
        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0, sizeof(Vector) * 2,
                        &mPoints[0][0]);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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
        mLength(5.f),
        mDampen(0.01f),
        mK(0.1f),
        mMass(1.f),
        mRest(0.f, glm::radians(-20.f)),
        mVelocity(0.f),
        mPosition(0.f, glm::radians(45.f))
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

        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(Vector), points.data(), GL_DYNAMIC_DRAW);

        // Create Shaders
        const std::string shader_dir = generated::ShaderPaths::getShaderDirectory();
        // The Angular vertex shader converts from spherical to Cartesian
        // coordinates
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

void AngularSpring::stepGeometry(atlas::utils::Time const& t)
{
        USING_ATLAS_CORE_NS;
        glm::vec2 x = glm::vec2(mPosition.x - mRest.x, mPosition.y - mRest.y);
        glm::vec2 F = glm::vec2(-mK * x) - glm::vec2(mDampen * mVelocity);

#ifdef PROG_DEBUG
        Log::log(Log::SeverityLevel::DEBUG, "Force: (" +
                        std::to_string(mLength) + ", " +
                        std::to_string(F.x) + ", " +
                        std::to_string(F.y) + ")");
#endif

        glm::vec2 a = F / mMass;

#ifdef PROG_DEBUG
        Log::log(Log::SeverityLevel::DEBUG, "Acceleration: (" +
                        std::to_string(mLength) + ", " +
                        std::to_string(a.x) + ", " +
                        std::to_string(a.y) + ")");
#endif

        glm::vec2 v = mVelocity + a * 0.5f;

#ifdef PROG_DEBUG
        Log::log(Log::SeverityLevel::DEBUG, "Velocity: (" +
                        std::to_string(mLength) + ", " +
                        std::to_string(v.x) + ", " +
                        std::to_string(v.y) + ")");
#endif

        glm::vec2 p = mPosition + v * 0.5f;

#ifdef PROG_DEBUG
        Log::log(Log::SeverityLevel::DEBUG, "Position: (" +
                        std::to_string(mLength) + ", " +
                        std::to_string(p.x) + ", " +
                        std::to_string(p.y) + ")");
#endif

        mVelocity = v;
        mPosition = p;

        const std::array<glm::vec3, 2> points {
                glm::vec3(0, mRest),
                glm::vec3(mLength, mPosition)
        };

        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        sizeof(glm::vec3) * 2,
                        points.data());

        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBindVertexArray(0);

}

void AngularSpring::updateGeometry(atlas::utils::Time const& t)
{
        if(mPaused) return;
        stepGeometry(t);
}

void AngularSpring::changeRest(glm::vec3 d)
{
        // x component: length
        mLength *= d.x;
        // y is the theta
        // z is the phi
        mRest = mRest + glm::vec2(glm::radians(d.y), glm::radians(d.z));

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
        mVelocity = glm::vec2(0.f);
        mPosition = glm::vec2(0.f, glm::radians(45.f));

        const std::array<glm::vec3, 2> points
        {
                glm::vec3(0.f),
                glm::vec3(mLength, mPosition)
        };


        // Upload reset vertex data
        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0, sizeof(glm::vec3) * 2,
                        points.data());
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

}
