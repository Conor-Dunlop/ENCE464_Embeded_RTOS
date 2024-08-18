#include "fff.h"
#include "circ_buf_t.h"

#ifdef FFF_MOCK_IMPL
    #define VOID_FUNC FAKE_VOID_FUNC
    #define VALUE_FUNC FAKE_VALUE_FUNC
#else
    #define VOID_FUNC DECLARE_FAKE_VOID_FUNC
    #define VALUE_FUNC DECLARE_FAKE_VALUE_FUNC
#endif

#define FFF_circ_buf_t_FAKES_LIST(FUNC)         \
    FUNC(initCircBuf)                    \
    FUNC(writeCircBuf)                   \
    FUNC(readCircBuf)                    \
    FUNC(freeCircBuf)      

VALUE_FUNC(circBuf_t *, initCircBuf, uint32_t);
VOID_FUNC(writeCircBuf, circBuf_t *, uint32_t);
VALUE_FUNC(uint32_t, readCircBuf, circBuf_t *);
VOID_FUNC(freeCircBuf, circBuf_t *);
