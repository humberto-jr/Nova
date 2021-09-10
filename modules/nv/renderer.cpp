#include "debug.hpp"
#include "renderer.hpp"

nv::renderer::renderer()
{
}

void nv::renderer::draw()
{
	// FIXME: to finish implementing the begin() function of the render pass at
	// vulkan.cpp before finishing draw().
}

nv::renderer::~renderer()
{
	if ((this->chain.handle != nullptr) || (this->pass.handle != nullptr))
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::renderer instance before calling nv::device::renderer_shutdown()")
		exit(EXIT_FAILURE);
	}
}
