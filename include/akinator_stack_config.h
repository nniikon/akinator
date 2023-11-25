#include <wchar.h>

/** Release Mode
 * - Optimizes the code for performance.
 * - Does not fill unused hash values with poison.
 * - Validates only essential parameters.
 */
#define RELEASE

/** Canary Protection
 * - Enhances security using canary values to protect the stack and data.
 * - Slightly increases the execution time of every stack operation.
 * - Requires a slightly larger amount of memory.
 */
#undef CANARY_PROTECT

/** Hash Protection
 * - Strengthens security using a hash function to protect the stack and data.
 * - Significantly increases the execution time of every stack operation.
 */
#undef HASH_PROTECT

// Element type.
typedef const wchar_t* stkElem_t;

// Canary type.
typedef unsigned long long canary_t;

// Format specifier for printf() to print elements.
#define ELEM_FORMAT "%p"

// Poison value for uninitialized elements.
static const stkElem_t POISON = NULL;

// Format specifier for printf() to print canary values.
#define CANARY_FORMAT "%llx"

// Canary value to protect data.
static const canary_t CANARY_VALUE = 0xBAADF00D;

// Default minimum capacity that the stack can have.
static const int STACK_SIZE_DEFAULT = 32;

// Multiplier by which the capacity of the stack will be increased when needed.
static const float STACK_CAPACITY_MULTIPLIER = 2.0;