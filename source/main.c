#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "jitsort.h"

static size_t get_string_size(const void* ptr)
{
    const char* str = *(const char**)ptr;
    return str != NULL ? strlen(str) + 1 : 0;
}

int main()
{
    printf("Testing jitsort...\n");
    
    // Test integers
    int ints[] = {3, 1, 2};
    void* (*f)() = jitsort(ints, sizeof(int), 3, NULL);
    assert(f != NULL);
    int* sorted_ints = (int*)f();
    assert(sorted_ints != NULL);
    assert(sorted_ints[0] == 1 && sorted_ints[1] == 2 && sorted_ints[2] == 3);
    free(sorted_ints);
    printf("✓ Integers\n");
    
    // Test chars
    char chars[] = {'c', 'a', 'b'};
    f = jitsort(chars, sizeof(char), 3, NULL);
    assert(f != NULL);
    char* sorted_chars = (char*)f();
    assert(sorted_chars != NULL);
    assert(sorted_chars[0] == 'a' && sorted_chars[1] == 'b' && sorted_chars[2] == 'c');
    free(sorted_chars);
    printf("✓ Characters\n");
    
    // Test strings
    const char* strings[] = {"world", "hello"};
    f = jitsort(strings, sizeof(char*), 2, get_string_size);
    assert(f != NULL);
    char* sorted_strings = (char*)f();
    assert(sorted_strings != NULL);
    free(sorted_strings);
    printf("✓ Strings\n");
    
    // Test single element
    int single[] = {42};
    f = jitsort(single, sizeof(int), 1, NULL);
    assert(f != NULL);
    int* sorted_single = (int*)f();
    assert(sorted_single != NULL);
    assert(sorted_single[0] == 42);
    free(sorted_single);
    printf("✓ Single element\n");
    
    // Test edge cases
    assert(jitsort(NULL, sizeof(int), 5, NULL) == NULL);
    assert(jitsort(ints, 0, 0, NULL) == NULL);
    printf("✓ Edge cases\n");
    
    printf("All tests passed.\n");
    return 0;
}