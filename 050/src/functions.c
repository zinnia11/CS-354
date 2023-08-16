/******************************************************************************
 * @file: functions.c
 *
 * zznie
 * Zinnia Nie
 * 908 319 4044
 * OTHER COMMENTS FOR THE GRADER (OPTIONAL)
 *
 * @creator: Zinnia Nie(zznie@wisc.edu)
 * @modified: SUBMISSION DATE
 *****************************************************************************/
#include <stdio.h>
#include "functions.h"

// Some macros that may be useful to you 
#define MAX_USERNAME_LEN    32
#define MAX_EMAIL_LEN       32
#define MAX_DOMAIN_LEN      64
#define MIN_PASSWORD_LEN    8
#define MAX_PASSWORD_LEN    16
#define NEW_LINE_CHAR       10

// Set this to 1 to enable dbgprintf statements, make sure to set it back to 0 
// before submitting!
#define DEBUG               0 
#define dbgprintf(...)      if (DEBUG) { printf(__VA_ARGS__); }

/******************************************************************************
 * Helper functions
 *****************************************************************************/

// ADD ANY HELPER FUNCTIONS YOU MIGHT WRITE HERE 
// Examples: IsLetter, IsDigit, Length, Find...

/*
 * Helper function to replace NEW_LINE_CHAR with '\0'
 * 
 * @param name: character array to search through
 */
void ReplaceNewLine(char name[], int max) {
    char *pname = name;

    for(int i=0; i<max; i++) { 
        if (*pname ==  NEW_LINE_CHAR) {
            *pname = '\0';
            break;
        }
        pname++;
    }
}

/*
 * Helper funtion to check if a given char is a letter A-Z or a-z
 * 
 * @param letter: char that is to be tested
 * @return 1 if the char is a letter, 0 otherwise
 */
int IsLetter(char letter) {
    // A is 65, Z is 90
    // a is 97, z is 122
    // the first index of the username has to be between those two 
    if ((letter>64 && letter<91) || (letter>96 && letter<123)) {
        return 1;
    }
    return 0;
}

/*
 * Helper funtion to check if a given char is a number 0-9
 * 
 * @param number: char that is to be tested
 * @return 1 if the char is a number, 0 otherwise
 */
int IsNumber(char number) {
    // 0 is 48, 9 is 57
    if (number>47 && number<58){
        return 1;
    }
    return 0;
}

/******************************************************************************
 * Verification functions
 *****************************************************************************/

/*
 * A username must begin with a letter [A-Z, a-z], contain 32 characters
 * or less, and  may only consist of letters, underscores, or 
 * digits [A-Z, a-z, _, 0-9]. An error message is displayed if any of 
 * these conditions are not met. Only print the first applicable error 
 * message. 
 *
 * @param user : The username string
 * @param len : Size of the username input buffer
 * @return 1 if valid, 0 if not
 */
int Verify_Username(char user[], size_t len) {

    /* BEGIN MODIFYING CODE HERE */
    dbgprintf("This line only prints if DEBUG is set to 1\n");

    char *puser = user; // pointer to traverse the array
    int length = 0;
    int invalid = 0;

    // check start letter
    if (IsLetter(user[0]) == 0) {
        printf(ERROR_01_USER_START_INVALID);
        return 0;
    }

    // check for invalid chars and count length
    while (*puser !=  '\0' && length <= MAX_USERNAME_LEN) {
        if ((IsLetter(*puser) == 0) && (IsNumber(*puser) == 0) 
            && (*puser != '_')){
            invalid = 1;
        }
        length++;
        puser++;
    }

    if (length > MAX_USERNAME_LEN) {
        printf(ERROR_02_USER_LEN_INVALID);
        return 0;
    }
    if (invalid > 0) {
        printf(ERROR_03_USER_CHARS_INVALID);
        return 0;
    }

    /* END MODIFYING CODE HERE */

    printf(SUCCESS_1_USER);
    return 1;
}

