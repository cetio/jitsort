# jitsort

Inspired by sleep sort, JITSort is a state of the art sorting algorithm that analyzes your array and generates x86_64 assembly code in executable memory using the values in the array as indexes into the memory, and returns the generated code as an executable function that returns the sorted array.

> [!WARNING]
> Ensure that your device has sufficient memory or your array doesn't have numbers or strings that are too big because there are no checks stopping this from allocating a petabyte of memory.

## Usage

```c
#include "jitsort.h"

// Sort integers
int arr[] = {3, 1, 2};
void* (*sort)() = jitsort(arr, sizeof(int), 3, NULL);
int* sorted = (int*)sort();
// sorted now contains {1, 2, 3}
```

## API

```c
void* (*jitsort(const void* ptr, size_t size, size_t len, size_t (*func)(const void*)))()
```

> [!NOTE]
> If providing a function to calculate size, `func` must return 0 when the provided pointer is invalid or no elements remain.

- `ptr`: Pointer to the array
- `size`: Size of each element
- `len`: Number of elements
- `func`: Optional function to get element size (for variable-length data)

Returns a function pointer that when called returns the sorted array.

## Building

```bash
make
```

## Testing

```bash
make run
```

## License

jitsort is licensed under [AGPL-3.0](LICENSE.txt)