#ifndef _PIPECMD_H_
#define _PIPECMD_H_

#include <unistd.h>
#include <KernelKit.h>

extern char** environ;
thread_id pipe_command(int argc, const char** argv, int& in, int& out, int& err,
					   char** envp = environ);

#endif
