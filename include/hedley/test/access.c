#include "../hedley.h"

#if defined(HEDLEY_GNUC_VERSION)
#pragma GCC diagnostic warning "-Wstringop-overread"
#endif

HEDLEY_ACCESS(__read_only__, 1, 2)
void mymemcpy(HEDLEY_UNUSED void *p, HEDLEY_UNUSED unsigned int size) {
	HEDLEY_USE(size);
	HEDLEY_USE(p);
}

int main(void) {
	char buf[10] = {0};
	mymemcpy(buf, 20);
}
