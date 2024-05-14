#ifndef TINYC_SPAN_H_
#define TINYC_SPAN_H_

#include "input/cache.h"

typedef struct {
    int row;
    int offset;
} position_t;

typedef struct {
    icache_id_t id;
    position_t start;
    position_t end;
} span_t;

span_t concat_span(span_t a, span_t b);

#endif  // TINYC_SPAN_H_
