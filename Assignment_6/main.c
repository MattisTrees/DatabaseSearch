#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "db_helper.h"


/***************************************************************************
  Instructions:
     1. You can choose either 1a or 1b. You do not to need to do both,
        just one. 
           a. Look at all accounts and determine which have access
	      to which resources.
	   b. Have your program read a person's TUid from STDIN
	      (i.e. accept it as a command line argument) and then
              determine the resources to which that TUid has access.

        Note that if someone has an expiration date that is in the
        past for a role in the person_roles table, you can stop any
        further processing for that role since it is no longer
        applicable.

        However, be careful. Make sure that you keep in mind that just
        because an individual has an expired role, it does not mean that
        all of their roles have expired. The unique combination here
        is the role_id in conjunction with the TUid, not just the TUid. 
        
	To help you get the data that you need from the database
	files, I've included in db_helper.c a function called
	getValue. Please see that file for a description of the
	function parameters and the details of the actions
	performed.

	If you decide to read a person's TUid from STDIN rather than
	evaluating all accounts, you will need to include a check that
	makes sure the TUid entered is valid. Valid TUids are 9
	characters long and may contain only alpha and numeric
	characters.

     2. In the previous step, you determined whether or not either the
	TUid entered or each TUid in the the list of all TUids in the
	database is entitled to each resource. Now you will need to
	log this information in either a log file or in a table in the
	database. Whichever you choose to do, all I am requiring you
	to log is the the list of TUid and resource ids for the
	resources to which they are entitled. I have given you the
	database table, named person_resource. 

	************************************************************
	***************** #3 is no longer required *****************
	************************************************************
     3. Lastly, for each resource, "send" to the resource a list of
	all entitled TUids. By "send", I mean write all of these 
        TUids to the file named after that resource (tumail, canvas,
	ldap). You should write one TUid per line.
	************************************************************
	************************************************************

     Additional Requirements:
        a. You are to create a new c source file (feel free to name it
           anything you'd like and you'll want to remember to include
	   db_helper.h) and include any functions that you write to
	   implement the functionality discussed above. Please attempt
	   to write modular code and do not just write a long stream of
	   code in the main function. You may add to the file
	   db_helper.c if you are implementing one of the functions I
	   defined in db_helper.h. Please note that you only need to
	   implement the functions that you find you need for the
	   assignment. Any others I defined in that header file do not
	   need to be implemented. I included way more than you will
	   need, just to give you a jump start if you wanted to look
	   further into it than is required.
	b. Use this main.c file and the main function only to test your
           code. Again, all of the actual functionality should be
	   written in a separate .c file (optionally include a .h with
	   all of your function declarations and global variable
	   declarations, if applicable). I've included one test to
	   grab the resource_id for the tumail resource. You should
	   include additional tests to demonstrate that your code
	   works end-to-end.

     Compiling your project:
        cd /path/to/extracted/project/files/
	gcc -o myProjectName db_helper.c yourFileName.c main.c

     BONUS POINTS (up to 25 possible):
	It is possible for you to earn bonus points by going beyond
	the basic requirements of this assignment. Some additional
	things you could do to earn bonus points are described below. 

	You can choose to do as many or as few of the below as
	you'd like. Also, if there is anything not described below
	that you would like to implement in your program that goes
	beyond the basic requirements, I am open to the possibility of
	awarding bonus points for this as well

        For anything beyond what's below, I recommend that you come
	speak with me or your TA in person about it prior to
	submitting the assignment or that at the very least you write
	both myself and your TA an email in which you should briefly
	describe the additional piece that you have added. Otherwise,
	I cannot guarantee that we will catch it when we grade your
	submission and award bonus points for it.
	   
	Optional functionality:
	   a. Instead of just assuming that the TUid is the id that the
	      resource accepts, look in the table resource_requirements
	      to see whether it wants the TUid or the AccessNet and
	      then send whichever each resource requires.
	   b. In addition to sending the person's id (i.e. the person's
	      TUid or AccessNet) also send the person's name. It is
	      okay to assume that either all accept preferred name or
	      all accept full name (first, middle, and last).
	   c. In addition to b, you may also choose to look in the
	      table resource_requirements to see whether or not the
	      resource wants the full name or the preferred name and
	      then send whichever is required by each resource.
           d. Rewrite/revise any number of the functions that I provided to
	      you. These were written with the sole purpose of
	      demonstrating specific concepts, which meant that I
	      needed to sacrifice efficiency and better organization of the
	      code in some places.
***************************************************************************/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//  ALL METHODS STARTING WITH "return" ARE SPECIAL CASES OF THE getValue METHOD
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

