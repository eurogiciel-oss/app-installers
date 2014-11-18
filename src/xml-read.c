/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <syslog.h>

#include <libxml/parser.h>
#include <libxml/xmlstring.h>

#include "xml-read.h"

#define cmp(x,y) (strcasecmp((x),(y)))
#define is(x,y)  (0==cmp((x),(y)))

#define MAX_DEPTH    128	/* maximum depth of the document */
#define MAX_ELEM    1024	/* maximum count of elements in the stack */

/* path of the current element */
struct path
{
  int count;			/* count or depth of the path */
  struct xml_read_elem *last;	/* last accepted element */
  struct xml_read_elem *path[MAX_DEPTH + 1];	/* path of parents of last [0..count[ */
};

/* accepting set data */
struct accept
{
  int base;			/* base of the accepting set */
  int count;			/* count of currently accepted elements */
  struct xml_read_elem *elems[MAX_ELEM];	/* elements, currently accepted: [base..base+count[, sorted */
  int pushed;			/* count of pushed accepting set */
  int stack[MAX_DEPTH];		/* stack of the counts pushed */
};

/* the reader */
struct xml_reader
{
  int err;			/* recorded error */
  void *data;			/* user data */
  struct path path;		/* current path */
  struct accept accept;		/* accepting state */
};

#if defined(XML_READ_SORT)
/* sorting callback (sorting accepting set) */
static int
cmpsort (const void *a, const void *b)
{
  return cmp ((*(const struct xml_read_elem **) a)->name, (*(const struct xml_read_elem **) b)->name);
}

/* searching callback (searching an element) */
static int
cmpsearch (const void *a, const void *b)
{
  return cmp ((const char *) a, (*(const struct xml_read_elem **) b)->name);
}
#endif

static void
reader_characters (struct xml_reader *reader, const char *ch, int len)
{
  /* nothing if error recorded */
  if (reader->err)
    return;

  /* trim */
  if (ch[0] == 0x09 || ch[0] == 0x0D || ch[0] == 0x0A)
    return;

  /* call the element "characters" routine if needed */
  if (reader->path.last->characters)
    if (reader->path.last->characters (reader, ch, len, reader->data))
      reader->err = errno;
}

static void
reader_start_element (struct xml_reader *reader, const char *name, const char **attrs)
{
  struct xml_read_elem *elem, **found;

  /* nothing if error recorded */
  if (reader->err)
    return;

  /* search the element in the accepting set */
#if defined(XML_READ_SORT)
  found =
    bsearch (name, &reader->accept.elems[reader->accept.base],
	     reader->accept.count, sizeof *reader->accept.elems, cmpsearch);
  if (found == NULL)
    found =
      bsearch ("", &reader->accept.elems[reader->accept.base],
	       reader->accept.count, sizeof *reader->accept.elems, cmpsearch);
#else
  {
    int i, e;
    i = reader->accept.base;
    e = i + reader->accept.count;
    found = NULL;
    while (i < e)
      {
        if (!reader->accept.elems[i]->name[0])
          found = &reader->accept.elems[i];
        else if (is(reader->accept.elems[i]->name, name))
          {
            found = &reader->accept.elems[i];
            break;
          }
        i++;
      }
  }
#endif
  if (found == NULL)
    {
      reader->err = EINVAL;
      return;
    }
  assert (*found != NULL);

  /* pushs the elements path */
  if (reader->path.count == sizeof reader->path.path / sizeof *reader->path.path)
    {
      reader->err = E2BIG;
      return;
    }
  reader->path.path[reader->path.count++] = reader->path.last;
  elem = *found;
  reader->path.last = elem;

  /* call the element "begin" routine if needed */
  if (elem->begin != NULL)
    if (elem->begin (reader, name, attrs, reader->data))
      reader->err = errno;
}

static void
reader_end_element (struct xml_reader *reader, const char *name)
{
  /* nothing if error recorded */
  if (reader->err)
    return;

  assert (reader->path.last != NULL);
  assert (reader->path.count != 0);
  assert (is (name, reader->path.last->name) || !*reader->path.last->name);

  /* call the element "end" routine if needed */
  if (reader->path.last->end)
    if (reader->path.last->end (reader, name, reader->data))
      reader->err = errno;

  /* pops the elements path */
  reader->path.last = reader->path.path[--reader->path.count];
}

/* initialisation of the reader */
static void
reader_init (struct xml_reader *reader, struct xml_read_elem **roots, int nroots, void *data)
{
  reader->err = 0;
  reader->data = data;
  reader->path.last = NULL;
  reader->path.count = 0;
  reader->accept.base = 0;
  reader->accept.count = 0;
  reader->accept.pushed = 0;
  if (xml_read_accept_set (reader, roots, nroots))
    reader->err = E2BIG;
}

