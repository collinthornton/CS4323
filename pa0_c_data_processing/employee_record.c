// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Alg     -   Employe Record SOURCE
//   Date    -   9-2-20
//
// ########################################## 

#include "employee_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
// @brief Create an employee database from input filesfields[5]
// @param input_files (char[2][100]) A list of filenames to be used as input
// @param numFiles (int) The number of input files
// @param database (Database*) Database to store output
// @return void
*/
void createDatabase(char input_files[2][100], int num_files, Database *database) {
    system("clear");
    printf("|----- Generating Database -----|\r\n\r\n");

    // ITERATE THROUGH INPUT FILES
    for(int i=0; i<num_files; ++i) {
        FILE *file = fopen(input_files[i], "r");

        if(file == NULL) {
            printf("File '%s' not found. Destorying database and returning to main menu\r\n", input_files[i]);
            destroyDatabase(database);
            return;
        }

        printf("START\tFile %d of %d\r\n", i+1, num_files);

        char buff[200];
        char *rest;

        // TRASH FIRST LINE OF FILE
        fgets(buff, 98, file);
        
        int j = 2;

        // ITERATE THROUGH LINES IN FILE
        while(fgets(buff, 198, file) != NULL) {
            char fields[7][100];

            // BUST LINE INTO FIELDS WITH COMMA DELIMITER
            char *tmp = strtok(buff, ",");
            int k = 0;

            // ITERATE THROUGH FIELDS
            while(tmp != NULL) {
                char field[100];
                strcpy(field, tmp);
                
                char *quotes_check = strchr(field, '\"');
                
                // QUOTATION HAVE PRECEDENCE OVER COMMA FOR A FIELD
                if(quotes_check != NULL) {
                    char name[99];
                    strcpy(name, field+1);
                    tmp = strtok(NULL, "\"");
                    strcat(name, ", ");
                    strcat(name, tmp);
                    strcpy(field, name);
                }

                strcpy(fields[k++], field);

                // ADVANCE TO NEXT FIELD
                tmp = strtok(NULL, ",");
            }

            // CONVERT STRING DATE INTO STRUCT
            Date hire_date;
            if(strchr(fields[4], ':') == NULL) {
                hire_date.day.month   = 1;
                hire_date.day.day     = 1;
                hire_date.day.year    = 2019;
                hire_date.time.hour   = 0;
                hire_date.time.minute = 0; 
            } else {
                hire_date.day.month   = atof(strtok(fields[4], "/"));
                hire_date.day.day     = atof(strtok(NULL, "/"));
                hire_date.day.year    = atof(strtok(NULL, " "));
                hire_date.time.hour   = atof(strtok(NULL, ":"));
                hire_date.time.minute = atof(strtok(NULL, ":"));               
            }

            // INITIALIZE TERMINATION DATE
            Date term_date;
            term_date.day.month     = 0;
            term_date.day.day       = 0;
            term_date.day.year      = 0;
            term_date.time.hour     = 0;
            term_date.time.minute   = 0;

            // INITIALIZE TEMPORARY EMPLOYEE; USED TO PASS DATA
            Employee tmp_employee;
            char hire_date_str[100];

            char id_whitespace[100];
            strcpy(id_whitespace,           fields[0]);
            strcat(id_whitespace,           dateToString(&hire_date, hire_date_str));
            removeWhitespace(id_whitespace, tmp_employee.id);

            strcpy(tmp_employee.full_name,  fields[0]);
            strcpy(tmp_employee.job_title,  fields[1]);
            strcpy(tmp_employee.dpt_id,     fields[2]);
            strcpy(tmp_employee.dpt_descr,  fields[3]);
            
            tmp_employee.hire_date   = hire_date;
            tmp_employee.term_date   = term_date;
            tmp_employee.net_salary  = atof(fields[5]);
            tmp_employee.grs_salary  = atof(fields[6]);
            tmp_employee.hire_status   = newHire;

            // DETERMINE WHETHER EMPLOYEE IS FROM FY2018 OR FY2019
            char tmp_filename[100];
            strcpy(tmp_filename, input_files[i]);
            if(strstr(tmp_filename, "FY2018") != NULL) addEmployee(&tmp_employee, database, 2018);
            else addEmployee(&tmp_employee, database, 2019);
        }

        printf("FINISH\tFile %d of %d\r\n", i+1, num_files);
        fclose(file);
    }

    // INITIALIZE TERMINATION DATE AS JAN 1, 2019 AT MIDNIGHT
    Date term_date;
    term_date.day.month     = 1;
    term_date.day.day       = 1;
    term_date.day.year      = 2019;
    term_date.time.hour     = 0;
    term_date.time.minute   = 0;

    // UPDATE TERMINATION DATE FOR EMPLOYEES NOT REHIRED
    for(int i=0; i<database->num_employees; ++i) {
        if(database->employee_list[i].hire_status != reHired && database->employee_list[i].hire_date.day.year < 2019) { 
            database->employee_list[i].hire_status = terminated;
            database->employee_list[i].term_date = term_date;
        }
    }

    // UPDATE NET BUDGET CHANGE FOR DEPARTMENTS
    for(int i=0; i<database->num_dpts; ++i) {
        database->dpt_list[i].net_change = database->dpt_list[i].annual_salary_2019 - database->dpt_list[i].annual_salary_2018;
    }

    printf("\r\nDatabase generated successfully. %d unique employees found in %d departments\r\n", database->num_employees, database->num_dpts);
    return;
}

