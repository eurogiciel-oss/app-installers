/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

extern "C" {
#include "xml-read.h"
}

#include <string>
#include "Fail.hxx"

namespace AppInstaller {

struct XmlReadAttributes {
	const char *attrs;

	inline XmlReadAttributes(const char *xattrs) : attrs(xattrs) {}

	inline bool has(const std::string &name) {
		return xml_read_attribute(attrs, name.c_str()) != NULL;
	}
	inline std::string &operator[](const std::string &name) {
		const char *value = xml_read_attribute(attrs, name.c_str());
		return std::string(value ? value : "");
	}
};

struct XmlReader {
	struct xml_reader *xmlReader;

	inline XmlReader(struct xml_reader *reader) : xmlReader(reader) {}
	inline XmlReader(const XmlReader &reader) : xmlReader(reader.xmlReader) {}

	inline FailCode add(XmlReadElem &elem) {
		return xml_read_accept_add (xmlReader, elem);
	}

	inline FailCode drop(XmlReadElem &elem) {
		return xml_read_accept_drop (xmlReader, elem);
	}

/* Sets the set of accepted elements to be the 'nelems' elements of 'elems'.
 Returns 0 in case of success or -1 with errno set in case of error. */
int xml_read_accept_set (struct xml_reader *reader, struct xml_read_elem **elems, int nelems);

	inline FailCode push() {
		return xml_read_accept_push_nothing (xmlReader);
	}

	inline FailCode push(...) {
		FailCode code = push();
		if (code) code = set(elems);
		return code;
	}

	inline FailCode pop() {
		return xml_read_accept_pop (xmlReader);
	}

/* Reads the file of 'path' and parse it. It accepts any of the given
 'nroots' elements described by 'roots'. 'data' is a user data pointer
 given as is in callbacks. Returns 0 in case of success or -1 with
 errno set in case of error. */
	FailCode readFile (const std::string &path, xml_read_file (const char *path, struct xml_read_elem **roots, int nroots, void *data);

};



/* Private structure for a reader */
struct xml_reader;

/* Description of an expected element */
struct xml_read_elem
{
  /* Element name */
  const char *name;
  /* Called when entering the element, should return 0 on success */
  int (*begin) (struct xml_reader * reader, const char *name, const char **attrs, void *data);
  /* Called when leaving the element, should return 0 on success */
  int (*end) (struct xml_reader * reader, const char *name, void *data);
  /* Called for character content of the element, should return 0 on success */
  int (*characters) (struct xml_reader * reader, const char *ch, int len, void *data);
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



}


