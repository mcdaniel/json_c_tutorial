////////////////////////////////////////////////////////////////////////////////
//
//  File          : ctest.cpp
//  Description   : This is a set of test code for the json library.
//
//   Author        : Patrick McDaniel
//   Last Modified : Tue Feb  6 13:41:44 EST 2024
//

// Include Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <json-c/json.h>

//
// Global test data, defines

const char *JSON_EXAMPLES[2] = { "{ \
        \"firstname\": \"Patrick\", \
        \"lastname\": \"McDaniel\", \
        \"class\": \"cse311\", \
        \"exam-dates\": [ \
            { \"first\": 511 }, \
            { \"second\": true }, \
            { \"third\": 0.99 }, \
        ], \
        \"student-ids\": [ \
            10, \
            20, \
            30, \
            40, \
        ], \
    }", 
    "{ \"broken\": \"json\", \"badsyntax\" }" };

#define filename "example1.json"

//
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : json_create
// Description  : This function demostrate the creation of a JSON object
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int json_create( void ) {

    // Local variables
    json_object *obj, *sobj, *iobj, *aobj, *a2obj, *tobj;
    char tmpstr[128];
    int i;

    // Setup a basic JSON object
    obj = json_object_new_object();
    sobj = json_object_new_string("Patrick");
    iobj = json_object_new_int(412);
    aobj = json_object_new_array();
    json_object_object_add(obj, "key1", sobj); // Adds previously created string obj
    json_object_object_add(obj, "key2", iobj); // Adds previously created int obj

    // Create a basic array
    for (i=0; i<5; i++) {
        json_object_array_add(aobj, json_object_new_int(i*7)); // Add array element
    }
    json_object_object_add(obj, "key3", aobj); // Adds previously created array obj

    // Now create a more complex array
    a2obj = json_object_new_array();
    for (i=0; i<5; i++) {
        snprintf(tmpstr, 128, "key3-%d", i);
        tobj = json_object_new_object();
        json_object_object_add(tobj, tmpstr, json_object_new_int(i*7)); 
        json_object_array_add(a2obj, tobj); // Add array element
    }
    json_object_object_add(obj, "key4", a2obj); // Adds complex array

    // Print out the new object
    printf("Created JSON: %s\n", json_object_to_json_string(obj));
    json_object_put(obj); // Clean up the object

    // Return successfully
    return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : json_parse
// Description  : This function demostrate the parsing of a JSON string
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int json_parse( void ) {

    // Local variables
    json_object *jobj;
    json_tokener *tok;
    enum json_tokener_error jerr;
    int i;

    // Parse the JSON string (easy version)
    if ((jobj = json_tokener_parse(JSON_EXAMPLES[0])) == NULL) {
       //  json_tokener_get_error
        printf("Error parsing JSON string.\n\n %s\n", JSON_EXAMPLES[0]);
        return(-1);
    }
    printf("Unparsed JSON: %s\n", JSON_EXAMPLES[0]);
    printf("Parsed JSON: %s\n", json_object_to_json_string(jobj));    

    // Walk through the JSON objects (harder version)
    for (i=0; i<2; i++) {
        // Parse using tokenizer
        tok = json_tokener_new();
        jobj = json_tokener_parse_ex(tok, JSON_EXAMPLES[i], strlen(JSON_EXAMPLES[i]));
        if (jobj == NULL) {
                jerr = json_tokener_get_error(tok);
                printf("Error parsing JSON string (errno %d),\n%s\n\n", jerr, json_tokener_error_desc(jerr));
                json_tokener_free(tok);
                json_object_put(jobj); // Clean up the object
                return(-1);
        }
        printf("Tokenizer unparsed JSON: %s\n", JSON_EXAMPLES[i]);
        printf("Tokenizer parsed JSON: %s\n", json_object_to_json_string(jobj));
        printf("Tokenizer parsed JSON (pretty): %s\n", 
                json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PRETTY));
        json_tokener_free(tok);
        json_object_put(jobj); // Clean up the object
    }

    // Return successfully
    return(0);
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : json_process_object
// Description  : Test search, add, delete, reference counting
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int json_process_object( void ) {

    // Local variables
    json_object *jobj, *tobj, *aobj;
    int i;

    // Parse the JSON string (easy version)
    if ((jobj = json_tokener_parse(JSON_EXAMPLES[0])) == NULL) {
       //  json_tokener_get_error
        printf("Error parsing JSON string.\n\n %s\n", JSON_EXAMPLES[0]);
        return(-1);
    }

    // Grab a couple of objects
    if ( ((tobj = json_object_object_get(jobj, "student-ids")) == NULL) ||
          ((aobj = json_object_array_get_idx(tobj, 2)) == NULL) ) {
        printf("Error getting object from JSON.\n");
        return(-1);
    } 
    printf("Got array object: %s\n", json_object_to_json_string(tobj));
    printf("Got array element object: %s\n", json_object_to_json_string(aobj));
    printf("\n");

    // Note the funky syntax (key and value are defined within macro)
    printf("Walking object ...\n");
    json_object_object_foreach(jobj, key, value) {
        printf("Found key: %s, value: %s\n", key, json_object_to_json_string(value));
    }
    printf("\n");

    // Walk the array and print out the elements
    printf("Walking array ...\n");
    int len = json_object_array_length(tobj);
    for (i=0; i<len; i++) {
        aobj = json_object_array_get_idx(tobj, i);
        printf("Array element %d: %s\n", i, json_object_to_json_string(aobj));
    }
    printf("\n");

    // Add a new object to the array
    json_object_put(jobj); // Clean up the object
    return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : json_file_operations
// Description  : This function demostrates file operations
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int json_file_operations( void ) {

    // Local variables
    json_object *jobj;

    // Read the file into a string
    if ((jobj = json_object_from_file(filename)) == NULL) {
        printf("Error reading JSON from file [%s].\n", filename);
        return(-1);
    }
    printf("Parsed JSON file: \n%s\n\n", json_object_to_json_string(jobj));

    // Write object into another file
    if (json_object_to_file_ext("output.json", jobj, JSON_C_TO_STRING_PRETTY) == -1) {
        printf("Error writing JSON to file.\n");
        return(-1);
    }
    
    // Cleanup, return successfully
    json_object_put(jobj); // Clean up the object
    return(0);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : main
// Description  : The main function for the PW database manager
//
// Inputs       : none
// Outputs      : 0 if successful test, -1 if failure

int main( void ) {

    // Local variables
    json_object *jobj;

    // Do the JSON tests/demonstrations
    json_create();
    json_parse();
    json_process_object();
    json_file_operations();

    // Return successfully
    printf("JTest: successful.\n");
    return(0);
}