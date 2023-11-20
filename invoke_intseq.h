#ifndef INVOKE_INTSEQ
#define INVOKE_INTSEQ

#include <iostream>

typedef int result_type;
template <class F, class... Args>
result_type invoke_intseq(F&& f, Args&&... args);
#endif // !INVOKE_INTSEQ
