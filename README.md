# JSON-C tutorial

This repo contains a very brief and to the point tutorial of the  json-c library, a library for managing (i.e., reading, parsing, creating) JSON files in C code.  The intent is to demonstrate the basic functions and use of the library in a short, easy to consume matter.  I am going to assume you know C, but very little about JSON or how it is generally used.

The JSON-C library is an interface to process JSON data.  Note that there are many, many other tools (e.g., the most popular at the moment being jq) for managing JSON, and many languages provide extensive libraries and support for its manipulation.  The library described here is specifically for the C/C++ language, which is generally used for programming systems.  Unless you do something silly, the resulting code will be blazing fast and not take up a lot of memory.  That being said, unless you need to do something in C (for a system for example), then the other tools may be more appropriate.

Note that there are many other functions that are part of the library that I am not covering (the goal here is to give you the basics so you can create code for 90% of the cases).  I refer you the [JSON-C github repo](https://github.com/json-c) and its documentation [here](https://json-c.github.io/json-c/json-c-0.16/doc/html/files.html) (particularly [json_object.h](https://json-c.github.io/json-c/json-c-0.16/doc/html/json__object_8h.html)) for more information.  In some cases, you will need to read the .h files for more information.

This tutorial was created from the library documentation and random sources on the Internet as well as personal experimentation and trial and error.  Thanks to the many folks who contributed to the libjson-c library and the numerous helpful documents, pages and posts.

**Patick McDaniel (mcdaniel@cs.wisc.edu), Feb 2024**

## JSON Format

Before getting to the use of the library, lets do a quick review of JSON.  JSON is short for JavaScript Object Notation, which as you can see is in reference to the JavaScript language from which it was lifted -- in fact, the approach was so simple and useful it was widely adopted any number of applications and uses across the Internet and beyond.  In short, JSON has become a standard data format that all computer scientists and serious developers should be aware of.

To the point, JSON is a general purpose data format widely used in modern computing systems based entirely on the ASCII character set (although there is support for extended character sets for internationalization), and now is used for everything from configurations to databases to AI datasets and much more.  The basic unit of data in JSON is a key/value pair, 

    { "key":"value" }

where the key is a string.  The value can be null, string, a number, a boolean, an array or and object.  Note that braces "{" and "}" serve as annotations to define the scope of an object.  In this case, there is just one key/value pair, but objects can be much more complex when they act as containers.

There are two kinds of containers in JSON, arrays and the objects themeselves.  Arrays are lists of objects that are in a total order, and an object is a list of key/value pairs.  Note that objects can contain other objects and arrays, and arrays can contain other arrays and objects, etc.  Put more simply, objects and arrays can be arbitrarily nested.  For example,

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

You can see here by this example that arrays contain lists of items which may either be base types (int, float, string, ...) or objects.  Also note that the last element of an array cannot have a trailing comma (I suppose to make parsing easier).  If you inadvertently add the comment, the parser will error when trying to parse the text.

Note that JSON, oddly, does not support comments (I suppose this is to save space and make parsing easier).  However, there is a convention that you can annotate an object with an comment by creating adding an additional object with the key "_comment".  For example, 

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

The central data structure/object around which JSON is programmed is the json_object.  Most of the time you have a root object from which all of the child objects are referenced, added to, etc.  Those of you who are familiar with other general-purpose data formats like XML will recognize this as the _root_ element.  The life-cycle of the process is that you (a) either create the objects by parsing a string/file containing JSON or you construct one by adding basic data typ0e elements to it, (b) walk the elements and add/edit/extract data from the JSON tree, (c) serialize (or re-serialize) the JSON object, then free the objects (which can be done recursively).

One of the cool things about JSON-C is how it handles memory management.  The idea is that you can parse/allocate a huge tree ending up with a bunch of parent/child relationships.  However, when you free the tree it frees all of the child nodes automatically.  This avoids a lot of complexity in freeing code and can prevent memory leaks.  Of course, if you don't want it to delete a particular child node, you can prevent it from doing so (see [refcount](#reference-counting-and-memory-management) discussion below).

## JSON parsing and serializing

Parsing JSON data is a pretty straightforward affair.  It works by passing some descriptor to the approporiate parse function and it returns the object tree or NULL if it failed.

1. **json_tokener_parse()** - this function takes a string containing the JSON text and returns either the tree or NULL if it fails.

        // Parse the JSON string
        json_object *jobj;
        if ((jobj = json_tokener_parse(js_str)) == NULL) {
            printf("Error parsing JSON string.\n\n %s\n", js_str);
            return(-1);
        }

2. **json_tokener_new()/json_tokener_parse_ex()/json_tokener_free()** - This is the alternate way to process JSON, which has a lot more flexiblity (and complexity).  The idea is that you create an tokenizer object and repeatedly call it to parse the object is a stepwise way.

        tok = json_tokener_new();
        jobj = json_tokener_parse_ex(tok, js_str, strlen(js_str));
        if (jobj == NULL) {
            printf("Error parsing JSON string.\n\n %s\n", js_str);
            return(-1);
        }
        json_tokener_free(tok);

    One might wonder why you would go to the extra trouble of using this over the parser above.  The trick is that the _ex function actually can process fragments of JSON passed as strings.  In this way, you can control the parsing yourself and handle the processing of the objects as they are created in a way that suits you.

3. **json_tokener_get_error()/json_tokener_error_desc()** - These functions allow you to identify parsing errors and get descriptions of the errors you encountered.

        enum json_tokener_error jerr;
        if ((jobj = json_tokener_parse(js_str)) == NULL) {
            jerr = json_tokener_get_error();
            return(-1);
        }

5. **json_object_to_json_string()/json_object_to_json_string_ext_()** - The simplest approach to serialization is to simply create strings from objects.  These calls return a string which is an ASCII representation of the JSON object.  Note that the string returned is connected to the object from whence it can -- and that gets cleaned up when the object is freed itself later.  The easiest form is to just call it on an object:

        printf("JSON object: \n%s\n\n", json_object_to_json_string(jobj));

    What you get back is a compact version of the object which works but is not very human friendly.  The library provides an additional methods which you can print things out in "pretty form", which simply uses flags to indicate how to do that with the _ext version of the functions.

        printf("Tokenizer parsed JSON (pretty): %s\n", 
                json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PRETTY));

