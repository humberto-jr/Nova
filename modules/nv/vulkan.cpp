#include <fstream>

#include "debug.hpp"
#include "vulkan.hpp"

#define NV_VULKAN_ERROR(name, code)                                 \
{                                                                   \
  if (code != VK_SUCCESS)                                           \
  {                                                                 \
	 PRINT_ERROR("%s failed with error code %d\n", name, (int) code) \
	 exit(EXIT_FAILURE);                                             \
  }                                                                 \
}

static const char* const layers[] =
{
	"VK_LAYER_LUNARG_standard_validation",
	"VK_LAYER_LUNARG_parameter_validation",
	"VK_LAYER_LUNARG_object_tracker",
	"VK_LAYER_LUNARG_core_validation"
};

static const char* const extension_name[] = {"VK_KHR_swapchain"};

//
// nv::vulkan::instance
//

nv::vulkan::instance::instance():
	handle(nullptr)
{
	vkEnumerateInstanceVersion(&this->info.apiVersion);

	this->info.pNext = nullptr;
	this->info.engineVersion = 0;
	this->info.pEngineName = "NOVA";
	this->info.pApplicationName = "";
	this->info.applicationVersion = 0;
	this->info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	if (!glfwInit())
	{
		glfwSwapInterval(1);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	this->setup.ppEnabledExtensionNames
		= glfwGetRequiredInstanceExtensions(&this->setup.enabledExtensionCount);

	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.enabledLayerCount = 4; // FIXME: using four debug layers but also causes a segfault at termination.
	this->setup.ppEnabledLayerNames = layers;
	this->setup.pApplicationInfo = &this->info;
	this->setup.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
}

void nv::vulkan::instance::create()
{
	const VkResult error = vkCreateInstance(&this->setup, nullptr, &this->handle);
	NV_VULKAN_ERROR("vkCreateInstance()", error)
}

void nv::vulkan::instance::destroy()
{
	if (this->handle == nullptr) return;

	vkDestroyInstance(this->handle, nullptr);
	this->handle = nullptr;
}

nv::vulkan::instance::~instance()
{
	this->destroy();
}

//
// nv::vulkan::physical_device
//

nv::vulkan::physical_device::physical_device()
{
}

void nv::vulkan::physical_device::enumerate(const nv::vulkan::instance &i)
{
	ASSERT(i.handle != nullptr)

	VkResult error;
	uint32_t counter = 0;

	error = vkEnumeratePhysicalDevices(i.handle, &counter, nullptr);
	NV_VULKAN_ERROR("vkEnumeratePhysicalDevices()", error)

	this->handle.resize(counter);

	error = vkEnumeratePhysicalDevices(i.handle, &counter, this->handle.data());
	NV_VULKAN_ERROR("vkEnumeratePhysicalDevices()", error)

	this->properties.resize(counter);
	this->memory.resize(counter);

	for (uint32_t n = 0; n < counter; ++n)
	{
		vkGetPhysicalDeviceProperties(this->handle[n], &this->properties[n]);
		vkGetPhysicalDeviceMemoryProperties(this->handle[n], &this->memory[n]);
	}
}

uint32_t nv::vulkan::physical_device::count() const
{
	return this->handle.size();
}

nv::vulkan::physical_device::~physical_device()
{
	this->handle.clear();
	this->properties.clear();
	this->memory.clear();
}

//
// nv::vulkan::device
//

nv::vulkan::device::device():
	handle(nullptr)
{
	this->priority.push_back(1.0f);

	this->info.flags = 0;
	this->info.pNext = nullptr;
	this->info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

	// NOTE: using the 1st queue family and its 1st queue as default.
	this->info.queueCount = 1;
	this->info.queueFamilyIndex = 0;
	this->info.pQueuePriorities = this->priority.data();

	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.enabledLayerCount = 0;
	this->setup.queueCreateInfoCount = 1;
	this->setup.enabledExtensionCount = 1;
	this->setup.pEnabledFeatures = nullptr;
	this->setup.ppEnabledLayerNames = nullptr;
	this->setup.pQueueCreateInfos = &this->info;
	this->setup.ppEnabledExtensionNames = extension_name;
	this->setup.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
}

void nv::vulkan::device::create(const nv::vulkan::physical_device &d, const uint32_t index)
{
	ASSERT(index < d.count())
	ASSERT(d.handle[index] != nullptr)

	// FIXME: Vulkan warning: family index is not less than counter (?).
	VkResult error;
	uint32_t counter = 0;

	error = vkCreateDevice(d.handle[index], &this->setup, nullptr, &this->handle);
	NV_VULKAN_ERROR("vkCreateDevice()", error)

	vkGetPhysicalDeviceQueueFamilyProperties(d.handle[index], &counter, nullptr);

	this->family.resize(counter);

	vkGetPhysicalDeviceQueueFamilyProperties(d.handle[index], &counter, this->family.data());
}

void nv::vulkan::device::destroy()
{
	if (this->handle == nullptr) return;

	this->priority.clear();
	vkDestroyDevice(this->handle, nullptr);
	this->handle = nullptr;
}

nv::vulkan::device::~device()
{
	this->destroy();
}

//
// nv::vulkan::command_pool
//

nv::vulkan::command_pool::command_pool():
	handle(nullptr)
{
	this->setup.pNext = nullptr;
	this->setup.queueFamilyIndex = 0;
	this->setup.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	this->setup.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
}

void nv::vulkan::command_pool::create(const nv::vulkan::device &d)
{
	ASSERT(d.handle != nullptr)

	this->setup.queueFamilyIndex = d.info.queueFamilyIndex;

	VkResult error = vkCreateCommandPool(d.handle, &this->setup, nullptr, &this->handle);
	NV_VULKAN_ERROR("vkCreateCommandPool()", error)

	// TODO: as for now only the 1st queue (= 0) is used.
	vkGetDeviceQueue(d.handle, this->setup.queueFamilyIndex, 0, &this->queue);
	NV_VULKAN_ERROR("vkGetDeviceQueue()", error)
}

void nv::vulkan::command_pool::destroy(const nv::vulkan::device &d)
{
	if (this->handle == nullptr) return;

	ASSERT(d.handle != nullptr)

	// NOTE: this->queue will be destroyed by Vulkan alongside with d.handle.
	vkDestroyCommandPool(d.handle, this->handle, nullptr);
	this->handle = nullptr;
}

nv::vulkan::command_pool::~command_pool()
{
	if (this->handle != nullptr)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::vulkan::command_pool instance before calling nv::vulkan::command_pool::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::command_buffer
//

nv::vulkan::command_buffer::command_buffer():
	handle(nullptr)
{
	this->setup.pNext = nullptr;
	this->setup.commandPool = nullptr;
	this->setup.commandBufferCount = 0;
	this->setup.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	this->setup.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
}

void nv::vulkan::command_buffer::allocate(const nv::vulkan::device &d,
                                           nv::vulkan::command_pool &p)
{
	ASSERT(d.handle != nullptr)
	ASSERT(p.handle != nullptr)

	this->setup.commandBufferCount = 1;
	this->setup.commandPool = p.handle;

	const VkResult error = vkAllocateCommandBuffers(d.handle, &this->setup, &this->handle);
	NV_VULKAN_ERROR("vkAllocateCommandBuffers()", error)
}

nv::vulkan::command_buffer::~command_buffer()
{
}

//
// nv::vulkan::semaphore
//

nv::vulkan::semaphore::semaphore()
{
	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
}

void nv::vulkan::semaphore::create(const nv::vulkan::device &d)
{
	ASSERT(d.handle != nullptr)

	VkSemaphore s;

	const VkResult error = vkCreateSemaphore(d.handle, &this->setup, nullptr, &s);
	NV_VULKAN_ERROR("vkCreateSemaphore()", error)

	this->handle.push_back(s);
}

void nv::vulkan::semaphore::destroy(const nv::vulkan::device &d)
{
	ASSERT(d.handle != nullptr)

	for (uint32_t n = 0; n < this->handle.size(); ++n)
		vkDestroySemaphore(d.handle, this->handle[n], nullptr);

	this->handle.clear();
}

uint32_t nv::vulkan::semaphore::size() const
{
	return this->handle.size();
}

nv::vulkan::semaphore::~semaphore()
{
	if (this->handle.size() > 0)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a list of nv::vulkan::semaphore instances before calling nv::vulkan::semaphore::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::surface
//

nv::vulkan::surface::surface():
	handle(nullptr),
	mode_index(0),
	image_index(0)
{
}

void nv::vulkan::surface::create(GLFWwindow *w, const nv::vulkan::instance &i,
                                  const nv::vulkan::physical_device &d, const uint32_t index)
{
	ASSERT(w != nullptr)

	VkResult error;
	uint32_t counter = 0;
	int width = 0, height = 0;

	// Update the resolution:

	glfwGetFramebufferSize(w, &width, &height);

	this->resolution.width = static_cast<uint32_t>(width);
	this->resolution.height = static_cast<uint32_t>(height);

	// Attach the surface:

	ASSERT(i.handle != nullptr)

	error = glfwCreateWindowSurface(i.handle, w, nullptr, &this->handle);
	NV_VULKAN_ERROR("glfwCreateWindowSurface()", error)

	//	Query of KHR capabilities available:

	ASSERT(index < d.count())
	ASSERT(d.handle[index] != nullptr)

	error = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(d.handle[index], this->handle, &this->capabilities);
	NV_VULKAN_ERROR("vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", error)

	// Query of image formats available:

	error = vkGetPhysicalDeviceSurfaceFormatsKHR(d.handle[index], this->handle, &counter, nullptr);
	NV_VULKAN_ERROR("vkGetPhysicalDeviceSurfaceFormatsKHR()", error)

	this->image.resize(counter);

	error = vkGetPhysicalDeviceSurfaceFormatsKHR(d.handle[index], this->handle, &counter, this->image.data());
	NV_VULKAN_ERROR("vkGetPhysicalDeviceSurfaceFormatsKHR()", error)

	// Query of presentation modes available:

	error = vkGetPhysicalDeviceSurfacePresentModesKHR(d.handle[index], this->handle, &counter, nullptr);
	NV_VULKAN_ERROR("vkGetPhysicalDeviceSurfacePresentModesKHR()", error)

	this->mode.resize(counter);

	error = vkGetPhysicalDeviceSurfacePresentModesKHR(d.handle[index], this->handle, &counter, this->mode.data());
	NV_VULKAN_ERROR("vkGetPhysicalDeviceSurfacePresentModesKHR()", error)
}

uint32_t nv::vulkan::surface::image_count() const
{
	// NOTE: this->capabilities.maxImageCount == 0 is a special case that represents "as many as needed".
	if ((this->capabilities.maxImageCount > 0) && ((this->capabilities.minImageCount + 1) > this->capabilities.maxImageCount))
		return this->capabilities.maxImageCount;
	else
		return this->capabilities.minImageCount + 1;
}

void nv::vulkan::surface::destroy(const nv::vulkan::instance &i)
{
	if (this->handle == nullptr) return;

	vkDestroySurfaceKHR(i.handle, this->handle, nullptr);
	this->handle = nullptr;
	this->image.clear();
	this->mode.clear();
}

nv::vulkan::surface::~surface()
{
	if (this->handle != nullptr)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::vulkan::surface instance before calling nv::vulkan::surface::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::swapchain
//

nv::vulkan::swapchain::swapchain():
	handle(nullptr)
{
	this->setup.pNext = nullptr;
	this->setup.clipped = VK_TRUE;
	this->setup.imageArrayLayers = 1;
	this->setup.queueFamilyIndexCount = 0;
	this->setup.pQueueFamilyIndices = nullptr;
	this->setup.oldSwapchain = VK_NULL_HANDLE;
	this->setup.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	this->setup.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	this->setup.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	this->setup.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	this->setup.flags = VK_SWAPCHAIN_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR;

	// NOTE: the remaining surface-dependent setup entries are filled during the
	// swapchain creation.

	this->attachment.flags = 0;
	this->attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	this->attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	this->attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	this->attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	this->attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	this->attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	this->attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	// NOTE: a proper choice of image format will be made by a given surface
	// during the swapchain creation. It is left undefined until then.

	this->attachment.format = VK_FORMAT_UNDEFINED;

	this->reference.attachment = 0;
	this->reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void nv::vulkan::swapchain::create(const nv::vulkan::device &d,
                                   const nv::vulkan::surface &s)
{
	ASSERT(d.handle != nullptr)
	ASSERT(s.handle != nullptr)

	VkResult error;
	const auto m = s.mode_index;
	const auto i = s.image_index;

	this->setup.surface = s.handle;
	this->setup.presentMode = s.mode[m];
	this->setup.imageExtent = s.resolution;
	this->setup.minImageCount = s.image_count();
	this->setup.imageFormat = s.image[i].format;
	this->setup.imageColorSpace = s.image[i].colorSpace;
	this->setup.preTransform = s.capabilities.currentTransform;

	error = vkCreateSwapchainKHR(d.handle, &this->setup, nullptr, &this->handle);
	NV_VULKAN_ERROR("vkCreateSwapchainKHR()", error)

	this->attachment.format = this->setup.imageFormat;
}

void nv::vulkan::swapchain::destroy(const nv::vulkan::device &d)
{
	if (this->handle == nullptr) return;

	ASSERT(d.handle != nullptr)

	vkDestroySwapchainKHR(d.handle, this->handle, nullptr);
	this->handle = nullptr;
}

nv::vulkan::swapchain::~swapchain()
{
	if (this->handle != nullptr)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::vulkan::swapchain instance before calling nv::vulkan::swapchain::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::image
//

nv::vulkan::image::image()
{
	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.viewType = VK_IMAGE_VIEW_TYPE_2D;
	this->setup.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	this->setup.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->setup.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->setup.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	this->setup.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	this->setup.subresourceRange.levelCount = 1;
	this->setup.subresourceRange.layerCount = 1;
	this->setup.subresourceRange.baseMipLevel = 0;
	this->setup.subresourceRange.baseArrayLayer = 0;
	this->setup.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
}

void nv::vulkan::image::create(const nv::vulkan::device &d,
                               const nv::vulkan::swapchain &c)
{
	ASSERT(d.handle != nullptr)
	ASSERT(c.handle != nullptr)

	VkResult error;
	uint32_t counter = 0;

	error = vkGetSwapchainImagesKHR(d.handle, c.handle, &counter, nullptr);
	NV_VULKAN_ERROR("vkGetSwapchainImagesKHR()", error)

	this->handle.resize(counter);

	error = vkGetSwapchainImagesKHR(d.handle, c.handle, &counter, this->handle.data());
	NV_VULKAN_ERROR("vkGetSwapchainImagesKHR()", error)

	this->view.resize(counter);
	this->setup.format = c.setup.imageFormat;

	for (uint32_t n = 0; n < counter; ++n)
	{
		this->setup.image = this->handle[n];

		error = vkCreateImageView(d.handle, &this->setup, nullptr, &this->view[n]);
		NV_VULKAN_ERROR("vkCreateImageView()", error)
	}

	this->resolution = c.setup.imageExtent;
}

void nv::vulkan::image::destroy(const nv::vulkan::device &d)
{
	ASSERT(d.handle != nullptr)

	for (uint32_t n = 0; n < this->view.size(); ++n)
		vkDestroyImageView(d.handle, this->view[n], nullptr);

	this->view.clear();
	this->handle.clear();
}

uint32_t nv::vulkan::image::size() const
{
	return this->view.size();
}

nv::vulkan::image::~image()
{
	if (this->handle.size() > 0)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a list of nv::vulkan::image instances before calling nv::vulkan::image::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::shader_module
//

nv::vulkan::shader_module::shader_module()
{
	this->setup.flags = 0;
	this->setup.codeSize = 0;
	this->setup.pNext = nullptr;
	this->setup.pCode = nullptr;
	this->setup.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

	this->info.flags = 0;
	this->info.pNext = nullptr;
	this->info.setLayoutCount = 0;
	this->info.pSetLayouts = nullptr;
	this->info.pushConstantRangeCount = 0;
	this->info.pPushConstantRanges = nullptr;
	this->info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	this->usage = VK_SHADER_STAGE_ALL_GRAPHICS;

	this->constants.dataSize = 0;
	this->constants.pData = nullptr;
	this->constants.mapEntryCount = 0;
	this->constants.pMapEntries = nullptr;
}

void nv::vulkan::shader_module::load(const std::string &filename)
{
	std::ifstream input(filename, std::ios::ate | std::ios::binary);

	if (!input.is_open())
	{
		PRINT_ERROR("unable to open %s\n", filename.c_str())
		return;
	}

	this->setup.codeSize = static_cast<size_t>(input.tellg());

	ASSERT(this->setup.codeSize > 0)

	this->spirv.resize(this->setup.codeSize);

	input.seekg(0);

	input.read(this->spirv.data(), this->setup.codeSize);

	input.close();

	this->setup.pCode = reinterpret_cast<const uint32_t*>(this->spirv.data());
}

void nv::vulkan::shader_module::for_vertex_stage()
{
	this->usage = VK_SHADER_STAGE_VERTEX_BIT;
}

void nv::vulkan::shader_module::for_fragment_stage()
{
	this->usage = VK_SHADER_STAGE_FRAGMENT_BIT;
}

void nv::vulkan::shader_module::for_compute_stage()
{
	this->usage = VK_SHADER_STAGE_COMPUTE_BIT;
}

nv::vulkan::shader_module::~shader_module()
{
}

//
// nv::vulkan::render_pass
//

nv::vulkan::render_pass::render_pass()
{
	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.subpassCount = 1;
	this->setup.dependencyCount = 0;
	this->setup.pDependencies = nullptr;
	this->setup.pSubpasses = &this->info;
	this->setup.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	// NOTE: a proper choice of attachment will be made by a given swapchain
	// during the render pass creation.

	this->setup.attachmentCount = 0;
	this->setup.pAttachments = nullptr;

	this->info.flags = 0;
	this->info.inputAttachmentCount = 0;
	this->info.preserveAttachmentCount = 0;
	this->info.pInputAttachments = nullptr;
	this->info.pResolveAttachments = nullptr;
	this->info.pPreserveAttachments = nullptr;
	this->info.pDepthStencilAttachment = nullptr;
	this->info.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	// NOTE: a proper choice of attachment reference will be made by a given
	// swapchain during the render pass creation.

	this->info.colorAttachmentCount = 0;
	this->info.pColorAttachments = nullptr;
}

void nv::vulkan::render_pass::create(const nv::vulkan::device &d,
                                     const nv::vulkan::swapchain &c)
{
	ASSERT(d.handle != nullptr)
	ASSERT(c.handle != nullptr)

	this->setup.attachmentCount = 1;
	this->setup.pAttachments = &c.attachment;

	this->info.colorAttachmentCount = 1;
	this->info.pColorAttachments = &c.reference;

	VkResult error = vkCreateRenderPass(d.handle, &this->setup, nullptr, &this->handle);
	NV_VULKAN_ERROR("vkCreateRenderPass()", error)
}

void nv::vulkan::render_pass::destroy(const nv::vulkan::device &d)
{
	if (this->handle == nullptr) return;

	ASSERT(d.handle != nullptr)

	vkDestroyRenderPass(d.handle, this->handle, nullptr);
	this->handle = nullptr;
}

nv::vulkan::render_pass::~render_pass()
{
	if (this->handle != nullptr)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::vulkan::render_pass instance before calling nv::vulkan::render_pass::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::framebuffer
//

nv::vulkan::framebuffer::framebuffer()
{
	this->setup.flags = 0;
	this->setup.layers = 1;
	this->setup.pNext = nullptr;
	this->setup.attachmentCount = 1;
	this->setup.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

	// NOTE: the remaining image- and render pass-dependent setup entries are
	// filled during the framebuffer creation.
}

void nv::vulkan::framebuffer::create(const nv::vulkan::device &d,
                                     const nv::vulkan::image &i,
                                     const nv::vulkan::render_pass &p)
{
	ASSERT(d.handle != nullptr)
	ASSERT(p.handle != nullptr)

	const uint32_t view_count = i.view.size();

	ASSERT(view_count > 0)

	this->handle.resize(view_count);

	this->setup.renderPass = p.handle;
	this->setup.width = i.resolution.width;
	this->setup.height = i.resolution.height;

	for (uint32_t n = 0; n < view_count; ++n)
	{
		this->setup.pAttachments = &i.view[n];

		ASSERT(this->setup.pAttachments != nullptr)

		VkResult error = vkCreateFramebuffer(d.handle, &this->setup, nullptr, &this->handle[n]);
		NV_VULKAN_ERROR("vkCreateFramebuffer()", error)
	}
}

uint32_t nv::vulkan::framebuffer::size() const
{
	return this->handle.size();
}

void nv::vulkan::framebuffer::destroy(const nv::vulkan::device &d)
{
	ASSERT(d.handle != nullptr)

	for (uint32_t n = 0; n < this->handle.size(); ++n)
		vkDestroyFramebuffer(d.handle, this->handle[n], nullptr);

	this->handle.clear();
}

nv::vulkan::framebuffer::~framebuffer()
{
	if (this->handle.size() > 0)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a list of nv::vulkan::framebuffer instances before calling nv::vulkan::framebuffer::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::viewport
//

nv::vulkan::viewport::viewport()
{
	this->handle.x = 0.0f;
	this->handle.y = 0.0f;
	this->handle.width = 0.0f;
	this->handle.height = 0.0f;
	this->handle.minDepth = 0.0f;
	this->handle.maxDepth = 1.0f;

	this->scissor.offset.x = 0;
	this->scissor.offset.y = 0;
	this->scissor.extent.width = 0;
	this->scissor.extent.height = 0;

	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.scissorCount = 1;
	this->setup.viewportCount = 1;
	this->setup.pScissors = &this->scissor;
	this->setup.pViewports = &this->handle;
	this->setup.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
}

nv::vulkan::viewport::~viewport()
{
}

//
// nv::vulkan::rasterizer
//

nv::vulkan::rasterizer::rasterizer()
{
	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.lineWidth = 1.0f;
	this->setup.depthBiasClamp = 0.0f;
	this->setup.depthBiasEnable = VK_FALSE;
	this->setup.depthBiasSlopeFactor = 0.0f;
	this->setup.depthClampEnable = VK_FALSE;
	this->setup.depthBiasConstantFactor = 0.0f;
	this->setup.cullMode = VK_CULL_MODE_BACK_BIT;
	this->setup.rasterizerDiscardEnable = VK_FALSE;
	this->setup.polygonMode = VK_POLYGON_MODE_FILL;
	this->setup.frontFace = VK_FRONT_FACE_CLOCKWISE;
	this->setup.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
}

nv::vulkan::rasterizer::~rasterizer()
{
}

//
// nv::vulkan::multisample
//

nv::vulkan::multisample::multisample()
{
	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.pSampleMask = nullptr;
	this->setup.minSampleShading = 1.0f;
	this->setup.alphaToOneEnable = VK_FALSE;
	this->setup.sampleShadingEnable = VK_FALSE;
	this->setup.alphaToCoverageEnable = VK_FALSE;
	this->setup.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	this->setup.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
}

nv::vulkan::multisample::~multisample()
{
}

//
// nv::vulkan::color_blend
//

nv::vulkan::color_blend::color_blend()
{
	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.attachmentCount = 1;
	this->setup.blendConstants[0] = 0.0f;
	this->setup.blendConstants[1] = 0.0f;
	this->setup.blendConstants[2] = 0.0f;
	this->setup.blendConstants[3] = 0.0f;
	this->setup.logicOpEnable = VK_FALSE;
	this->setup.logicOp = VK_LOGIC_OP_COPY;
	this->setup.pAttachments = &this->attachment;
	this->setup.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	this->attachment.blendEnable = VK_FALSE;
	this->attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	this->attachment.colorBlendOp = VK_BLEND_OP_ADD;
	this->attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	this->attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	this->attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	this->attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;

	this->attachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}

nv::vulkan::color_blend::~color_blend()
{
}

//
// nv::vulkan::depth_stencil
//

nv::vulkan::depth_stencil::depth_stencil()
{
	this->setup.flags = 0;
	this->setup.back = {};
	this->setup.front = {};
	this->setup.pNext = nullptr;
	this->setup.minDepthBounds = 0.0f;
	this->setup.maxDepthBounds = 1.0f;
	this->setup.depthTestEnable = VK_TRUE;
	this->setup.depthWriteEnable = VK_TRUE;
	this->setup.stencilTestEnable = VK_FALSE;
	this->setup.depthBoundsTestEnable = VK_FALSE;
	this->setup.depthCompareOp = VK_COMPARE_OP_LESS;
	this->setup.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
}

nv::vulkan::depth_stencil::~depth_stencil()
{
}

//
// nv::vulkan::layout
//

nv::vulkan::layout::layout()
{
	this->setup.flags = 0;
	this->setup.pNext = nullptr;
	this->setup.setLayoutCount = 0;
	this->setup.pSetLayouts = nullptr;
	this->setup.pushConstantRangeCount = 0;
	this->setup.pPushConstantRanges = nullptr;
	this->setup.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
}

void nv::vulkan::layout::create(const nv::vulkan::device &d)
{
	ASSERT(d.handle != nullptr)

	VkResult error = vkCreatePipelineLayout(d.handle, &this->setup, nullptr, &this->handle);
	NV_VULKAN_ERROR("vkCreatePipelineLayout()", error)
}

void nv::vulkan::layout::destroy(const nv::vulkan::device &d)
{
	if (this->handle == nullptr) return;

	ASSERT(d.handle != nullptr)

	vkDestroyPipelineLayout(d.handle, this->handle, nullptr);
	this->handle = nullptr;
}

nv::vulkan::layout::~layout()
{
	if (this->handle != nullptr)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::vulkan::layout instance before calling nv::vulkan::layout::destroy()")
		exit(EXIT_FAILURE);
	}
}

//
// nv::vulkan::pipeline
//

nv::vulkan::pipeline::pipeline():
	handle(nullptr),
	cache(nullptr)
{
	this->setup.flags = 0;
	this->setup.subpass = 0;
	this->setup.stageCount = 0;
	this->setup.pNext = nullptr;
	this->setup.layout = nullptr;
	this->setup.renderPass = nullptr;
	this->setup.basePipelineIndex = -1;
	this->setup.pDynamicState = nullptr;
	this->setup.pViewportState = nullptr;
	this->setup.pColorBlendState = nullptr;
	this->setup.pMultisampleState = nullptr;
	this->setup.pStages = this->stage.data();
	this->setup.pDepthStencilState = nullptr;
	this->setup.pRasterizationState = nullptr;
	this->setup.pVertexInputState = &this->input;
	this->setup.basePipelineHandle = VK_NULL_HANDLE;
	this->setup.pInputAssemblyState = &this->assembly;
	this->setup.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	this->input.flags = 0;
	this->input.pNext = nullptr;
	this->input.vertexBindingDescriptionCount = 0;
	this->input.pVertexBindingDescriptions = nullptr;
	this->input.vertexAttributeDescriptionCount = 0;
	this->input.pVertexAttributeDescriptions = nullptr;
	this->input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	this->assembly.flags = 0;
	this->assembly.pNext = nullptr;
	this->assembly.primitiveRestartEnable = VK_FALSE;
	this->assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	this->assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
}

void nv::vulkan::pipeline::add(const nv::vulkan::shader_module &s)
{
	VkPipelineShaderStageCreateInfo shader;

	shader.flags = 0;
	shader.pName = "main";
	shader.pNext = nullptr;
	shader.stage = s.usage;
	shader.module = s.handle;
	shader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	if (s.constants.mapEntryCount > 0)
		shader.pSpecializationInfo = &s.constants;
	else
		shader.pSpecializationInfo = nullptr;

	this->stage.push_back(shader);
	this->setup.stageCount += 1;
}

void nv::vulkan::pipeline::add(const nv::vulkan::render_pass &p)
{
	ASSERT(p.handle != nullptr)

	// TODO: using the 1st subpass only.
	this->setup.renderPass = p.handle;
	this->setup.subpass = 0;
}

void nv::vulkan::pipeline::add(const nv::vulkan::viewport &v)
{
	this->setup.pViewportState = &v.setup;
}

void nv::vulkan::pipeline::add(const nv::vulkan::rasterizer &r)
{
	this->setup.pRasterizationState = &r.setup;
}

void nv::vulkan::pipeline::add(const nv::vulkan::multisample &m)
{
	this->setup.pMultisampleState = &m.setup;
}

void nv::vulkan::pipeline::add(const nv::vulkan::color_blend &c)
{
	this->setup.pColorBlendState = &c.setup;
}

void nv::vulkan::pipeline::add(const nv::vulkan::depth_stencil &s)
{
	this->setup.pDepthStencilState = &s.setup;
}

void nv::vulkan::pipeline::add(const nv::vulkan::layout &l)
{
	this->setup.layout = l.handle;
}

void nv::vulkan::pipeline::create(const nv::vulkan::device &d)
{
	ASSERT(d.handle != nullptr)

	VkResult error = vkCreateGraphicsPipelines(d.handle, this->cache, 1, &this->setup, nullptr,  &this->handle);
	NV_VULKAN_ERROR("vkCreateGraphicsPipelines()", error)
}

void nv::vulkan::pipeline::destroy(const nv::vulkan::device &d)
{
	if (this->handle == nullptr) return;

	ASSERT(d.handle != nullptr)

	vkDestroyPipeline(d.handle, this->handle, nullptr);
	this->handle = nullptr;
}

nv::vulkan::pipeline::~pipeline()
{
	if (this->handle != nullptr)
	{
		PRINT_ERROR("%s\n", "error: end of scope for a nv::vulkan::pipeline instance before calling nv::vulkan::pipeline::destroy()")
		exit(EXIT_FAILURE);
	}
}
