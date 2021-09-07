#if !defined(NV_WINDOW_HEADER)
	#define NV_WINDOW_HEADER

	#include <GLFW/glfw3.h>
	#include <string>

	#include "vulkan.hpp"
	#include "device.hpp"

	namespace nv
	{
		class renderer;

		class window
		{
			public:
			window(const uint32_t width, const uint32_t height);

			window();

			void close();

			bool is_closed() const;

			bool is_opened() const;

			void set_title(const std::string &title);

			void resize(const uint32_t width, const uint32_t height);

			void set_full_screen();

			double time();

			void resolution(uint32_t &width, uint32_t &height) const;

			float aspect_ratio();

			void refresh();

			void update_inputs();

			bool should_close();

			bool key_pressed(const uint32_t key);

			bool key_released(const uint32_t key);

			uint32_t count();

			uint32_t mode_count();

			uint32_t format_count();

			uint32_t image_count() const;

			bool support_srgb();

			bool support_double_buffer();

			bool support_triple_buffer();

			void use_srgb();

			void use_double_buffer();

			void use_triple_buffer();

			~window();

			friend void nv::device::attach_surface(nv::window &w) const;
			friend void nv::device::detach_surface(nv::window &w) const;
			friend void nv::device::renderer_startup(nv::renderer &r, const nv::window &w) const;

			private:
			GLFWwindow *handle;
			nv::vulkan::surface surface;
		};
	}

	#define NV_WINDOW_KEY_UNKNOWN -1

	#define NV_WINDOW_KEY_MOUSE_BUTTON_1      0
	#define NV_WINDOW_KEY_MOUSE_BUTTON_2      1
	#define NV_WINDOW_KEY_MOUSE_BUTTON_3      2
	#define NV_WINDOW_KEY_MOUSE_BUTTON_4      3
	#define NV_WINDOW_KEY_MOUSE_BUTTON_5      4
	#define NV_WINDOW_KEY_MOUSE_BUTTON_6      5
	#define NV_WINDOW_KEY_MOUSE_BUTTON_7      6
	#define NV_WINDOW_KEY_MOUSE_BUTTON_8      7
	#define NV_WINDOW_KEY_MOUSE_BUTTON_LAST   WINDOW_KEY_MOUSE_BUTTON_8
	#define NV_WINDOW_KEY_MOUSE_BUTTON_LEFT   WINDOW_KEY_MOUSE_BUTTON_1
	#define NV_WINDOW_KEY_MOUSE_BUTTON_RIGHT  WINDOW_KEY_MOUSE_BUTTON_2
	#define NV_WINDOW_KEY_MOUSE_BUTTON_MIDDLE WINDOW_KEY_MOUSE_BUTTON_3

	#define NV_WINDOW_KEY_SPACE          32
	#define NV_WINDOW_KEY_APOSTROPHE     39 /* ' */
	#define NV_WINDOW_KEY_COMMA          44 /* , */
	#define NV_WINDOW_KEY_MINUS          45 /* - */
	#define NV_WINDOW_KEY_PERIOD         46 /* . */
	#define NV_WINDOW_KEY_SLASH          47 /* / */
	#define NV_WINDOW_KEY_0              48
	#define NV_WINDOW_KEY_1              49
	#define NV_WINDOW_KEY_2              50
	#define NV_WINDOW_KEY_3              51
	#define NV_WINDOW_KEY_4              52
	#define NV_WINDOW_KEY_5              53
	#define NV_WINDOW_KEY_6              54
	#define NV_WINDOW_KEY_7              55
	#define NV_WINDOW_KEY_8              56
	#define NV_WINDOW_KEY_9              57
	#define NV_WINDOW_KEY_SEMICOLON      59 /* ; */
	#define NV_WINDOW_KEY_EQUAL          61 /* = */
	#define NV_WINDOW_KEY_A              65
	#define NV_WINDOW_KEY_B              66
	#define NV_WINDOW_KEY_C              67
	#define NV_WINDOW_KEY_D              68
	#define NV_WINDOW_KEY_E              69
	#define NV_WINDOW_KEY_F              70
	#define NV_WINDOW_KEY_G              71
	#define NV_WINDOW_KEY_H              72
	#define NV_WINDOW_KEY_I              73
	#define NV_WINDOW_KEY_J              74
	#define NV_WINDOW_KEY_K              75
	#define NV_WINDOW_KEY_L              76
	#define NV_WINDOW_KEY_M              77
	#define NV_WINDOW_KEY_N              78
	#define NV_WINDOW_KEY_O              79
	#define NV_WINDOW_KEY_P              80
	#define NV_WINDOW_KEY_Q              81
	#define NV_WINDOW_KEY_R              82
	#define NV_WINDOW_KEY_S              83
	#define NV_WINDOW_KEY_T              84
	#define NV_WINDOW_KEY_U              85
	#define NV_WINDOW_KEY_V              86
	#define NV_WINDOW_KEY_W              87
	#define NV_WINDOW_KEY_X              88
	#define NV_WINDOW_KEY_Y              89
	#define NV_WINDOW_KEY_Z              90
	#define NV_WINDOW_KEY_LEFT_BRACKET   91 /* [ */
	#define NV_WINDOW_KEY_BACKSLASH      92 /* \ */
	#define NV_WINDOW_KEY_RIGHT_BRACKET  93 /* ] */
	#define NV_WINDOW_KEY_GRAVE_ACCENT   96 /* ` */
	#define NV_WINDOW_KEY_WORLD_1       161 /* non-US #1 */
	#define NV_WINDOW_KEY_WORLD_2       162 /* non-US #2 */
	#define NV_WINDOW_KEY_ESCAPE        256
	#define NV_WINDOW_KEY_ENTER         257
	#define NV_WINDOW_KEY_TAB           258
	#define NV_WINDOW_KEY_BACKSPACE     259
	#define NV_WINDOW_KEY_INSERT        260
	#define NV_WINDOW_KEY_DELETE        261
	#define NV_WINDOW_KEY_RIGHT         262
	#define NV_WINDOW_KEY_LEFT          263
	#define NV_WINDOW_KEY_DOWN          264
	#define NV_WINDOW_KEY_UP            265
	#define NV_WINDOW_KEY_PAGE_UP       266
	#define NV_WINDOW_KEY_PAGE_DOWN     267
	#define NV_WINDOW_KEY_HOME          268
	#define NV_WINDOW_KEY_END           269
	#define NV_WINDOW_KEY_CAPS_LOCK     280
	#define NV_WINDOW_KEY_SCROLL_LOCK   281
	#define NV_WINDOW_KEY_NUM_LOCK      282
	#define NV_WINDOW_KEY_PRINT_SCREEN  283
	#define NV_WINDOW_KEY_PAUSE         284
	#define NV_WINDOW_KEY_F1            290
	#define NV_WINDOW_KEY_F2            291
	#define NV_WINDOW_KEY_F3            292
	#define NV_WINDOW_KEY_F4            293
	#define NV_WINDOW_KEY_F5            294
	#define NV_WINDOW_KEY_F6            295
	#define NV_WINDOW_KEY_F7            296
	#define NV_WINDOW_KEY_F8            297
	#define NV_WINDOW_KEY_F9            298
	#define NV_WINDOW_KEY_F10           299
	#define NV_WINDOW_KEY_F11           300
	#define NV_WINDOW_KEY_F12           301
	#define NV_WINDOW_KEY_F13           302
	#define NV_WINDOW_KEY_F14           303
	#define NV_WINDOW_KEY_F15           304
	#define NV_WINDOW_KEY_F16           305
	#define NV_WINDOW_KEY_F17           306
	#define NV_WINDOW_KEY_F18           307
	#define NV_WINDOW_KEY_F19           308
	#define NV_WINDOW_KEY_F20           309
	#define NV_WINDOW_KEY_F21           310
	#define NV_WINDOW_KEY_F22           311
	#define NV_WINDOW_KEY_F23           312
	#define NV_WINDOW_KEY_F24           313
	#define NV_WINDOW_KEY_F25           314
	#define NV_WINDOW_KEY_KP_0          320
	#define NV_WINDOW_KEY_KP_1          321
	#define NV_WINDOW_KEY_KP_2          322
	#define NV_WINDOW_KEY_KP_3          323
	#define NV_WINDOW_KEY_KP_4          324
	#define NV_WINDOW_KEY_KP_5          325
	#define NV_WINDOW_KEY_KP_6          326
	#define NV_WINDOW_KEY_KP_7          327
	#define NV_WINDOW_KEY_KP_8          328
	#define NV_WINDOW_KEY_KP_9          329
	#define NV_WINDOW_KEY_KP_DECIMAL    330
	#define NV_WINDOW_KEY_KP_DIVIDE     331
	#define NV_WINDOW_KEY_KP_MULTIPLY   332
	#define NV_WINDOW_KEY_KP_SUBTRACT   333
	#define NV_WINDOW_KEY_KP_ADD        334
	#define NV_WINDOW_KEY_KP_ENTER      335
	#define NV_WINDOW_KEY_KP_EQUAL      336
	#define NV_WINDOW_KEY_LEFT_SHIFT    340
	#define NV_WINDOW_KEY_LEFT_CONTROL  341
	#define NV_WINDOW_KEY_LEFT_ALT      342
	#define NV_WINDOW_KEY_LEFT_SUPER    343
	#define NV_WINDOW_KEY_RIGHT_SHIFT   344
	#define NV_WINDOW_KEY_RIGHT_CONTROL 345
	#define NV_WINDOW_KEY_RIGHT_ALT     346
	#define NV_WINDOW_KEY_RIGHT_SUPER   347
	#define NV_WINDOW_KEY_MENU          348

	#define NV_WINDOW_KEY_JOYSTICK_1   0
	#define NV_WINDOW_KEY_JOYSTICK_2   1
	#define NV_WINDOW_KEY_JOYSTICK_3   2
	#define NV_WINDOW_KEY_JOYSTICK_4   3
	#define NV_WINDOW_KEY_JOYSTICK_5   4
	#define NV_WINDOW_KEY_JOYSTICK_6   5
	#define NV_WINDOW_KEY_JOYSTICK_7   6
	#define NV_WINDOW_KEY_JOYSTICK_8   7
	#define NV_WINDOW_KEY_JOYSTICK_9   8
	#define NV_WINDOW_KEY_JOYSTICK_10  9
	#define NV_WINDOW_KEY_JOYSTICK_11 10
	#define NV_WINDOW_KEY_JOYSTICK_12 11
	#define NV_WINDOW_KEY_JOYSTICK_13 12
	#define NV_WINDOW_KEY_JOYSTICK_14 13
	#define NV_WINDOW_KEY_JOYSTICK_15 14
	#define NV_WINDOW_KEY_JOYSTICK_16 15
#endif
