#include <atomic>

#include "debug.hpp"
#include "window.hpp"

// NOTE: counter keeps track of how many instances corresponds to opened windows.
static std::atomic<uint32_t> counter(0);

nv::window::window(const uint32_t width, const uint32_t height):
	handle(nullptr)
{
	if (!glfwInit())
	{
		glfwSwapInterval(1);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	this->handle = glfwCreateWindow((int) width, (int) height, "", nullptr, nullptr);

	if (this->handle == nullptr)
	{
		PRINT_ERROR("%s\n", "unable to allocate resources for the window")
		return;
	}

	++counter;
}

nv::window::window():
	window(1, 1)
{
	this->set_full_screen();
}

void nv::window::close()
{
	if (this->handle == nullptr) return;

	glfwDestroyWindow(this->handle);
	this->handle = nullptr;

	--counter;
}

bool nv::window::is_closed() const
{
	return (this->handle == nullptr);
}

bool nv::window::is_opened() const
{
	return (this->handle != nullptr);
}

void nv::window::set_title(const std::string &title)
{
	if (this->handle != nullptr)
		glfwSetWindowTitle(this->handle, title.std::string::c_str());
}

void nv::window::resize(const uint32_t width, const uint32_t height)
{
	if (this->handle != nullptr)
		glfwSetWindowSize(this->handle, width, height);
}

void nv::window::set_full_screen()
{
	if (this->handle == nullptr) return;

	int width, height;
	glfwGetWindowSize(this->handle, &width, &height);

	//	NOTE: x = -1 and y = 1 are the coordinates of the upper-left corner of the screen.
	glfwSetWindowMonitor(this->handle, glfwGetPrimaryMonitor(), -1, 1, width, height, GLFW_DONT_CARE);
}

double nv::window::time()
{
	return glfwGetTime();
}

void nv::window::resolution(uint32_t &width, uint32_t &height) const
{
	if (this->handle == nullptr) return;

	int w, h;
	glfwGetFramebufferSize(this->handle, &w, &h);

	width = static_cast<uint32_t>(w);
	height = static_cast<uint32_t>(h);
}

float nv::window::aspect_ratio()
{
	if (this->handle == nullptr) return 0.0f;

	uint32_t width = 0;
	uint32_t height = 0;
	this->resolution(width, height);

	return static_cast<float>(width/height);
}

void nv::window::refresh()
{
	if (this->handle != nullptr)
		glfwSwapBuffers(this->handle);
}

void nv::window::update_inputs()
{
	if (this->handle != nullptr)
		glfwPollEvents();
}

bool nv::window::should_close()
{
	if (this->handle != nullptr)
		return (glfwWindowShouldClose(this->handle) != 0);
	else
		return false;
}

bool nv::window::key_pressed(const uint32_t key)
{
	if (this->handle != nullptr)
		return (glfwGetKey(this->handle, key) == GLFW_PRESS);
	else
		return false;
}

bool nv::window::key_released(const uint32_t key)
{
	if (this->handle != nullptr)
		return (glfwGetKey(this->handle, key) == GLFW_RELEASE);
	else
		return false;
}

uint32_t nv::window::count()
{
	return counter;
}

uint32_t nv::window::mode_count()
{
	return this->surface.mode.size();
}

uint32_t nv::window::format_count()
{
	return this->surface.image.size();
}

uint32_t nv::window::image_count() const
{
	return this->surface.image_count();
}

bool nv::window::support_srgb()
{
	for (uint32_t n = 0; n < this->surface.image.size(); ++n)
		if (this->surface.image[n].format == VK_FORMAT_B8G8R8A8_SRGB
		 && this->surface.image[n].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) return true;

	return false;
}

bool nv::window::support_double_buffer()
{
	for (uint32_t n = 0; n < this->surface.mode.size(); ++n)
		if (this->surface.mode[n] == VK_PRESENT_MODE_FIFO_KHR) return true;

	return false;
}

bool nv::window::support_triple_buffer()
{
	for (uint32_t n = 0; n < this->surface.mode.size(); ++n)
		if (this->surface.mode[n] == VK_PRESENT_MODE_MAILBOX_KHR) return true;

	return false;
}

void nv::window::use_srgb()
{
	for (uint32_t n = 0; n < this->surface.image.size(); ++n)
		if (this->surface.image[n].format == VK_FORMAT_B8G8R8A8_SRGB
		 && this->surface.image[n].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) this->surface.image_index = n;
}

void nv::window::use_double_buffer()
{
	for (uint32_t n = 0; n < this->surface.mode.size(); ++n)
		if (this->surface.mode[n] == VK_PRESENT_MODE_FIFO_KHR) this->surface.mode_index = n;
}

void nv::window::use_triple_buffer()
{
	for (uint32_t n = 0; n < this->surface.mode.size(); ++n)
		if (this->surface.mode[n] == VK_PRESENT_MODE_MAILBOX_KHR) this->surface.mode_index = n;
}

nv::window::~window()
{
	if (this->handle != nullptr)
		this->close();
}
