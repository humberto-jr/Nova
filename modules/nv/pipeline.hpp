#if !defined(NV_PIPELINE_HEADER)
	#define NV_PIPELINE_HEADER
	#define "vulkan.hpp"

	namespace nv
	{
		class pipeline
		{
			public:
			pipeline();

			~pipeline();

			private:
			nv::vulkan::layout l;
			nv::vulkan::viewport vp;
			nv::vulkan::rasterizer r;
			nv::vulkan::multisample ms;
			nv::vulkan::color_blend cb;
			nv::vulkan::depth_stencil ds;
			nv::vulkan::pipeline pl;
		};
	}
#endif
