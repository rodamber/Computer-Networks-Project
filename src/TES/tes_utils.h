#pragma once

// String and MACRO concatenator.
// The extra level of indirection will allow the preprocessor to expand the
// macros before they are converted to strings.
#define STR_HELPER(X) #X
#define STR(X)        STR_HELPER(X)

struct msg * reply_request(const char * const request);