/*
 * An email address has four parts:
 *      name
 *          exists
 *          must start with letter
 *          must be 32 characters or less (inclusive on 32)
 *          may contain only letters and digits
 *      @ symbol
 *          exists
 *      domain name
 *          exists
 *          max of 64 characters
 *          composed of one or more subdomains separated by .
 *          subdomain must begin with a letter
 *          subdomains may contain only letters and digits
 *      top-level domain 
 *          must be [.edu, .com, .net]
 *
 * If the email address contains one or more errors print only the first
 * applicable error from the list.
 *
 * Note this task is based on a real world problem and may not be the best 
 * order to approach writing the code.
 *
 * @param email : The email string
 * @param len : Size of the email input buffer
 * @return 1 if valid, 0 if not
 */
int Verify_Email(char email[], size_t len) {

    /* BEGIN MODIFYING CODE HERE */
    char *pemail = email;
    char *pend;
    int length = 0;
    // act as boolean flags for valid emails
    int invalid = 0;
    int start = 0;
    int top_domain = 0;

    // find the end of the input
    while (*pemail != '\0' && length < (int) (len/sizeof(char))){
        pemail++;
    }
    pend = pemail - 4; // top level domain is the last 4 characters plus the '\0'

    // check top-level domain
    if (*pend != '.') {
        top_domain = 1;
    }
    pend++;
    switch (*pend) {
        case 'n' :
            // increments pemail pointer three spaces if fully completed, else increments until first fail
            if (*(++pend) !=  'e' || *(++pend) !=  't' || *(++pend) != '\0') { 
                top_domain = 1;                               
            }                                                
            break;
        case 'c' :
            if (*(++pend) !=  'o' || *(++pend) !=  'm' || *(++pend) != '\0') {
                top_domain = 1;
            } 
            break;
        case 'e' :
            if (*(++pend) !=  'd' || *(++pend) !=  'u' || *(++pend) != '\0') {
                top_domain = 1;
            } 
            break;
        default :
            top_domain = 1;
    }

    if (top_domain == 0) { // if the top domain is right, omit it from future checks
        pend = pend - 4;
    } else {
        pend = pemail; // otherwise reassign it to point to the end of the email
    }

    pemail = email; // reassign pemail pointer to the beginning of the email

    // check for missing name (when email begins with an @)
    if (email[0] == '@') {
        printf(ERROR_04_EMAIL_MISSING_NAME);  // example @domain.com
        return 0;
    }
    // check for letter starting name
    if (IsLetter(email[0]) == 0) {
        printf(ERROR_05_EMAIL_START_INVALID);
        return 0;
    }
    
    // check the name
    while ((*pemail != '@' && pemail != pend) && length <= MAX_EMAIL_LEN) {
        if (IsLetter(*pemail) == 0 && IsNumber(*pemail) == 0) {
            invalid = 1;
        }
        length++;
        pemail++;
    }

    if (length > MAX_EMAIL_LEN) {
        printf(ERROR_06_EMAIL_NAME_LEN_INVALID);
        return 0;
    }
    if (invalid > 0) {
        printf(ERROR_07_EMAIL_NAME_CHARS_INVALID); // example mike.wisc.edu
        return 0;
    }
    // check for @
    if (*pemail != '@') {
        printf(ERROR_08_EMAIL_MISSING_SYMBOL);
        return 0;
    } 
    // pemail should be pointing at the '@' char

    // check domain name
    length = 0; // reset length
    invalid = 0; // set invalid again just to be sure
    while (pemail != pend && length < (int) (len/sizeof(char))) {
        if (*pemail == '.' || *pemail == '@') { 
            if (IsLetter(*(++pemail)) == 0 && *pemail != '\0') { // check if the subdomain starts with a letter
                start = 1;
            }
        } else { // check if the rest of the subdomain is valid chars
            if (IsLetter(*pemail) == 0 && IsNumber(*pemail) == 0) {
                invalid = 1;
            }
            pemail++;
        }
        length++;
    }

    length--; // remove the length for @
    // after top level domain, see if there is a subdomain
    if (length <= 0) {
        printf(ERROR_09_EMAIL_MISSING_DOMAIN); // example mike@.edu
        return 0;
    }
    if (length > MAX_DOMAIN_LEN) {
        printf(ERROR_10_EMAIL_DOMAIN_LEN_INVALID);
        return 0;
    }
    if (start > 0) {
        printf(ERROR_11_EMAIL_DOMAIN_START_INVALID);
        return 0;
    }   
    if (invalid > 0) {
        printf(ERROR_12_EMAIL_DOMAIN_CHARS_INVALID);
        return 0;
    }   
    if (top_domain > 0) {
        printf(ERROR_13_TOP_LEVEL_DOMAIN_INVALID);
        return 0;
    }
    
    /* END MODIFYING CODE HERE */
    
    printf(SUCCESS_2_EMAIL);
    return 1;
}


