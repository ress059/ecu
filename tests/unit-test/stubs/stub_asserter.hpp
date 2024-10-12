


#include <cstdint>

namespace stubs {


enum class assert_response : std::uint8_t
{
    FAIL,   /**< Test will fail if an assert fires. */
    OK      /**< Test will not fail if an assert fires. */
};


extern void set_assert_handler(assert_response response);

extern void set_assert_handler(void (*handler)(const char *file, int line));


} /* namespace stubs */
