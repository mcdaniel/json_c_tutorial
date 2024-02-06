# JSON-C tutorial

This repo contains a very brief and to the point tutorial of the  json-c library, a library for managing (i.e., reading, parsing, creating) JSON files in C code.  The intent is to demonstrate the basic functions and use of the library in a short, easy to consume matter.

The JSON-C library is an interface to process JSON data.  Note that there are many, many other tools (e.g., the most popular at the moment being jq) for managing JSON, and many languages provide extensive libraries and support for its manipulation.  The library described here is specifically for the C/C++ language, which is generally used for programming systems.  Unless you do something silly, the resulting code will be blazing fast and not take up a lot of memory.  That being said, unless you need to do something in C (for a system for example), then the other tools may be more appropriate.

This tutorial was created from the library documentation and random sources on the Internet as well as personal experimentation and trial and error.  Thanks to the many folks who contributed to the libjson-c library and the numerous helpful documents, pages and posts.

**Patick McDaniel (mcdaniel@cs.wisc.edu), Jan 2024**

## JSON Format

Before getting to the use of the library, lets do a quick review of JSON.  JSON is a general purpose data format widely used in modern computing systems, and now is used for everything from configurations to databases to AI datasets and much more.  The basic unit of data in JSON is a key/value pair, 

    "key":"value",

where the key is a string.  The value can be null, string, a number, a boolean, an array or and object.  There are two kinds of containers in JSON, arrays and objects.  Arrays are lists of objects that are in a total order, and an object is a list of key/value pairs.  Note that objects can contain other objects and arrays, and arrays can contain other arrays and objects, etc.  Put more simply, objects and arrays can be arbitrarily nested.

    {
        "firstname": "Patrick",
        "lastname": "McDaniel",
        "class": "cse311",
        "exam-dates": [
            { "first": 511 },
            { "second": true },
            { "third": 0.99 }
        ],
        "student-ids" [
            10,
            20,
            30,
            40
        ]
    }

You can see here by this example that arrays contain lists of items which may either be base types (int, float, string, ...) or objects.  Also note that the last element of an array cannot have a trailing comma (I suppose to make parsing easier).

Note that JSON, oddly, does not support comments (I suppose this is to save space and make parsing easier).  However, there is a convention that you can annotate an object with an comment by creating adding an additional field with the key "_comment".  For example, 

    {
        "_comment":"This is the comment for my object",
        "key1": "Field data one",
        "key2": "Field data two".
        ...
    }

## Building with json-c

To build, you need to include the base library, as well as any libraries you want to use for your application.  For the functions included in this tutorial, you should use:

    #include <json-c/json.h>

To link, you will need to add the link directives associated with the libraries.  For example.

    gcc -o jtest jtest.o -ljson-c

## Basic concepts

The central data structure/object around which JSON is programmed is the json_object.  Most of the time you have a root object from which all of the child objects are referenced, added to, etc.

## JSON parsing, searching, and extracting values

## JSON creation functions

The way you construct an JSON file is to start with a root object, then add all of the elements, sometimes recursively as needed.  The cycle of calls is pretty straightforward

1. **json_object_new_object** - this creates a base object that you can use for filling out the JSON (i.e., acts as a root object).

        json_object *obj;
        obj = json_object_new_object();

2. **json_object_new_string()/json_object_new_int()/...** - These functions create new objects of a particular type.

        json_object *sobj, *iobj, *aobj;
        sobj = json_object_new_string("Patrick");
        iobj = json_object_new_int(412);
        aobj = json_object_new_array();

    Note that in addition to string and int, there are new types for array, boolean, int64, double, and null.

3. **json_object_object_add()/json_object_array_add()** - These functions add new child objects to an existing object.  In the case of the array, it just adds another element to the array.

        json_object_object_add(obj, "key1", sobj); // Adds previously created string obj
        json_object_object_add(obj, "key2", iobj); // Adds previously created int obj
        for (i=0; i<5; i++) {
            json_object_array_add(aobj, json_object_new_int(i*7)); // Add array element
        }

    One thing to be super careful with is the name of the key.  If an name is reused within an object, the last add will overwrite any previous ones.  
    


----





The primary type in json-c is json_object. It describes a reference counted tree of json objects which are created by either parsing text with a json_tokener (i.e. json_tokener_parse_ex()), or by creating (with json_object_new_object(), json_object_new_int(), etc...) and adding (with json_object_object_add(), json_object_array_add(), etc...) them individually. Typically, every object in the tree will have one reference, from its parent. When you are done with the tree of objects, you call json_object_put() on just the root object to free it, which recurses down through any child objects calling json_object_put() on each one of those in turn.

You can get a reference to a single child (json_object_object_get() or json_object_array_get_idx()) and use that object as long as its parent is valid.
If you need a child object to live longer than its parent, you can increment the child's refcount (json_object_get()) to allow it to survive the parent being freed or it being removed from its parent (json_object_object_del() or json_object_array_del_idx())

When parsing text, the json_tokener object is independent from the json_object that it returns. It can be allocated (json_tokener_new()) used one or multiple times (json_tokener_parse_ex(), and freed (json_tokener_free()) while the json_object objects live on.

A json_object tree can be serialized back into a string with json_object_to_json_string_ext(). The string that is returned is only valid until the next "to_json_string" call on that same object. Also, it is freed when the json_object is freed.