/*
 * The following password requirements must be verified:
 *  - May use any character except spaces (i.e., "my password" is invalid)
 *  - Must contain at least 8 characters (i.e., "Password" has 8 characters 
 *    and is valid)
 *  - May have at most 16 characters (i.e., "1234567890Abcdef" has 16 
 *    characters and is valid)
 *  - Must contain at least one upper case character [A-Z]
 *  - Must contain at least one lower case character [a-z]
 *
 * @param pwd : The original password string
 * @param len : Size of the original password input buffer
 * @return 1 if valid, 0 if not
 */
int Verify_Password(char pwd[], size_t len) {

    /* BEGIN MODIFYING CODE HERE */
    char *ppwd = pwd; // pointer to traverse the char array
    int upper = 0; // acts like a boolean flag for checking for uppercase
    int lower = 0; // checking for lowercase
    int length = 0;

    while (*ppwd != '\0' && length <= MAX_PASSWORD_LEN) {
        if (*ppwd == 32) { // 32 is ASCII value for whitespace
            printf(ERROR_14_PWD_SPACES_INVALID);
            return 0;
        }
        if (*ppwd>64 && *ppwd<91) {
            upper = 1; // flag on for uppercase letter
        }
        if (*ppwd>96 && *ppwd<123) {
            lower = 1; // flag on for lowercase letter
        }
        length++;
        ppwd++;
    }

    // check length of password
    if (length < MIN_PASSWORD_LEN) {
        printf(ERROR_15_PWD_MIN_LEN_INVALID);
        return 0;
    }
    if (length > MAX_PASSWORD_LEN) {
        printf(ERROR_16_PWD_MAX_LEN_INVALID);
        return 0;
    }
    
    // check for upper and lower case
    if (upper == 0) {
        printf(ERROR_17_PWD_MIN_UPPER_INVALID);
        return 0;
    }
    if (lower == 0) {
        printf(ERROR_18_PWD_MIN_LOWER_INVALID);
        return 0;
    }
    
    /* END MODIFYING CODE HERE */

    return 1;
}
/*
 * Original Password and the Reentered Password must match
 *
 * @param pwd1 : The original password string
 * @param len1 : Size of the original password input buffer
 * @param pwd2 : The reentered password string
 * @param len2 : Size of the renetered password input buffer
 * @return 1 if valid, 0 if not
 */
int Verify_Passwords_Match(char pwd1[], size_t len1, char pwd2[], size_t len2) {

    /* BEGIN MODIFYING CODE HERE */

    char *ppwd1 = pwd1;
    char *ppwd2 = pwd2;
    int exit = 0;

    for (int i=0; i < (int) (len1/sizeof(char)); i++) {
        if (*ppwd1 == '\0' && *ppwd2 == '\0') {
            break;
        } 
        if (*ppwd1 != *ppwd2) {
            printf(ERROR_19_PWD_MATCH_INVALID);
            return 0;
        }
        ppwd2++;
        ppwd1++;
    }

    /* END MODIFYING CODE HERE */

    printf(SUCCESS_3_PASSWORDS);
    return 1;
}

/******************************************************************************
 * I/O functions
 *****************************************************************************/

/*
 * Prompts user with an input and reads response from stdin
 *
 * @param message : Prompt displayed to the user
 * @param data : char array to hold user repsonse
 * @param MAX_LENGTH : Size of user response input buffer
 */
void Get_User_Data(char *message, char *data, const int MAX_LENGTH) {
    printf("%s", message);
    fgets(data, MAX_LENGTH, stdin);
    /* BEGIN MODIFYING CODE HERE */
    // replace the new line char for every input
    ReplaceNewLine(data, MAX_LENGTH);
    /* END MODIFYING CODE HERE */
    return;
}
