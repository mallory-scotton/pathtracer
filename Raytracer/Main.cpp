///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Raytracer.hpp"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    try
    {
        Ray::Vector<Ray::String> args;

        for (int i = 0; i < argc; i++)
        {
            args.push_back(argv[i]);
        }

        Ray::Application app = Ray::Application(args);

        app.Run();
    }
    catch (const Ray::Exception& error)
    {
        RAY_ERROR(error.what());
        return (RAY_EXIT_FAILURE);
    }
    catch (const std::exception& error)
    {
        RAY_FATAL(RAY_UNEXPECTED + Ray::String(error.what()));
        return (RAY_EXIT_FAILURE);
    }

    return (RAY_EXIT_SUCCESS);
}
