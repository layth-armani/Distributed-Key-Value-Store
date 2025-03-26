#define MAKE_FAKE_NETWORK                              \
    do {                                               \
        ADD_ENTRY("", "empty");                        \
        ADD_ENTRY("empty", "");                        \
    } while (0)
