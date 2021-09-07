#if !defined(NV_VULKAN_HEADER)
	#define NV_VULKAN_HEADER
	#include <vulkan/vulkan.h>
	#include <GLFW/glfw3.h>
	#include <vector>
	#include <string>

	namespace nv
	{
		namespace vulkan
		{
			struct instance
			{
				instance();

				void create();

				void destroy();

				~instance();

				VkInstance handle;
				VkApplicationInfo info;
				VkInstanceCreateInfo setup;
			};

			struct physical_device
			{
				physical_device();

				void enumerate(const nv::vulkan::instance &i);

				uint32_t count() const;

				~physical_device();

				std::vector<VkPhysicalDevice> handle;
				std::vector<VkPhysicalDeviceProperties> properties;
				std::vector<VkPhysicalDeviceMemoryProperties> memory;
			};

			struct device
			{
				device();

				void create(const nv::vulkan::physical_device &d, const uint32_t index);

				void destroy();

				~device();

				VkDevice handle;
				VkDeviceCreateInfo setup;
				std::vector<float> priority;
				VkDeviceQueueCreateInfo info;
				std::vector<VkQueueFamilyProperties> family;
			};

			struct command_pool
			{
				command_pool();

				void create(const nv::vulkan::device &d);

				void destroy(const nv::vulkan::device &d);

				~command_pool();

				VkQueue queue;
				VkCommandPool handle;
				VkCommandPoolCreateInfo setup;
			};

			struct command_buffer
			{
				command_buffer();

				void allocate(const nv::vulkan::device &d, nv::vulkan::command_pool &p);

				~command_buffer();

				VkCommandBuffer handle;
				VkCommandBufferAllocateInfo setup;
			};

			struct semaphore
			{
				semaphore();

				void create(const nv::vulkan::device &d);

				void destroy(const nv::vulkan::device &d);

				uint32_t size() const;

				~semaphore();

				std::vector<VkSemaphore> handle;
				VkSemaphoreCreateInfo setup;
			};

			struct surface
			{
				surface();

				void create(GLFWwindow *w, const nv::vulkan::instance &i,
				            const nv::vulkan::physical_device &d, const uint32_t index);

				uint32_t image_count() const;

				void destroy(const nv::vulkan::instance &i);

				~surface();

				VkSurfaceKHR handle;
				uint32_t mode_index;
				uint32_t image_index;
				VkExtent2D resolution;
				std::vector<VkPresentModeKHR> mode;
				std::vector<VkSurfaceFormatKHR> image;
				VkSurfaceCapabilitiesKHR capabilities;
			};

			struct swapchain
			{
				swapchain();

				void create(const nv::vulkan::device &d, const nv::vulkan::surface &s);

				void destroy(const nv::vulkan::device &d);

				~swapchain();

				VkSwapchainKHR handle;
				VkSwapchainCreateInfoKHR setup;
				VkAttachmentReference reference;
				VkAttachmentDescription attachment;
			};

			struct image
			{
				image();

				void create(const nv::vulkan::device &d, const nv::vulkan::swapchain &c);

				void destroy(const nv::vulkan::device &d);

				uint32_t size() const;

				~image();

				VkExtent2D resolution;
				std::vector<VkImage> handle;
				std::vector<VkImageView> view;
				VkImageViewCreateInfo setup;
			};

			struct shader_module
			{
				shader_module();

				void load(const std::string &filename);

				void for_vertex_stage();

				void for_fragment_stage();

				void for_compute_stage();

				~shader_module();

				VkShaderModule handle;
				std::vector<char> spirv;
				VkShaderStageFlagBits usage;
				VkShaderModuleCreateInfo setup;
				VkPipelineLayoutCreateInfo info;
				VkSpecializationInfo constants;
			};

			struct render_pass
			{
				render_pass();

				void create(const nv::vulkan::device &d, const nv::vulkan::swapchain &c);

				void destroy(const nv::vulkan::device &d);

				~render_pass();

				VkRenderPass handle;
				VkSubpassDescription info;
				VkRenderPassCreateInfo setup;
			};

			struct framebuffer
			{
				framebuffer();

				void create(const nv::vulkan::device &d,
				            const nv::vulkan::image &i, const nv::vulkan::render_pass &p);

				uint32_t size() const;

				void destroy(const nv::vulkan::device &d);

				~framebuffer();

				std::vector<VkFramebuffer> handle;
				VkFramebufferCreateInfo setup;
			};

			struct viewport
			{
				viewport();

				~viewport();

				VkViewport handle;
				VkRect2D scissor;
				VkPipelineViewportStateCreateInfo setup;
			};

			struct rasterizer
			{
				rasterizer();

				~rasterizer();

				VkPipelineRasterizationStateCreateInfo setup;
			};

			struct multisample
			{
				multisample();

				~multisample();

				VkPipelineMultisampleStateCreateInfo setup;
			};

			struct color_blend
			{
				color_blend();

				~color_blend();

				VkPipelineColorBlendStateCreateInfo setup;
				VkPipelineColorBlendAttachmentState attachment;
			};

			struct depth_stencil
			{
				depth_stencil();

				~depth_stencil();

				VkPipelineDepthStencilStateCreateInfo setup;
			};

			struct layout
			{
				layout();

				void create(const nv::vulkan::device &d);

				void destroy(const nv::vulkan::device &d);

				~layout();

				VkPipelineLayout handle;
				VkPipelineLayoutCreateInfo setup;
			};

			struct pipeline
			{
				pipeline();

				void add(const nv::vulkan::shader_module &s);

				void add(const nv::vulkan::render_pass &p);

				void add(const nv::vulkan::viewport &v);

				void add(const nv::vulkan::rasterizer &r);

				void add(const nv::vulkan::multisample &m);

				void add(const nv::vulkan::color_blend &c);

				void add(const nv::vulkan::depth_stencil &s);

				void add(const nv::vulkan::layout &l);

				void create(const nv::vulkan::device &d);

				void destroy(const nv::vulkan::device &d);

				~pipeline();

				VkPipeline handle;
				VkPipelineCache cache;
				VkGraphicsPipelineCreateInfo setup;
				VkPipelineVertexInputStateCreateInfo input;
				VkPipelineInputAssemblyStateCreateInfo assembly;
				std::vector<VkPipelineShaderStageCreateInfo> stage;
			};
		}
	}
#endif