/*
// @brief Destroy the entire employee database.
// @param database (Database*) Database to destroy
// @return (void)
*/
void destroyDatabase(Database* database) { 
    // FREE SPACE AND RESET VARIABLES OF DATABASE
    if(database != NULL) { 
        if(database->dpt_list != NULL)      free(database->dpt_list);
        if(database->employee_list != NULL) free(database->employee_list);

        database->num_employees         = 0;
        database->num_dpts              = 0;
        database->dpts_allocated        = 0;
        database->employees_allocated   = 0;
        database->dpt_list              = NULL;
        database->employee_list         = NULL;
    }
}

/*
// @brief Add an employee to the database. Allocated on heap.
// @param employee (const Employee*) Temporary allocation to ease passage of data to heap
// @param database (Database*) Database to which employee is added
// @param year (int) Fiscal year of employee tabulation
// @return (unsigned int) -> Number of employees in database
*/
unsigned int addEmployee(const Employee *employee, Database *database, int year) {

    // ALLOCATE SPACE FOR EMPLOYEES
    if(database->num_employees == 0) {
        database->employee_list = (Employee*)malloc(2*sizeof(Employee));
        database->employees_allocated = 2;
        if(database->employee_list == NULL) {
            printf("Error during employee allocation\r\n");
            destroyDatabase(database);
            exit(-1);
        }
    }
    else if(database->num_employees > database->employees_allocated/2) {
        Employee *tmp_list = (Employee*)realloc(database->employee_list, 2*database->employees_allocated*sizeof(Employee));
        if(tmp_list == NULL) {
            printf("Error during employee list reallocation\r\n");
            destroyDatabase(database);
            exit(-1);
        }
        database->employee_list = tmp_list;    
        database->employees_allocated *= 2;
    }

    // ALLOCATE SPACE FOR DEPARTMENTS
    if(database->num_dpts == 0) {
        Department *tmp_list = (Department*)malloc(2*sizeof(Department));
        if(tmp_list == NULL) {
            printf("Error during dpt list allocation\r\n");
            destroyDatabase(database);
            exit(-1);
        }
        database->dpt_list = tmp_list; 
        database->dpts_allocated = 2;
    }
    else if(database->num_dpts > database->dpts_allocated/2) {
        Department *tmp_list = (Department*)realloc(database->dpt_list, 2*database->dpts_allocated*sizeof(Department));
        if(tmp_list == NULL) {
            printf("Error during dpt list reallocation\r\n");
            destroyDatabase(database);
            exit(-1);
        }
        
        database->dpt_list = tmp_list;
        database->dpts_allocated *= 2;
    }

    // ADD EMPLOYEE INFORMATION TO DATABASE
    int employee_index = checkEmployee(database, employee->id);
    if(employee_index < 0) {
        strcpy(database->employee_list[database->num_employees].id,         employee->id);
        strcpy(database->employee_list[database->num_employees].full_name,  employee->full_name);
        strcpy(database->employee_list[database->num_employees].job_title,  employee->job_title);
        strcpy(database->employee_list[database->num_employees].dpt_id,     employee->dpt_id);
        strcpy(database->employee_list[database->num_employees].dpt_descr,  employee->dpt_descr);

        database->employee_list[database->num_employees].hire_date   = employee->hire_date;
        database->employee_list[database->num_employees].term_date   = employee->term_date;
        database->employee_list[database->num_employees].grs_salary  = employee->grs_salary;
        database->employee_list[database->num_employees].net_salary  = employee->net_salary;
        database->employee_list[database->num_employees].hire_status   = employee->hire_status;
      
        ++database->num_employees;
    } else {
        if(year == 2019) database->employee_list[employee_index].hire_status = reHired;
    }

    // ADD DEPARTMENT INFORMATION TO DATABASE
    int dpt_index = checkDpt(database, employee->dpt_descr);
    if(dpt_index < 0) {
        strcpy(database->dpt_list[database->num_dpts].dpt_id, employee->dpt_id);
        strcpy(database->dpt_list[database->num_dpts].dpt_descr, employee->dpt_descr);
        switch(year) {
            case 2018:
            database->dpt_list[database->num_dpts].annual_salary_2018 = employee->grs_salary;
            database->dpt_list[database->num_dpts].annual_salary_2019 = 0.0;
            break;

            case 2019:
            database->dpt_list[database->num_dpts].annual_salary_2019 = employee->grs_salary;
            database->dpt_list[database->num_dpts].annual_salary_2018 = 0.0;
            break;

            default:
            printf("Year not recognized for employee %s. Exiting.\r\n", employee->full_name);
            exit(-2);
        }
        ++database->num_dpts;
    } else {
        switch(year) {
            case 2018:
            database->dpt_list[dpt_index].annual_salary_2018 += employee->grs_salary;
            break;

            case 2019:
            database->dpt_list[dpt_index].annual_salary_2019 += employee->grs_salary;
            break;

            default:
            printf("Year not recognized for employee %s. Exiting.\r\n", employee->full_name);
            exit(-2);
        }
    }

    return database->num_employees;
}

