#if !defined(NV_SHADER_HEADER)
	#define NV_SHADER_HEADER

	#include "vulkan.hpp"
	#include "pipeline.hpp"

	namespace nv
	{
		class shader
		{
			public:
			shader();

			void push(const std::string &filename);

			void push(const char filename[]);

			uint32_t size() const;

			void for_vertex_stage(const uint32_t n);

			void for_fragment_stage(const uint32_t n);

			void for_compute_stage(const uint32_t n);

			~shader();

			friend void nv::pipeline::use(const nv::shader &s, const uint32_t index);

			private:
			uint32_t vert_index;
			uint32_t frag_index;
			uint32_t comp_index;
			std::vector<nv::vulkan::shader_module> list;
		};
	}
#endif
