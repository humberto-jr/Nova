#if !defined(NV_RENDERER_HEADER)
	#define NV_RENDERER_HEADER
	#include <vector>

	#include "vulkan.hpp"
	#include "device.hpp"

	namespace nv
	{
		class window;

		class renderer
		{
			public:
			renderer();

			void draw();

			friend void nv::device::renderer_startup(nv::renderer &r, const nv::window &w) const;
			friend void nv::device::renderer_shutdown(nv::renderer &r) const;

			~renderer();

			private:
			nv::vulkan::image image;
			nv::vulkan::swapchain chain;
			nv::vulkan::render_pass pass;
			nv::vulkan::framebuffer frame;
			nv::vulkan::semaphore signal;
			std::vector<nv::vulkan::command_buffer> buffer;
		};
	}
#endif
