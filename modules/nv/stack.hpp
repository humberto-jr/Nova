#if !defined(NV_STACK_HEADER)
	#define NV_STACK_HEADER
	#include <cstdlib>
	#include <cstdint>

	#if !defined(NV_STACK_SIZE_INCREMENT)
		#define NV_STACK_SIZE_INCREMENT 2048
	#endif

	namespace nv
	{
		template<typename T>
		class stack
		{
			public:
			static inline size_t size()
			{
				return sizeof(T)*length;
			};

			static inline size_t type_size()
			{
				return sizeof(T);
			};

			static inline size_t capacity()
			{
				return sizeof(T)*max_length;
			};

			static inline size_t count()
			{
				return counter;
			};

			static inline float usage()
			{
				return static_cast<float>(length)/static_cast<float>(max_length);
			};

			static inline void set_size_increment(const size_t n_bytes)
			{
				size_increment = n_bytes/sizeof(T);
			};

			static void reserve(const size_t n_bytes)
			{
				max_length += n_bytes/sizeof(T);

				auto new_buffer = std::realloc(buffer, sizeof(T)*max_length);

				if (new_buffer != nullptr)
					buffer = static_cast<T*>(new_buffer);
			};

			explicit inline stack(const size_t n = 1):
				rank(++counter), data_length(n), buffer_offset(length)
			{
				while ((max_length - length) < n)
					increase_storage();

				length += n;
			};

			inline bool at_top() const
			{
				return (this->rank == counter);
			}

			inline bool at_bottom() const
			{
				return (this->rank == 1);
			}

			inline size_t block_size() const
			{
				return this->data_length;
			}

			inline T &operator [](const size_t n)
			{
				return buffer[this->buffer_offset + n];
			};

			inline void operator =(const T &value)
			{
				for (size_t n = 0; n < this->data_length; ++n)
					buffer[this->buffer_offset + n] = value;
			};

			~stack()
			{
				length -= this->data_length;
				--counter;

				if (length == 0)
				{
					std::free(buffer);
					buffer = nullptr;
					max_length = 0;
				}
			};

			private:
			const size_t rank;
			const size_t data_length;
			const size_t buffer_offset;

			static inline T *buffer = nullptr;
			static inline size_t length = 0;
			static inline size_t counter = 0;
			static inline size_t max_length = 0;
			static inline size_t size_increment = NV_STACK_SIZE_INCREMENT/sizeof(T);

			void increase_storage()
			{
				max_length += size_increment;

				auto new_buffer = std::realloc(buffer, sizeof(T)*max_length);

				if (new_buffer != nullptr)
					buffer = static_cast<T*>(new_buffer);
			};
		};
	}
#endif
