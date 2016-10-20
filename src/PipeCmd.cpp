#include "PipeCmd.h"

// source code (c) by Philippe Houdoin
thread_id pipe_command(int argc, const char** argv, int& in, int& out, int& err, char** envp)
{
	// Save current FDs
	int old_in = dup(0);
	int old_out = dup(1);
	int old_err = dup(2);

	int filedes[2];

	/* Create new pipe FDs as stdin, stdout, stderr */
	pipe(filedes);
	dup2(filedes[0], 0);
	close(filedes[0]);
	in = filedes[1]; // Write to in, appears on cmd's stdin
	pipe(filedes);
	dup2(filedes[1], 1);
	close(filedes[1]);
	out = filedes[0]; // Read from out, taken from cmd's stdout
	pipe(filedes);
	dup2(filedes[1], 2);
	close(filedes[1]);
	err = filedes[0]; // Read from err, taken from cmd's stderr

	// "load" command.
	thread_id ret = load_image(argc, argv, (const char**)envp);
	// thread ret is now suspended.

	// Restore old FDs
	close(0);
	dup(old_in);
	close(old_in);
	close(1);
	dup(old_out);
	close(old_out);
	close(2);
	dup(old_err);
	close(old_err);

	/* Theoretically I should do loads of error checking, but
	   the calls aren't very likely to fail, and that would
	   muddy up the example quite a bit.  YMMV. */

	return ret;
}