/*
// @brief Convert Date struct to string
// @param date (const Date*) Date to be converted
// @param str (char*) String to store output
// @return (const char*) -> Converted string (same string as str)
*/
const char* dateToString(const Date *date, char* str) {
    sprintf(str, "%d/%d/%d %d:%d", date->day.month, date->day.day, date->day.year, date->time.hour, date->time.minute);
    return str;
}

/*
// @brief Convert Employee struct to string
// @param employee (const Employee*) Employee to be converted
// @param str (char*) String to store output
// @param human_readable (unsigned short) Flag to toggle between output delimeters of columns (1) or tabs (0)
// @return (const char*) -> Converted string (same string as str)
*/
const char* employeeToString(const Employee *employee, char* str, unsigned short human_readable) {
    char tmp_str[20];
    if(human_readable == 1) {
        sprintf(str, "%35s", employee->full_name);
        sprintf(str + strlen(str), "%40s",      employee->job_title);
        sprintf(str + strlen(str), "%10s",      employee->dpt_id);
        sprintf(str + strlen(str), "%25s",      employee->dpt_descr);
        sprintf(str + strlen(str), "%15s",      dateToString(&employee->hire_date, tmp_str));
        sprintf(str + strlen(str), "%15s",      dateToString(&employee->term_date, tmp_str));
        sprintf(str + strlen(str), "%10.2f",    employee->net_salary);
        sprintf(str + strlen(str), "%10.2f\r\n",employee->grs_salary);        
    } else {
        sprintf(str, "%s\t", employee->full_name);
        sprintf(str + strlen(str), "%s\t",      employee->job_title);
        sprintf(str + strlen(str), "%s\t",      employee->dpt_id);
        sprintf(str + strlen(str), "%s\t",      employee->dpt_descr);
        sprintf(str + strlen(str), "%s\t",      dateToString(&employee->hire_date, tmp_str));
        sprintf(str + strlen(str), "%s\t",      dateToString(&employee->term_date, tmp_str));
        sprintf(str + strlen(str), "%.2f\t",    employee->net_salary);
        sprintf(str + strlen(str), "%.2f",  employee->grs_salary);   
    }

    return str;
}