char **returnPersonId(char* id){
    return getValue("person_ids", id, 0, 1);
} // end return Name

char **returnRoleName(char* roleNum){
    return getValue("roles", roleNum, 0, 1);
} // end returnRoleName

char **returnPersonRoles(char* id){
    return getValue("person_roles", id, 0, 1);
} // end returnPersonRoles

char **returnExpDate(char* id){
    return getValue("person_roles", id, 0, 2);
}// end returnExpdate

char **returnResource(char* validRoleNum){
    return getValue("resources_roles", validRoleNum, 1, 0);
} // end returnResource

char **findPersonName(char* id){
    // *** NOT USED *** NOT USED *** NOT USED ***
    // *** NOT USED *** NOT USED *** NOT USED ***
    // ******* I'M NOT EVEN SURE IT WORKS *******
    
    char **fullname = malloc(5*(30*sizeof(char)));
    char **answer = malloc(4*(30*sizeof(char)));
    
    for(int i = 1; i < 5; i++){
        answer = getValue("person_names", id, 0, i);
        
        printf("answer[0]: %s", answer[0]);
        fullname[i-1] = answer[0];
    } // end for loop
    
    printf("\nName: ");
    
    for(int i = 0; i < 4; i++){
        printf("%s ", fullname[i]);
    } // end for loop
    printf("\n");
    
    return answer;
} // end findPersonName

int checkForID(char* id){
    // This method checks the person_names table and checks to see if it is
    // identical to the user input, if it is, return 1
    char** idFound = getValue("person_names",id, 0, 0);
    int found = 0;
    
    if(strcmp(id, idFound[0]) == 0)
        found = 1;
    
    return found;
} // end checkForID

char* getID(){
    // This method takes the user input and checks it for a valid TUID
    
    char *id = malloc(20 * sizeof(char));
    
    printf("Please Enter a TUID: ");
    fflush(stdout);
    scanf("%s", id);

    int foundSymbol = 0;
    int foundID = 0;
    int numCount = 0;

    // Initialize flags to check for input correctness
    for(int i=0;id[i] != '\0'; i++){
        if(isalpha(id[i]) || isdigit(id[i]))
            numCount++;
        else
            foundSymbol = 1;
    } // end while loop

    if(numCount == 9 && foundSymbol == 0){
        // check to see if entered TUID is actually in database
        foundID = checkForID(id);
    }else if(numCount != 9){
        printf("Please enter and id with 9 digits!\n");
        fflush(stdout);
        id = getID();
    }else if(foundSymbol == 1){
        printf("That is not a valid temple id, no symbols!\n");
        fflush(stdout);
        id = getID();
    } else {
        printf("If you see this then something is seriously wrong!");
        fflush(stdout);
    }// end if/else
    
     if(foundID == 0){
        printf("Id not found, please enter a Valid id (check the database)\n");
        fflush(stdout);
        id = getID();
    } // end if
    
    return id;
} // end getID method

int * isExpired(char* id){
    // This block of code grabs the current date using the time.h library
    // and the parses the time into an int that we can do math on
    char *currentDate = malloc(10 * sizeof(char));
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(currentDate, sizeof(currentDate)+1, "%Y%m%d", t);
    int *todayDate = malloc(9 * sizeof(int));
    sscanf(currentDate, "%d", todayDate);
    
    int counter;
    
    // get array with the exp dates
    char ** expDates = returnExpDate(id);
    
    
    // array to hold true or false values for role expiry
    int *expired = malloc(5 * sizeof(int));
    
    for(int i=0; expDates[i] != NULL; i++){
        int expDateInt;
        expDateInt = atoi(expDates[i]);
        
            if(*todayDate < expDateInt){
                expired[i] = 0; // false (not expired)
            } else{
                expired[i] = 1; // true (is expired)
            } // end if/else
        counter = i;
    } // end while loop
    
    return expired;
} // end isExpired

