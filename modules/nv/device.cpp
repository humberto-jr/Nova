#include "debug.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "device.hpp"

static nv::vulkan::instance vk;
static nv::vulkan::physical_device list;

void nv::device::startup()
{
	vk.create();
	list.enumerate(vk);
}

void nv::device::shutdown()
{
	vk.destroy();
}

uint32_t nv::device::version()
{
	return vk.info.apiVersion;
}

uint32_t nv::device::count()
{
	return list.handle.size();
}

std::string nv::device::name(const uint32_t n)
{
	return std::string(list.properties[n].deviceName);
}

bool nv::device::is_integrated(const uint32_t n)
{
	return (list.properties[n].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
}

bool nv::device::is_dedicated(const uint32_t n)
{
	return (list.properties[n].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
}

bool nv::device::is_virtual(const uint32_t n)
{
	return (list.properties[n].deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
}

bool nv::device::is_cpu(const uint32_t n)
{
	return (list.properties[n].deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU);
}

uint32_t nv::device::layer_count()
{
	return vk.setup.enabledLayerCount;
}

std::string nv::device::layer_name(const uint32_t n)
{
	return std::string(vk.setup.ppEnabledLayerNames[n]);
}

uint32_t nv::device::extension_count()
{
	return vk.setup.enabledExtensionCount;
}

//
// nv::device
//

nv::device::device(const uint32_t d, const uint32_t f):
	index(d)
{
	ASSERT(vk.handle != nullptr)

	this->interface.info.queueFamilyIndex = f;

	this->interface.create(list, d);
	this->pool.create(this->interface);
	this->buffer.allocate(this->interface, this->pool);
}

uint32_t nv::device::family_count() const
{
	return this->interface.family.size();
}

uint32_t nv::device::queue_count(const uint32_t f) const
{
	return this->interface.family[f].queueCount;
}

bool nv::device::support_graphics(const uint32_t f) const
{
	return (this->interface.family[f].queueFlags & VK_QUEUE_GRAPHICS_BIT);
}

bool nv::device::support_computation(const uint32_t f) const
{
	return (this->interface.family[f].queueFlags & VK_QUEUE_COMPUTE_BIT);
}

bool nv::device::support_transfer(const uint32_t f) const
{
	return (this->interface.family[f].queueFlags & VK_QUEUE_TRANSFER_BIT);
}

bool nv::device::support_all(const uint32_t f) const
{
	return (this->interface.family[f].queueFlags & VK_QUEUE_GRAPHICS_BIT)
	    && (this->interface.family[f].queueFlags & VK_QUEUE_COMPUTE_BIT)
	    && (this->interface.family[f].queueFlags & VK_QUEUE_TRANSFER_BIT);
}

void nv::device::attach_surface(nv::window &w) const
{
	if (w.is_closed()) return;

	w.surface.create(w.handle, vk, list, this->index);

	// Query of KHR support by the physical device's family queue:

	VkBool32 supported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(list.handle[index], this->interface.info.queueFamilyIndex, w.surface.handle, &supported);

	if (!supported)
	{
		PRINT_ERROR("error: queue family %d has no KHR support for this surface\n", (int) this->interface.info.queueFamilyIndex)
		exit(EXIT_FAILURE);
	}
}

void nv::device::detach_surface(nv::window &w) const
{
	if (w.is_closed()) return;

	if (w.surface.handle != nullptr)
		w.surface.destroy(vk);
}

void nv::device::renderer_startup(nv::renderer &r, const nv::window &w) const
{
	if (w.is_closed())
	{
		PRINT_ERROR("%s\n", "warning: an attempt to start a renderer for a closed window")
		return;
	}

	r.chain.create(this->interface, w.surface);
	r.image.create(this->interface, r.chain);
	r.pass.create(this->interface, r.chain);
	r.frame.create(this->interface, r.image, r.pass);
	r.signal.create(this->interface);
	r.signal.create(this->interface);
}

void nv::device::renderer_shutdown(nv::renderer &r) const
{
	r.chain.destroy(this->interface);
	r.image.destroy(this->interface);
	r.pass.destroy(this->interface);
	r.frame.destroy(this->interface);
	r.signal.destroy(this->interface);
}

nv::device::~device()
{
	this->pool.destroy(this->interface);
	this->interface.destroy();
}