4. **json_object_from_file()/json_object_to_file()/json_object_to_file_ext()** - These functions simplify the whole parsing and output functions by just passing a filename to retreive or serialize the data to.

        // Read the file into a string
        if ((jobj = json_object_from_file(filename)) == NULL) {
            printf("Error reading JSON from file [%s].\n", filename);
            return(-1);
        }

        // Write object into another file
        if (json_object_to_file_ext("output.json", jobj, JSON_C_TO_STRING_PRETTY) == -1) {
            printf("Error writing JSON to file.\n");
            return(-1);
        }

## JSON object creation

The way you construct an JSON file is to start with a root object, then add all of the elements, sometimes recursively as needed.  The cycle of calls is pretty straightforward, where you create, add, ...

1. **json_object_new_object** - this creates a base object that you can use for filling out the JSON (i.e., acts as a root object).

        json_object *obj;
        obj = json_object_new_object();

2. **json_object_new_string()/json_object_new_int()/...** - These functions create new objects of a particular type.

        json_object *sobj, *iobj, *aobj;
        sobj = json_object_new_string("Patrick");
        iobj = json_object_new_int(412);
        aobj = json_object_new_array();

    Note that in addition to string and int, there are new types for array, boolean, int64, uint64, double, and null.

3. **json_object_object_add()/json_object_array_add()** - These functions add new child objects to an existing object.  In the case of the array, it just adds another element to the array.

        json_object_object_add(obj, "key1", sobj); // Adds previously created string obj
        json_object_object_add(obj, "key2", iobj); // Adds previously created int obj
        for (i=0; i<5; i++) {
            json_object_array_add(aobj, json_object_new_int(i*7)); // Add array element
        }

    One thing to be super careful with is the name of the key.  If an name is reused within an object, the last add will overwrite any previous ones.  

## Searching, traversing and extracting JSON data

1. **json_object_object_get()** - get an object by its key name from an object.  It returns NULL if it can't find the element in the JSON structure.

        tobj = json_object_object_get(jobj, "student-ids");

2. **json_object_array_get_idx()** -  This gets an element out of an array from a particular index.  Remember that JSON is zero-indexed, like most languages.

        aobj = json_object_array_get_idx(tobj, 2);  // Get third element

3. **json_object_object_foreach()** - This is a macro that allows you to walk an object, grabbing fields, etc.  Note that the some kinds of live modification of the traversed object are allowed while walking the fields, some are not.  Be safe, don't modify an object you are traverseing.

        // Note the funky syntax (key and value are defined within macro)
        json_object_object_foreach(jobj, key, value) {
            printf("Found key: %s, value: %s\n", key, json_object_to_json_string(value));
        }

    There is some hidden stuff going on here.  This macro expands into a for loop with the variable key and value defined in scope, where key is of typee char * and value is a json_object *.  I orginally tried to defined before I figured that out.

4. **json_object_array_length()** - Traversing an array is a bit easier; you simply use a normal for loop on the length of the array.  

        int len = json_object_array_length(obj);
        for (i=0; i<len; i++) {
             json_object_array_get_idx(obj, i);
        }

5. **json_object_get_boolean()/json_object_get_int()/json_object_get_int64()/json_object_get_uint64()...** - Once you have an object, you can extract its type sensitive value from the string.  Here the value portion of the key/value part is coerced into the type of function you are interested in, i.e., the text is converted to whatever type you want, if it can be "3.14" would be converted into the float 3.14.

        uint32_t val = json_get_object_int(i32obj);
        const char *str = json_object_get_string(obj);

    The supported types are: array, boolean, int, int64, uint64, double, and string, and null.

## Reference counting and memory management

As mentioned above, one of the cool features of the libarary is that the objects are memory referenced, meaning that once the last reference to an object is deleted, the underlying JSON object is freed.  This prevents memory leasks and makes the code much cleaner (you don't have to do complex recursive frees with the ubqitous hanging pointer problems that inevidably occur).  There are two basic functions you need to understand to work with the reference counting.

1. **json_object_put()** - This is the free function for the json objects, which does the recursive free for you (unless you have marked it not to delete--see next function).

        json_object *jobj = json_tokener_parse(js_str);
        ...
        json_object_put(jobj); // Free the object


2. **json_object_object_del()/json_object_array_del_idx()** - this removes the object from the JSON tree and deletes it either from an object or an array().  The first one deletes a particular object from another object, and the other removes a range of elements in an array.

        json_object_object_del(jobj, "name"); // Remove name object from field
        json_object_array_del_idx(jobj, 10, 1); // Remove 1 element at index 10

2. **json_object_get()** - these functions get a reference to an object.  However, in in addition to getting an object, it increases the reference count to the object.  In a sense (and as stated in the documentation), you take ownership of the object when you make this call.

        json_object *jobj = json_object_get(obj); // Take ownership of object

