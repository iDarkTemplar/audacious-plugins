/*
 * Unbuffered io for ffmpeg system
 * Copyright (c) 2001 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "avformat.h"
#include "cutils.h"

static int default_interrupt_cb(void);

URLProtocol *first_protocol = NULL;
URLInterruptCB *url_interrupt_cb = default_interrupt_cb;

int register_protocol(URLProtocol *protocol)
{
    URLProtocol **p;
    p = &first_protocol;
    while (*p != NULL) p = &(*p)->next;
    *p = protocol;
    protocol->next = NULL;
    return 0;
}

int url_vopen(URLContext **puc, VFSFile *fd)
{
    URLContext *uc;
    URLProtocol *up;
    const char *p;
    char proto_str[128], *q;
    int err;

    up = first_protocol;
    uc = av_malloc(sizeof(URLContext) + strlen(fd->uri ? fd->uri : ""));
    if (!uc) {
        err = -ENOMEM;
        goto fail;
    }
    strcpy(uc->filename, fd->uri ? fd->uri : "");
    uc->prot = up;
    uc->flags = URL_RDONLY;
    uc->is_streamed = 0; /* default = not streamed */
    uc->max_packet_size = 0; /* default: stream file */
    uc->priv_data = fd;
    if (err < 0) {
        free(uc);
        *puc = NULL;
        return err;
    }
    *puc = uc;
    return 0;
 fail:
    *puc = NULL;
    return err;
}

int url_open(URLContext **puc, const char *filename, int flags)
{
    URLContext *uc;
    URLProtocol *up;
    const char *p;
    char proto_str[128], *q;
    int err;

    up = first_protocol;
    uc = av_malloc(sizeof(URLContext) + strlen(filename));
    if (!uc) {
        err = -ENOMEM;
        goto fail;
    }
    strcpy(uc->filename, filename);
    uc->prot = up;
    uc->flags = flags;
    uc->is_streamed = 0; /* default = not streamed */
    uc->max_packet_size = 0; /* default: stream file */
    err = up->url_open(uc, filename, flags);
    if (err < 0) {
        free(uc);
        *puc = NULL;
        return err;
    }
    *puc = uc;
    return 0;
 fail:
    *puc = NULL;
    return err;
}

int url_read(URLContext *h, unsigned char *buf, int size)
{
    int ret;
    if (h->flags & URL_WRONLY)
        return -EIO;
    ret = h->prot->url_read(h, buf, size);
    return ret;
}

offset_t url_seek(URLContext *h, offset_t pos, int whence)
{
    offset_t ret;

    if (!h->prot->url_seek)
        return -EPIPE;
    ret = h->prot->url_seek(h, pos, whence);
    return ret;
}

int url_close(URLContext *h)
{
	int ret;

    	ret = h->prot->url_close(h);
    	free(h);
    
	return ret;
}

int url_exist(const char *filename)
{
    URLContext *h;
    if (url_open(&h, filename, URL_RDONLY) < 0)
        return 0;
    url_close(h);
    return 1;
}

offset_t url_filesize(URLContext *h)
{
    offset_t pos, size;
    
    pos = url_seek(h, 0, SEEK_CUR);
    size = url_seek(h, 0, SEEK_END);
    url_seek(h, pos, SEEK_SET);
    return size;
}

/* 
 * Return the maximum packet size associated to packetized file
 * handle. If the file is not packetized (stream like http or file on
 * disk), then 0 is returned.
 * 
 * @param h file handle
 * @return maximum packet size in bytes
 */
int url_get_max_packet_size(URLContext *h)
{
    return h->max_packet_size;
}

void url_get_filename(URLContext *h, char *buf, int buf_size)
{
    pstrcpy(buf, buf_size, h->filename);
}


static int default_interrupt_cb(void)
{
    return 0;
}

/** 
 * The callback is called in blocking functions to test regulary if
 * asynchronous interruption is needed. -EINTR is returned in this
 * case by the interrupted function. 'NULL' means no interrupt
 * callback is given.  
 */
void url_set_interrupt_cb(URLInterruptCB *interrupt_cb)
{
    if (!interrupt_cb)
        interrupt_cb = default_interrupt_cb;
    url_interrupt_cb = interrupt_cb;
}
