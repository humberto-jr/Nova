#if !defined(NV_DEVICE_HEADER)
	#define NV_DEVICE_HEADER
	#include <string>

	#include "vulkan.hpp"

	namespace nv
	{
		class window;
		class renderer;
		class pipeline;

		class device
		{
			public:
			static void startup();

			static void shutdown();

			static uint32_t version();

			static uint32_t count();

			static std::string name(const uint32_t n);

			static bool is_integrated(const uint32_t n);

			static bool is_dedicated(const uint32_t n);

			static bool is_virtual(const uint32_t n);

			static bool is_cpu(const uint32_t n);

			static uint32_t layer_count();

			static std::string layer_name(const uint32_t n);

			static uint32_t extension_count();

			device(const uint32_t d, const uint32_t f = 0);

			uint32_t family_count() const;

			uint32_t queue_count(const uint32_t f) const;

			bool support_graphics(const uint32_t f) const;

			bool support_computation(const uint32_t f) const;

			bool support_transfer(const uint32_t f) const;

			bool support_all(const uint32_t f) const;

			void attach_surface(nv::window &w) const;

			void detach_surface(nv::window &w) const;

			void create_pipeline(nv::pipeline &pl) const;

			void destroy_pipeline(nv::pipeline &pl) const;

			void renderer_startup(nv::renderer &r, const nv::window &w) const;

			void renderer_shutdown(nv::renderer &r) const;

			~device();

			private:
			uint32_t index;
			nv::vulkan::device interface;
			nv::vulkan::command_pool pool;
			nv::vulkan::command_buffer buffer;
		};
	}
#endif
