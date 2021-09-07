#include "debug.hpp"
#include "renderer.hpp"

nv::renderer::renderer()
{
}

nv::renderer::~renderer()
{
	if ((this->chain.handle != nullptr) || (this->pass.handle != nullptr))
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::renderer instance before calling nv::device::renderer_shutdown()")
		exit(EXIT_FAILURE);
	}
}
