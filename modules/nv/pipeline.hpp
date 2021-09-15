#if !defined(NV_PIPELINE_HEADER)
	#define NV_PIPELINE_HEADER

	#include "vulkan.hpp"
	#include "device.hpp"

	namespace nv
	{
		class shader;
		class pipeline;

		class pipeline
		{
			public:
			pipeline();

			void use(const nv::shader &s, const uint32_t index);

			~pipeline();

			friend void nv::device::create_pipeline(nv::pipeline &pl) const;

			friend void nv::device::destroy_pipeline(nv::pipeline &pl) const;

			private:
			nv::vulkan::layout layout;
			nv::vulkan::viewport viewport;
			nv::vulkan::rasterizer rasterizer;
			nv::vulkan::multisample multisampling;
			nv::vulkan::color_blend blending;
			nv::vulkan::depth_stencil stencil;
			nv::vulkan::pipeline interface;
		};
	}
#endif