/* 
// @brief Convert Department struct to string
// @param dpt (const Department*) Department to be converted
// @param str (char*) String to store output
// @param human_readable (unsigned short) Flag to toggle between output delimeters of columns (1) or tabs (0)
// @return (const char*) Converted string (same string as str)
*/
const char* dptToString(const Department *dpt, char *str, unsigned short human_readable) {
    char temp[20];
    if(human_readable == 1) {
        sprintf(str, "%40s", dpt->dpt_descr);
        sprintf(str + strlen(str), "%10.2f",    dpt->annual_salary_2018);
        sprintf(str + strlen(str), "%10.2f",    dpt->annual_salary_2019);
        sprintf(str + strlen(str), "%10.2f\r\n",dpt->net_change);
    } else {
        sprintf(str, "%s\t", dpt->dpt_descr);
        sprintf(str + strlen(str), "%.2f\t",    dpt->annual_salary_2018);
        sprintf(str + strlen(str), "%.2f\t",    dpt->annual_salary_2019);
        sprintf(str + strlen(str), "%.2f\t",dpt->net_change);        
    }

    return str;
}

/*
// @brief Remove whitespace from input, return as output
// @param input (const char*) String with whitespace to be removed
// @param output (char*) String to store output
// @return (char*) Processed string (same string as output)
*/
char* removeWhitespace(const char* input, char* output) {
    char tmp[200];
    int i=0, j=0;

    while(input[i] != '\0') {
        if(input[i] != ' ') tmp[j++] = input[i];
        ++i;
    }
    tmp[j] = '\0';
    strcpy(output, tmp);
    return output;
}

/*
// @brief Check if employee is in database
// @param database (const Database*) Database to check against
// @param id (const char*) ID of employee to check
// @return (int) If not found, -1. Else, index of employee in database.
*/
int checkEmployee(const Database* database, const char* id) {
    char employee_id_no_whitespace[200];
    removeWhitespace(id, employee_id_no_whitespace);

    for(int i=0; i<database->num_employees; ++i) {
        if(strcmp(employee_id_no_whitespace, database->employee_list[i].id) == 0) return i;
    }

    return -1;
}

/*
// @brief Check if department is in database
// @param database (const Database*) Database to check
// @param dpt_descr (const char*) Department description of department in question
// @return (int) If not found, -1. Else, index of department in database
*/
int checkDpt(const Database* database, const char* dpt_descr) {
    char dpt_descr_no_whitespace[100];
    removeWhitespace(dpt_descr, dpt_descr_no_whitespace);

    for(int i=0; i<database->num_dpts; ++i) {
        char dataspace_no_whitespace[100];
        removeWhitespace(database->dpt_list[i].dpt_descr, dataspace_no_whitespace);

        if(strcmp(dataspace_no_whitespace, dpt_descr_no_whitespace) == 0) return i;
    }
    return -1;
}

/*
// @brief Main menu for user interface
// @param database (const Database*) pointer to database
// @param input_files (char[2][100]) list of input files
// @return (unsigned short) user return. 7 on error
*/
unsigned short mainMenu(const Database* database, char input_files[2][100]) {
    printf("\r\n|----- Main Menu -----|\r\n\r\n");

    // DISPALY DATABASE STATUS
    if(database == NULL || database->employee_list == NULL || database->dpt_list == NULL) {
        printf("Database generated\tFALSE\r\n");
    } else {
        printf("Database generated\tTRUE\r\n");
    }

    // DISPLAY INPUT FILE STATUS
    printf("Loaded input files:\t");
    for(int i=0; i<2; ++i) {
        printf("%s\r\n\t\t\t", input_files[i]);
    }

    // DISPLAY MENU
    printf("\r\n");
    printf("0.)\tEnter <0> to exit\r\n");
    printf("1.)\tEnter <1> to load default input files\r\n");
    printf("2.)\tEnter <2> to enter/update list of input files relative to program path, delimtted by commas.\r\n");
    printf("3.)\tEnter <3> to generate/regenerate database\r\n\r\n");
    
    printf("4.)\tEnter <4> to display employees by department\r\n");
    printf("5.)\tEnter <5> to generate employeeRecord.txt\r\n");
    printf("6.)\tEnter <6> to generate deparmentBudget.txt\r\n");

    // GET INPUT
    int user_input;
    printf("\r\nEnter here-> ");
    scanf("%d", &user_input);

    if(user_input >= 0 && user_input <= 6) return user_input;

    printf("\r\nInput unregonized, please try again.\r\n");
    return 7;
}

