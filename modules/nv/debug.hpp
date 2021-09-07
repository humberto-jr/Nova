#if !defined(NV_DEBUG_HEADER)
	#define NV_DEBUG_HEADER

	#define NV_ASSERT(expr)                           \
	{                                                 \
	  if (!(expr))                                    \
	  {                                               \
	    PRINT_ERROR("assertion '%s' failed\n", #expr) \
	    exit(EXIT_FAILURE);                           \
	  }                                               \
	}

	#define PRINT_ERROR(format, ...)                                          \
	{                                                                         \
	  fprintf(stderr, "# %s, %s(), line %d: ", __FILE__, __func__, __LINE__); \
	  fprintf(stderr, format, ##__VA_ARGS__);                                 \
	}

	#define ASSERT(expr)                              \
	{                                                 \
	  if (!(expr))                                    \
	  {                                               \
	    PRINT_ERROR("assertion '%s' failed\n", #expr) \
	    exit(EXIT_FAILURE);                           \
	  }                                               \
	}

	#define AS_STRING(macro) #macro

	#define PRINT_MACRO(macro) AS_STRING(macro)
#endif
