#ifndef __TES_UTILS_H__
#define __TES_UTILS_H__


// String and MACRO concatenator.
// The extra level of indirection will allow the preprocessor to expand the
// macros before they are converted to strings.
#define STR_HELPER(X) #X
#define STR(X)        STR_HELPER(X)

struct msg parse_request    (const char * const request);
struct msg parse_rqt_request(const char * const request);
struct msg parse_rqs_request(const char * const request);

struct msg reply_request(const struct msg * const request);


#endif // __TES_UTILS_H__