int counter(char* id){
    // this function counts how many roles an individual's ID has,
    // it only counts the lines that are returned by the getValue function
    int i;
    // get array with the exp dates
    char ** expDates = returnExpDate(id);
    
    for(i=0; expDates[i] != NULL; i++){
    } // end loop that does nothing
    
    return i;
} // end isExpCounter

char ** whichRoles(char* id){
    // This methods returns an array of Strings showing which roll numbers are
    // not expired for the user ID input
    
    // String array of all possible roles for this id
    char **roles = returnPersonRoles(id);
    // String array to hold available role numbers
    char **availRoles;
    availRoles = (char**) malloc(5 * sizeof(char*));
    int* expArray;
    expArray = isExpired(id);
    
    // counts the number of roles that have been put into availRoles so
    // the correct index is used during the for loop
    int roleCount = 0;
    
    for(int j=0; j < counter(id); j++){
        if(expArray[j] == 0){
            availRoles[roleCount] = roles[j];
            roleCount++;
        } // end if
    } // end for loop
    
    return availRoles;
} // end whichRoles

char ** roleMap(char** validRoleNums){
    // this function takes the array of unexpired roll NUMBERS and 
    // returns an array of role names that the user can use
    // String array to hold all valid roles for a specific ID
    char **roles = malloc(7 * sizeof(char*));
    
    // for every available role number, return a string mapped to that role name
    for(int i=0; validRoleNums[i] != NULL; i++){
        roles[i] = *returnRoleName(validRoleNums[i]);
    } // end for loop
    
    return roles;
} // end roleMap

char ** resourceMap(char** validRoleNums){
    // This function takes the array of valid role NUMBERS for the id and
    // maps them to which resource the user has access to
    
    char **resources = malloc(3 * sizeof(char*));
    
    for(int i=0; validRoleNums[i] != NULL; i++){
        
        char** resource = returnResource(validRoleNums[i]);
        
        for(int j=0; resource[j]!=NULL; j++){
            
            if(strcmp(resource[j], "1") == 0){
                resources[0] = "tumail";
            }else if(strcmp(resource[j], "2") == 0){
                resources[1] = "canvas";
            }else if(strcmp(resource[j], "3") == 0){
                resources[2] = "ldap";
                
            } // end else/if branching
            
        } // end nested for loop
        
    } // end for loop
    
    return resources;
} // end resourceMap

void writeToFile(char *id, char* resource){
    
    FILE *file;
    file = fopen("person_resource", "a");
    fprintf(file,"\n%s%s%s", id, "#", resource);
    fclose(file);
    
    return;
} // end openFile

int main(int argc, char **argv) {
    
    // take in user input and check for correctness
    char* id = getID();
    
    // find out which role numbers are valid for this id
    printf("\nChecking for valid role numbers... ");
    char **validRoleNums = whichRoles(id);
    printf("Done!"); fflush(stdout);
    
    // find out which role names are associated with this list of role numbers
    printf("\nMapping roles to role numbers... ");
    char **validRoles = roleMap(validRoleNums);
    printf("Done!"); fflush(stdout);
    
    // find out which resources are available for the roles found
    printf("\nMapping resources to role numbers... ");
    char **availResources = resourceMap(validRoleNums);
    printf("Done!"); fflush(stdout);
    
    printf("\n\n************************\n  BACK IN MAIN METHOD\n***********************\n");
    fflush(stdout);
    
    printf("\nTUID: %s", id);
    fflush(stdout);
    printf("\nRoles: ");
    int i;
    for(i=0; validRoles[i]!=NULL; i++){
        printf("%s ", validRoles[i]);
    } // end loop
    printf("\nAvailable resources: %s, %s, %s\n", availResources[0],availResources[1],availResources[2]);
    fflush(stdout);
    
    // write to the file "person_resource"
    printf("\nWriting to file... ");
    if(availResources[0] != NULL)
        writeToFile(id, availResources[0]);
    if(availResources[1] != NULL)
        writeToFile(id, availResources[1]);
    if(availResources[2] != NULL)
        writeToFile(id, availResources[2]);
    printf("Done!");
    
    return 0;
} // end main

