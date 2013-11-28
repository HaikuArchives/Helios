// MakeBFSImage

#ifndef _MAKE_BFS_IMAGE_
#define _MAKE_BFS_IMAGE_

#include <fs_attr.h>

status_t					MakeBFSImage(const char *filename);
off_t							GetBFSImageSize(const char * baseFolder);

#endif
// _MAKE_BFS_IMAGE_
