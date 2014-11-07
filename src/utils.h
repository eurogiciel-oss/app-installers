/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#pragma once

/* standard read but retrying if interrupted (EINTR) */
ssize_t utils_read (int fd, void *buf, size_t count);

/* standard write but retrying if interrupted (EINTR) */
ssize_t utils_write (int fd, const void *buf, size_t count);
