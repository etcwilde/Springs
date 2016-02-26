#include <atlas/utils/Application.hpp>

#include "Scene.hpp"

int main()
{

        APPLICATION.createWindow(800, 800, "Springs");
        APPLICATION.addScene(new LinearScene);
        APPLICATION.addScene(new AngularScene);
        APPLICATION.runApplication();
        return 0;
}
