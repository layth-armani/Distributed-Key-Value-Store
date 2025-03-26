#define MAKE_FAKE_NETWORK              \
    do {                               \
        ADD_ENTRY("a", "hello a");     \
        ADD_ENTRY("ab", "hello ab");   \
        ADD_ENTRY("bb", "hello bb");   \
        ADD_ENTRY("bbb", "hello bbb"); \
    } while (0)
