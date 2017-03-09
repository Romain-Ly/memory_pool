#ifndef ERR_H_
#define ERR_H_

#include <stdio.h>

extern FILE *fdout;

#define SET_ERR(FD) fdout = FD
#define INIT_ERR() FILE *fdout
#define FLUSH fflush(fdout)

#if defined(DEBUG)
	#define dbg_print(FMT, ...) do { \
		fprintf(fdout, \
			 "DEBUG(%s:%d <%s>): " FMT, \
			 __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
		FLUSH; \
} while (0)
	#define err_print(FMT, ...) do { \
		fprintf(fdout, \
			 "ERROR(%s:%d <%s>): " FMT, \
			 __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
		FLUSH; \
} while (0)
	#define warn_print(FMT, ...) do { \
		fprintf(fdout, \
			 "WARN(%s:%d <%s>): " FMT, \
			 __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
		FLUSH; \
} while (0)
	#define print(FMT, ...) do { \
		fprintf(fdout, \
			 "%s:%d <%s>: " FMT, \
			 __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
		FLUSH; \
} while (0)
#else
	#define dbg_print(...)
	#define err_print(FMT, ...) fprintf(fdout, \
	"ERROR: " FMT, ## __VA_ARGS__)
	#define print(FMT, ...) fprintf(fdout, \
	FMT, ## __VA_ARGS__)
#endif

#endif /* ERR_H_ */
