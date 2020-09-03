// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Alg     -   Employe Record INCLUDE
//   Date    -   8-24-20
//
// ########################################## 

#ifndef __EMPLOYEE_RECORD_H__
#define __EMPLOYEE_RECORD_H__

/////////////////////////////////
//  ENUM DEFINITIONS 
//

/*
// @brief Simplifies tracking of employee hire status
*/
enum hireStatus{
    newHire,
    reHired,
    terminated
};

//
/////////////////////////////////
//  STRUCTURE DEFINITIONS 
//

/*
// @brief Stores the day of employment change (hire/terminated)
*/
struct Day  { short day, month, year; };

/*
// @brief Stores the time of employment change (hire/terminated). Defaults to midnight
*/
struct Time { short hour, minute; };

/*
// @brief Stores the date/time of employment change (hire/terminated)
*/
typedef struct {
    struct Day day;                             // Stores the day of employment change (hire/terminated)
    struct Time time;                           // Stores the time of employent change (hire/terminated -- defaults to midnight)
} Date;

/*
// @brief Organizes data for a single employee
*/
typedef struct {
    char id[200];                               // ID of employee. Consists of start date appended to full name (whitespace removed)
    char full_name[100];                        // Full name of employee. Given in file
    char job_title[100];                        // Job title of employee. Given in file

    char dpt_id[20];                            // Department ID of employee. Given in file
    char dpt_descr[100];                        // Deparmtnet description of employee. Given in file

    Date hire_date;                             // Hire date of employee. Given in file
    Date term_date;                             // Termination date of employee. Counted as Jan. 0, 2019 if employee not present in FY2019.
                                                //  Determined by ID. Employee not considered terminated if position changes.

    float net_salary;                           // Net salary of employee. Given in file
    float grs_salary;                           // Gross salary of employe. Determines department budget. Given in file.

    enum hireStatus hire_status;
} Employee;

/*
// @brief Organizes data for a single department
*/
typedef struct {
    char dpt_id[20];                            // ID of department given in file
    char dpt_descr[100];                        // DESCR of department given in file

    float annual_salary_2019;                   // Summation of employee salaries for FY2019
    float annual_salary_2018;                   // Summation of employee salaries for FY2018
    float net_change;                           // FY2019 - FY2018
} Department;

/*
// @brief Compiles information for entire database
*/
typedef struct {
    Employee* employee_list;                    // Master list of employees. Allocated on heap
    Department* dpt_list;                       // Master list of departments. Allocated on heap

    int num_employees, employees_allocated;     // Track number of allocations vs total in use
    int num_dpts, dpts_allocated;               // ""
} Database;

//
/////////////////////////////////
//  FUNCTION DEFINITIONS 
//

//
//------ Functions controlling the database ------ /
//

/*
// @brief Create an employee database from input filesfields[5]
// @param input_files (char[2][100]) A list of filenames to be used as input
// @param numFiles (int) The number of input files
// @param database (Database*)Database to store output
// @return (void)
*/
void createDatabase(char input_files[2][100], int numFiles, Database* database);

/*
// @brief Destroy the entire employee database.
// @param database (Database*) Database to destroy
// @return (void)
*/
void destroyDatabase(Database* database); 

/*
// @brief Add an employee to the database. Allocated on heap.
// @param employee (const Employee*) Temporary allocation to ease passage of data to heap
// @param database (Database*) Database to which employee is added
// @param year (int) Fiscal year of employee tabulation
// @return (unsigned int) -> Number of employees in database
*/
unsigned int addEmployee(const Employee *employee, Database *database, int year);

//
//---------- String processing and conversion ---------- /
//

/*
// @brief Convert Date struct to string
// @param date (const Date*) Date to be converted
// @param str (char*) String to store output
// @return (const char*) -> Converted string (same string as str)
*/
const char* dateToString(const Date *date, char* str);

/*
// @brief Convert Employee struct to string
// @param employee (const Employee*) Employee to be converted
// @param str (char*) String to store output
// @param human_readable (unsigned short) Flag to toggle between output delimeters of columns (1) or tabs (0)
// @return const char* -> Converted string (same string as str)
*/
const char* employeeToString(const Employee *employee, char* str, unsigned short human_readable);

/* 
// @brief Convert Department struct to string
// @param dpt (const Department*) Department to be converted
// @param str (char*) String to store output
// @param human_readable (unsigned short) Flag to toggle between output delimeters of columns (1) or tabs (0)
// @return (const char*) Converted string (same string as str)
*/
const char* dptToString(const Department *dpt, char *str, unsigned short human_readable);

/*
// @brief Remove whitespace from input, return as output
// @param input (const char*) String with whitespace to be removed
// @param output (char*) String to store output
// @return (char*) Processed string (same string as output)
*/
char* removeWhitespace(const char* input, char* output);

//
//---------- Check for entity in list ---------- /
//

/*
// @brief Check if employee is in database
// @param database (const Database*) Database to check against
// @param id (const char*) ID of employee to check
// @return (int) If not found, -1. Else, index of employee in database.
*/
int checkEmployee(const Database* database, const char* id);

/*
// @brief Check if department is in database
// @param database (const Database*) Database to check
// @param dpt_descr (const char*) Department description of department in question
// @return (int) If not found, -1. Else, index of department in database
*/
int checkDpt(const Database* database, const char* dpt_descr);

//
//---------- Menu functions ---------- /
//

/*
// @brief Main menu for user interface
// @param database (const Database*) pointer to database
// @param input_files (char[2][100]) list of input files
// @return (unsigned short) user return. 7 on error
*/
unsigned short mainMenu(const Database* datbase, char input_files[2][100]);

/*
// @brief Menu to display departments on CLI
// @param database (const Database*) Pointer to database
// @return (unsigned short) Return 1 on error
*/
unsigned short displayDepartments(const Database* database);

/*
// @brief Display employees for given deparment on CLI
// @param database (const Database*) Pointer to database with employee list
// @param dpt (const Department*) Pointer to department with employees
// @return (unsigned short) 0
*/
unsigned short displayEmployeesForDpt(const Database* database, const Department* dpt);

/*
// @brief Generate employeeRecord.txt
// @param database (const Database*) Database with employees
// @return (unsigned short) 0 on error 1 on success 
*/
unsigned short generateEmployeeRecord(const Database* database);

/*
// @brief generate departmentBudget.txt
// @param database (const Database*) Database with employees
// @return (unsigned short) 0 on error 1 on success
*/
unsigned short generateBudgetRecord(const Database* database);

#endif // __EMPLOYEE_RECORD_H__