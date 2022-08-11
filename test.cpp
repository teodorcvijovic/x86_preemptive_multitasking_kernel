#include <iostream.h>
#include <stdlib.h>
#include <assert.h>

#include "keyevent.h"
#include "bounded.h"
#include "user.h"
#include "intLock.h"
#include "event.h"
#include "semaphor.h"
#include "ivtentry.h"

///////////////////////////////////////////////////////////////

#include <DOS.H>
#include <STDIO.H>
#include <STDARG.H>
#include "locks.h"

#include <assert.h>

int syncPrintf(const char *format, ...)
{
	int ret;
	va_list args;
	LOCKED(
		va_start(args, format);
		ret = vprintf(format, args);
		va_end(args);
	)

	return ret;
}

/////////////////////////////////////////////////////////////////