/*
// @brief Menu to display departments on CLI
// @param database (const Database*) Pointer to database
// @return (unsigned short) Return 1 on error
*/
unsigned short displayDepartments(const Database* database) {
    system("clear");
    printf("\r\n|----- Choose Department -----|\r\n\r\n");

    // DISPLAY MENU
    printf("0.)\tEnter <0> to return to the main menu\r\n\r\n");
    for(int i=0; i<database->num_dpts; ++i) {
        printf("%d.)\tEnter <%d> to select %s\r\n", i+1, i+1, database->dpt_list[i].dpt_descr);
    }

    // GET USERT INPUT
    int user_input;
    printf("\r\nEnter here-> ");
    scanf("%d", &user_input);
    
    if(user_input == 0) return 0;

    if(user_input-1 >= database->num_dpts || user_input-1 < 0) {
        printf("\r\nInput must be between 1 and %d, inclusive. Please try again.", database->num_dpts);
        return 1;
    }

    // DISPLAY EMPLOYEES
    displayEmployeesForDpt(database, &database->dpt_list[user_input-1]);
    return 2;
}

/*
// @brief Display employees for given deparment on CLI
// @param database (const Database*) Pointer to database with employee list
// @param dpt (const Department*) Pointer to department with employees
// @return (unsigned short) 0
*/
unsigned short displayEmployeesForDpt(const Database* database, const Department* dpt) {
    system("clear");
    printf("\r\n|----- Display Employees for %s ----- |\r\n\r\n", dpt->dpt_descr);

    // DISPLAY ALL EMPLOYEES FOR GIVEN DEPARTMENT
    for(int i=0; i<database->num_employees; ++i) {
        if(strcmp(database->employee_list[i].dpt_descr, dpt->dpt_descr) == 0) {
            char buff[400];
            printf("%s\r\n", employeeToString(&database->employee_list[i], buff, 1));
        }
    }

    // WAIT FOR INPUT TO RETURN TO MAIN MENU
    printf("\r\n\r\nEnter any key to return to main menu\r\n");
    printf("\r\nEnter here-> ");
    char input[10];
    scanf("%s", input);

    return 0;
}

/*
// @brief Generate employeeRecord.txt
// @param database (const Database*) Database with employees
// @return (unsigned short) 0 on error 1 on success 
*/
unsigned short generateEmployeeRecord(const Database* database) {
    system("clear");
    printf("|----- Generating Employee Database -----|\r\n\r\n");
    if(database == NULL || database->employee_list == NULL || database->dpt_list == NULL) {
        printf("Database not initialized. Returning to main menu\r\n");
        return 0;
    }

    // TRY TO OPEN OUTPUT FILE. RETURN 0 ON FAILURE
    FILE *file = fopen("employeeRecord.txt", "w");
    if(file == NULL) {
        printf("employeeRecord.txt could not be created in the current directory. Returning to main menu\r\n");
        return 0;
    }

    // WRITE DATA
    fprintf(file, "FULL_NAME\tJOB_TITLE\tDEPT_ID\tDESCR\tHIRE_DT\tTERMINATION_DT\tANNUAL_RT\tGROSS");
    for(int i=0; i<database->num_employees; ++i) {
        char buff[400];
        fprintf(file, "\r\n%s", employeeToString(&database->employee_list[i], buff, 0));
    }

    // CLOSE FILE
    fclose(file);
    return 1;
}

/*
// @brief generate departmentBudget.txt
// @param database (const Database*) Database with employees
// @return (unsigned short) 0 on error 1 on success
*/
unsigned short generateBudgetRecord(const Database* database) {
    system("clear");
    printf("|----- Generating Department Budgets -----|\r\n\r\n");
    if(database == NULL || database->employee_list == NULL || database->dpt_list == NULL) {
        printf("Database not initalized. Returning to main menu\r\n");
        return 0;
    }

    // TRY TO OPEN OUTPUT FILE. RETURN 0 ON FAILURE
    FILE *file = fopen("departmentBudget.txt", "w");
    if(file == NULL) {
        printf("departmentBudget.txt could not be created in the current directory. Returning to the main menu\r\n");
        return 0;
    }

    // WRITE DATA
    fprintf(file, "DESCR\tANNUAL_SALARY_2018\tANNUAL_SALARY_2019\tNET_CHANGE");
    for(int i=0; i<database->num_dpts; ++i) {
        char buff[400];
        fprintf(file, "\r\n%s", dptToString(&database->dpt_list[i], buff, 0));
    }

    // CLOSE FILE
    fclose(file);
    return 1;
}