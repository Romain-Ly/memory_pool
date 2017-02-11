# Memory pool
A simple memory pool implementation with no 
extra memory required.

TODO:
Fix some errors detected by valgrind :
make DEBUG=1 test

When NVALGRIND flag is added, no error :
make DEBUG=1 CFLAGS="-Wall -Wextra -DNVALGRIND" test
