#if !defined(NV_STACK_HEADER)
	#define NV_STACK_HEADER
	#include <cstdlib>

	// 2 Gb
	#define NV_STACK_CHUNK_SIZE 2147483648

	namespace nv
	{
		template<typename type>
		class stack
		{
			public:
			static inline size_t size()
			{
				return length;
			};

			static inline size_t capacity()
			{
				return max_length;
			};

			static inline size_t blocks()
			{
				return counter;
			};

			explicit stack(const size_t n): block_start(length), block_length(n)
			{
				if (n*sizeof(type) > (max_length - length))
				{
					max_length += NV_STACK_CHUNK_SIZE;

					auto new_buffer = (uint8_t *) realloc(buffer, sizeof(uint8_t)*max_length);

					if (new_buffer != nullptr)
						buffer = new_buffer;
					else
						return;
				}

				length += n*sizeof(type);
				++counter;
			};

			type &operator [](const size_t n)
			{
				return buffer[this->block_start + n];
			};

			~stack()
			{
				length -= this->block_length*sizeof(type);
				--counter;

				if (counter == 0)
				{
					free(buffer);
					max_length = 0;
					buffer = nullptr;
				}
			};

			private:
			size_t block_start;
			size_t block_length;

			// NOTE: counter keeps track of how many instances (blocks) are created.
			inline static size_t counter = 0;

			// NOTE: length is the total number of elements stored (< max_length).
			inline static size_t length = 0;

			// NOTE: max_length is the size of the storage space currently allocated.
			inline static size_t max_length = 0;

			// NOTE: buffer points to the chunck of allocated memory.
			inline static uint8_t *buffer = nullptr;
		};
	}
#endif