int
xml_read_accept_add (struct xml_reader *reader, struct xml_read_elem *elem)
{
  /* check not full */
  if (reader->accept.base + reader->accept.count >= (sizeof reader->accept.stack / sizeof *reader->accept.stack))
    {
      errno = E2BIG;
      return -1;
    }

  /* add and sort */
  reader->accept.elems[reader->accept.base + reader->accept.count++] = elem;
#if defined(XML_READ_SORT)
  qsort (&reader->accept.elems[reader->accept.base], reader->accept.count, sizeof *reader->accept.elems, cmpsort);
#endif
  return 0;
}

int
xml_read_accept_drop (struct xml_reader *reader, struct xml_read_elem *elem)
{
  struct xml_read_elem **read, **write, **end;

  /* search ... */
  read = &reader->accept.elems[reader->accept.base];
  write = read;
  end = read + reader->accept.count;
  while (read != end)
    {
      if (*read != elem)
	read++;
      else
	{
	  /* ... and destroy */
	  write = read++;
	  reader->accept.count--;
	  while (read != end)
	    {
	      *write++ = *read++;
	    }
	}
    }
  return 0;
}

int
xml_read_accept_set (struct xml_reader *reader, struct xml_read_elem **elems, int nelems)
{
  /* check args */
  if (nelems < 0)
    {
      errno = EINVAL;
      return -1;
    }

  /* check enough space */
  if (reader->accept.base + nelems > (sizeof reader->accept.stack / sizeof *reader->accept.stack))
    {
      errno = E2BIG;
      return -1;
    }

  /* set and sort */
  memcpy (&reader->accept.elems[reader->accept.base], elems, nelems * sizeof *reader->accept.elems);
  reader->accept.count = nelems;
#if defined(XML_READ_SORT)
  qsort (&reader->accept.elems[reader->accept.base], reader->accept.count, sizeof *reader->accept.elems, cmpsort);
#endif

  return 0;
}

int
xml_read_accept_push_nothing (struct xml_reader *reader)
{
  /* check enough space */
  if (reader->accept.pushed == (sizeof reader->accept.stack / sizeof *reader->accept.stack))
    {
      errno = E2BIG;
      return -1;
    }

  /* push the empty set */
  reader->accept.stack[reader->accept.pushed++] = reader->accept.count;
  reader->accept.base += reader->accept.count;
  reader->accept.count = 0;

  return 0;
}

int
xml_read_accept_push (struct xml_reader *reader, struct xml_read_elem **elems, int nelems)
{
  if (xml_read_accept_push_nothing (reader))
    return -1;
  return xml_read_accept_set (reader, elems, nelems);
}

int
xml_read_accept_pop (struct xml_reader *reader)
{
  /* check ahs something pushed */
  if (reader->accept.pushed == 0)
    {
      errno = EINVAL;
      return -1;
    }

  /* restore the saved accepting set */
  assert (reader->accept.stack[reader->accept.pushed - 1] >= 0);
  assert (reader->accept.base - reader->accept.stack[reader->accept.pushed - 1] >= 0);
  reader->accept.count = reader->accept.stack[--reader->accept.pushed];
  reader->accept.base -= reader->accept.count;
  return 0;
}

const char *
xml_read_attribute (const char **attrs, const char *name)
{
  while (attrs[0] != NULL)
    {
      if (is (attrs[0], name))
	return attrs[1] != NULL ? attrs[1] : "";
      attrs += 2;
    }
  return NULL;
}

int
xml_read_attribute_copy (const char **attrs, const char *name, char **copy)
{
  const char *value;

  value = xml_read_attribute (attrs, name);
  if (value == NULL)
    return 0;

  *copy = strdup (value);
  if (*copy != NULL)
    return 1;

  errno = ENOMEM;
  return -1;
}

int
xml_read_file (const char *path, struct xml_read_elem **roots, int nroots, void *data)
{
  int status;
  struct xml_reader reader;
  xmlSAXHandler sax;

  /* init */
  memset (&reader, 0, sizeof reader);
  memset (&sax, 0, sizeof sax);
/*
  xmlSAX2InitDefaultSAXHandler (&sax, 0);
*/
  sax.startElement = (startElementSAXFunc) reader_start_element;
  sax.endElement = (endElementSAXFunc) reader_end_element;
  sax.characters = (charactersSAXFunc) reader_characters;
  reader_init (&reader, roots, nroots, data);

  /* process */
  status = xmlSAXUserParseFile (&sax, &reader, path);
  if (status)
    {
      syslog (LOG_ERR, "Failed to parse XML file %s", path);
      errno = ECANCELED;
      return -1;
    }

  if (reader.err)
    {
      errno = reader.err;
      return -1;
    }

  return 0;
}
