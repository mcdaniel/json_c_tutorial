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
// Functions

////////////////////////////////////////////////////////////////////////////////
//
// Function     : json_from_file
// Description  : This function demostrates the creation of a JSON object 
//                from a file
//
// Inputs       : filename - the name of the file to read
// Outputs      : 0 if successful test, -1 if failure

json_object * json_from_file( const char *filename ) {

    // Local variables
    FILE *fp;
    char *js_str;
    json_object *jobj;
    struct stat st;

    // Get the size of the file using stat
    if (stat(filename, &st) != 0) {
        printf("Error getting file size [%s].\n", filename);
        return(NULL);
    }
    js_str = (char *)malloc(st.st_size+1);
    js_str[st.st_size] = '\0';

    // Open the file (assume this is an ASCII file, which it has to be for JSON)
    if ( (fp = fopen(filename, "r")) == NULL ) {
        printf("Error opening file [%s], %s.\n", filename, strerror(errno));
        free(js_str);
        return(NULL);
    }

    // Read the file into the string, close file
    if (fread(js_str, st.st_size, 1, fp) != 1) {
        printf("Error reading file [%s], incorrect read size.\n", filename);
        free(js_str);
        fclose(fp);
        return(NULL);
    }
    fclose(fp);

    // Parse the JSON string
    if ((jobj = json_tokener_parse(js_str)) == NULL) {
        printf("Error parsing JSON string.\n\n %s\n", js_str);
        free(js_str);
        return(NULL);
    }
    printf("Unparsed JSON file: \n%s\n\n", js_str);
    printf("Parsed JSON file: \n%s\n\n", json_object_to_json_string(jobj));
    free(js_str);

    // Return successfully
    return(jobj);
}

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

    // Define the JSON string (second one will error on purpose)
    const char *js_str[2] = { "{ \
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

    // Parse the JSON string (easy version)
    if ((jobj = json_tokener_parse(js_str[0])) == NULL) {
       //  json_tokener_get_error
        printf("Error parsing JSON string.\n\n %s\n", js_str[0]);
        return(-1);
    }
    printf("Unparsed JSON: %s\n", js_str[0]);
    printf("Parsed JSON: %s\n", json_object_to_json_string(jobj));    

    // Walk through the JSON objects (harder version)
    for (i=0; i<2; i++) {
        // Parse using tokenizer
        tok = json_tokener_new();
        jobj = json_tokener_parse_ex(tok, js_str[i], strlen(js_str[i]));
        if (jobj == NULL) {
                jerr = json_tokener_get_error(tok);
                printf("Error parsing JSON string (errno %d),\n%s\n\n", jerr, json_tokener_error_desc(jerr));
                json_tokener_free(tok);
                json_object_put(jobj); // Clean up the object
                return(-1);
        }
        printf("Tokenizer unparsed JSON: %s\n", js_str[i]);
        printf("Tokenizer parsed JSON: %s\n", json_object_to_json_string(jobj));
        json_tokener_free(tok);
        json_object_put(jobj); // Clean up the object
    }

    // Return successfully
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

    // Try some utility functions
    if ( (jobj=json_from_file("example1.json")) == NULL ) {
        printf("Error reading JSON from file.\n");
        return(-1);
    }
    json_object_put(jobj); // Clean up the object

    // Return successfully
    printf("JTest: successful.\n");
    return(0);
}