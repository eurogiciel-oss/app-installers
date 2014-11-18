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

	inline FailCode add(XmlReadElem &elem) const {
		return xml_read_accept_add (xmlReader, elem);
	}

	inline FailCode drop(XmlReadElem &elem) const {
		return xml_read_accept_drop (xmlReader, elem);
	}

	inline FailCode set(const std::vector<XmlReadElem*> &elems) const {
		struct xml_read_elem *array[elems.size()];
		init(roots, array);
		return xml_read_accept_set(xmlReader, array, elems.size());
	}

	inline FailCode push() const {
		return xml_read_accept_push_nothing (xmlReader);
	}

	inline FailCode push(const std::vector<XmlReadElem*> &elems) const {
		struct xml_read_elem *array[elems.size()];
		init(roots, array);
		return xml_read_accept_push(xmlReader, array, elems.size());
	}

	inline FailCode pop() const {
		return xml_read_accept_pop (xmlReader);
	}

	static inline FailCode readFile (const std::string &path, const std::vector<XmlReadElem*> &roots, void * data) {
		struct xml_read_elem *array[roots.size()];
		init(roots, array);
		return xml_read_file (path.c_str(), array, roots.size(), data);
	}

private:
	static inline void init(const std::vector<XmlReadElem*> &elems, struct xml_read_elem *array) {
		for (int i = 0 , n = elems.size() ; i < n ; i++)
			array[i] = *elems[i];
	}
};

class XmlReadElem {

private:

	virtual FailCode begin(XmlReader reader, const char *name, XmlReadAttributes attrs) = 0;
	virtual FailCode end(XmlReader reader, const char *name) = 0;
	virtual FailCode characters(XmlReader reader, const char *ch, int len) = 0;


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


