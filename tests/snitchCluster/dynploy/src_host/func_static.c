
#include "test_func.h"
#include "test_cluster.h"

int32_t (*get_function_pointer(void** stack_ptr))(void* args) {
    return clusterMain;
}
