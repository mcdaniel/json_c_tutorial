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
#include <json-c/json.h>

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

    // Define the JSON string
    const char *js_str = "{ \
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
    }";

    // Parse the JSON string
    if ((jobj = json_tokener_parse(js_str)) == NULL) {
        printf("Error parsing JSON string.\n\n %s\n", js_str);
        return(-1);
    }
    printf("Unparsed JSON: %s\n", js_str);
    printf("Parsed JSON: %s\n", json_object_to_json_string(jobj));    

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

    // Do the JSON tests/demonstrations
    json_create();
    json_parse();

    // Return successfully
    printf("JTest: successful.\n");
    return(0);
}