#ifndef __UTILS_H__
#define __UTILS_H__


#include <sys/types.h>

// WARNING
//
// Avoid the *FAIL macros, as the applications must not crash!
// Use of perror is encouraged, but ensure that your application is exiting
// cleanly and only when strictly necessary (like in the eminence of the of the
// world).
#define FAIL return fail("error")
#define MAY_FAIL(X) if ((X) == -1) FAIL

#define PORT         59000 + GROUP_NUMBER
#define GROUP_NUMBER 9

#define REQUEST_MAX_PARAMETERS 8
#define REQUEST_MAX_SIZE       10 * 1024
#define REQUEST_TYPE_SIZE      3


// Helper function to indicate failure. Beware that this may not be a clean a
// way to exit the program, e.g., if you need to free resources.
int fail(const char * const msg);

// Attempts to read nbytes bytes of data (or the whole file if the file has less
// than nbytes bytes) from the object referenced by the descriptor fd into the
// buffer pointed to by buf. If successful, returns the actual number of bytes
// read. Otherwise, returns -1 and the global variable errno is set to indicate
// the error.
int read_bytes(const int fd, const unsigned nbytes, char * buf);

// Converts a string to uppercase
char *strupr(char *str);

// Generates a pseudo-random integer in the *closed* range specified.
int random_in_range(int l, int r);

// Get's a file size.
off_t fsize(const char *fname);

// Basic representation of a client-server request/reply.
struct msg {
    char * type;
    char ** parameters;
    unsigned n_parameters; // Number of parameters.
};

// msg representation of an error
const struct msg error_msg;

// Creates a new msg structure. Accepts a string consisting of a request type
// and its parameters space separated, e.g., "type p1 p2 p3 p4".
// The user then should free the resources (for example, using free_msg).
struct msg * new_msg(const char * str);

// Duplicates a message, pretty much like strdup duplicates a string.
struct msg * msgdup(const struct msg * const m);

// Frees the resources allocated when creating a msg.
// Only use when all the resources regarding the msg tofree where manually
// allocated.
void free_msg(struct msg * tofree);

// Converts a struct msg to a string according to the project protocol.
// So if msg has type t and parameters p1, p2, ..., pn, the resulting string
// will be "t p1 p2 ... pn".
// The returning string will need to be freed.
char * msg_to_string(const struct msg * const m);


#endif // __UTILS_H__
