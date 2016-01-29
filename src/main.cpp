#include <atlas/utils/Application.hpp>

#include "Scene.hpp"

int main()
{
        APPLICATION.createWindow(800, 800, "Springs");
        APPLICATION.addScene(new Scene);
        APPLICATION.runApplication();
        return 0;
}
