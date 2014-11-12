/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

/* Private structure for a reader */
struct xml_reader;

/* Description of an expected element */
struct xml_read_elem
{
  /* Element name */
  const char *name;
  /* Called when entering the element, should return 0 on success */
  int (*begin) (struct xml_reader * reader, const char *name,
		const char **attrs, void *data);
  /* Called when leaving the element, should return 0 on success */
  int (*end) (struct xml_reader * reader, const char *name, void *data);
  /* Called for character content of the element, should return 0 on success */
  int (*characters) (struct xml_reader * reader, const char *ch, int len,
		     void *data);
};

/* Macro that fills an element description */
#define XML_READ_ELEM($name,$begin,$end,$characters) {\
  .name = $name,\
  .begin = (int(*)(struct xml_reader*,const char*,const char**,void*))($begin),\
  .end = (int(*)(struct xml_reader*,const char*,void*))($end),\
  .characters = (int(*)(struct xml_reader*,const char*,int,void*))($characters)\
 }

/* Macro that declare an element description structure */
#define DECL_XML_READ_ELEM($var,$name,$begin,$end,$characters) \
  struct xml_read_elem $var = XML_READ_ELEM($name,$begin,$end,$characters)

/* Gets the value of the attribute of 'name' in the list 'attrs' as
 given in the "begin" callback. Returns NULL if no attribute of 'name'. */
const char *xml_read_attribute (const char **attrs, const char *name);

/* Gets a fresh allocated copy of the value of the attribute of 'name' in
 the list 'attrs' as given in the "begin" callback. The allocated string
 is returned in *'copy'. Returns:
  * 0 if there is no attribute of 'name',
  * 1 if the attribute of name exists and its value is succefully allocated and copied,
  * -1 if the attribute of name exists but the allocation failed. */
int xml_read_attribute_copy (const char **attrs, const char *name,
			     char **copy);

/* Adds the element to the set of currently accepted elements.
 Returns 0 in case of success or -1 with errno set in case of error. */
int xml_read_accept_add (struct xml_reader *reader,
			 struct xml_read_elem *elem);

/* Removes the element from the set of currently accepted elements.
 Returns 0 in case of success or -1 with errno set in case of error. */
int xml_read_accept_drop (struct xml_reader *reader,
			  struct xml_read_elem *elem);

/* Sets the set of accepted elements to be the 'nelems' elements of 'elems'.
 Returns 0 in case of success or -1 with errno set in case of error. */
int xml_read_accept_set (struct xml_reader *reader,
			 struct xml_read_elem **elems, int nelems);

/* Saves the current set of accepted alements and empties the set of
 currently accepted elements until the further matching "xml_read_accept_pop".
 Returns 0 in case of success or -1 with errno set in case of error. */
int xml_read_accept_push_nothing (struct xml_reader *reader);

/* Saves the current set of accepted element and pushs the new set of
 accepted elements to be the 'nelems' elements of 'elems' until the further
 matching "xml_read_accept_pop". Equivalent to "xml_read_accept_push_nothing"
 followed by "xml_read_accept_set".
 Returns 0 in case of success or -1 with errno set in case of error. */
int xml_read_accept_push (struct xml_reader *reader,
			  struct xml_read_elem **elems, int nelems);

/* Pops the currently accepted elements and restores the previous set.
 Returns 0 in case of success or -1 with errno set in case of error. */
int xml_read_accept_pop (struct xml_reader *reader);

/* Reads the file of 'path' and parse it. It accepts any of the given
 'nroots' elements described by 'roots'. 'data' is a user data pointer
 given as is in callbacks. Returns 0 in case of success or -1 with
 errno set in case of error. */
int xml_read_file (const char *path, struct xml_read_elem **roots, int nroots,
		   void *data);
