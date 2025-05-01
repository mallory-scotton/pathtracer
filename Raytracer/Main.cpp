///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Raytracer.hpp"
#include "Errors.hpp"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    try
    {
        Ray::Raytracer application(argc, argv);

        application.Run();
    }
    catch (const Ray::Exception& error)
    {
        RAY_ERROR(error.what());
        return (RAY_EXIT_FAILURE);
    }
    catch (const std::exception& error)
    {
        RAY_FATAL(error.what());
        return (RAY_EXIT_FAILURE);
    }

    return (RAY_EXIT_SUCCESS);
}
