#include <atlas/utils/Application.hpp>

#include "Scene.hpp"

int main()
{

        APPLICATION.createWindow(800, 800, "Springs");
        // TODO: There appears to be an issue in switching between scenes in Atlas
        APPLICATION.addScene(new LinearScene);
        // APPLICATION.addScene(new AngularScene);
        APPLICATION.runApplication();
        return 0;
}
