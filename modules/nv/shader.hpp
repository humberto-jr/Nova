#if !defined(NV_SHADER_HEADER)
	#define NV_SHADER_HEADER

	#include "vulkan.hpp"

	namespace nv
	{
		class shader
		{
			public:
			shader();

			void operator =(const std::string &filename);

			void operator =(const char filename[]);

			uint32_t size() const;

			void for_vertex_stage(const uint32_t n);

			void for_fragment_stage(const uint32_t n);

			void for_compute_stage(const uint32_t n);

			~shader();

			private:
			uint32_t vert_index;
			uint32_t frag_index;
			uint32_t comp_index;
			std::vector<nv::vulkan::shader_module> list;
		};
	}
#endif
