// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Alg     -   Employe Record SOURCE
//   Date    -   8-24-20
//
// ########################################## 

#include "employee_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    Database database;
    database.employee_list = NULL;
    database.dpt_list = NULL;
    database.num_employees = 0;
    database.num_dpts = 0;

    const unsigned int num_input_files = 2;
    char input_files[2][100];
    strcpy(input_files[0], "pa_1/resources/BaltimoreCityEmployeeSalariesFY2018.csv");
    strcpy(input_files[1], "pa_1/resources/BaltimoreCityEmployeeSalariesFY2019.csv");

    printf("This is a menu driven program. Please follow the instructions to execute properly.\r\n\r\n");

    while(1==1) {
        unsigned short mainMenuReturn = mainMenu(&database, input_files);

        switch(mainMenuReturn) {
            case 0:
                destroyDatabase(&database);
                exit(0);
                break;
            case 1:
                system("clear");
                printf("\r\n|----- Loading Default Input Files -----|\r\n\r\n");
                strcpy(input_files[0], "./BaltimoreCityEmployeeSalariesFY2018.csv");
                printf("Loaded:\t%s\r\n", input_files[0]);

                strcpy(input_files[1], "./BaltimoreCityEmployeeSalariesFY2019.csv");
                printf("Loaded:\t%s\r\n", input_files[1]);
                break;
            case 2:
            {
                system("clear");
                printf("|----- Enter Input Files -----|\r\n\r\n");

                printf("Enter a comma delimmited list of input files relative to program path\r\n\r\n");
                printf("Enter here-> ");

                char buff[num_input_files*100+10];
                scanf("%s", buff);

                char* tmp = strtok(buff, ",");
                int i=0;
                while(tmp != NULL) {
                    if(i >= num_input_files) {
                        printf("Only 2 input files may be entered. Ensure there is only one comma\r\n");
                        break;
                    }
                    strcpy(input_files[i++], tmp);
                    tmp = strtok(NULL, ",");
                }

                if(i != 2) printf("Incorrect number of input files detected\r\n");

                break;
            }
            case 3:
                destroyDatabase(&database);
                createDatabase(input_files, num_input_files, &database);
                break;
            case 4:
            {
                unsigned int displayDptReturn = displayDepartments(&database);
                while(displayDptReturn == 1) displayDptReturn = displayDepartments(&database);
                
                break;
            }
            case 5:
                //generateEmployeeRecord();
                break;
            case 6:
                //generateDptBudget();
                break;
            case 7:
                break;
        }
    }

    /*
    printf("%d\r\n", database.num_dpts);
    float budget2019 = 0, budget2018 = 0;
    for(int i=0; i<database.num_dpts; ++i) {
        budget2018 += database.dpt_list[i].annual_salary_2018;
        budget2019 += database.dpt_list[i].annual_salary_2019;
    }

    printf("City Budget 2018:\t%f\r\n", budget2018);
    printf("City Budget 2019:\t%f\r\n", budget2019);
    for(int i=0; i<database.num_employees; ++i) {
        if(database.employee_list[i].hire_status == terminated) {
            char tmp_employee_str[400];
            printf("%s\r\n", employeeToString(&database.employee_list[i], tmp_employee_str));
        }
    }
    */
    return 0;
}

/*
// @brief Create an employee database from input filesfields[5]
// @param input_files A list of filenames to be used as input
// @param numFiles The number of input files
// @param database Database to store output
*/
void createDatabase(char input_files[2][100], int num_files, Database *database) {
    system("clear");
    printf("|----- Generating Database -----|\r\n\r\n");

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
        fgets(buff, 98, file);
        
        int j = 2;
        while(fgets(buff, 198, file) != NULL) {
            char fields[7][100];

            if(j == 1864) {
                int s = 1;
            }

            char *tmp = strtok(buff, ",");
            int k = 0;
            while(tmp != NULL) {
                char field[100];
                strcpy(field, tmp);
                
                char *quotes_check = strchr(field, '\"');
                
                if(quotes_check != NULL) {
                    char name[99];
                    strcpy(name, field+1);
                    tmp = strtok(NULL, "\"");
                    strcat(name, ", ");
                    strcat(name, tmp);
                    strcpy(field, name);
                }

                strcpy(fields[k++], field);
                tmp = strtok(NULL, ",");
            }

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


            Date term_date;
            term_date.day.month     = 0;
            term_date.day.day       = 0;
            term_date.day.year      = 0;
            term_date.time.hour     = 0;
            term_date.time.minute   = 0;

            Employee tmp_employee;
            char hire_date_str[100];

            strcpy(tmp_employee.id,         fields[0]);
            strcat(tmp_employee.id,         dateToString(&hire_date, hire_date_str));

            strcpy(tmp_employee.full_name,  fields[0]);
            strcpy(tmp_employee.job_title,  fields[1]);
            strcpy(tmp_employee.dpt_id,     fields[2]);
            strcpy(tmp_employee.dpt_descr,  fields[3]);
            
            tmp_employee.hire_date   = hire_date;
            tmp_employee.term_date   = term_date;
            tmp_employee.net_salary  = atof(fields[5]);
            tmp_employee.grs_salary  = atof(fields[6]);
            tmp_employee.hire_status   = newHire;

            char tmp_filename[100];
            strcpy(tmp_filename, input_files[i]);
            if(strstr(tmp_filename, "FY2018") != NULL) addEmployee(&tmp_employee, database, 2018);
            else addEmployee(&tmp_employee, database, 2019);
        }

        printf("FINISH\tFile %d of %d\r\n", i+1, num_files);
        fclose(file);
    }

    Date term_date;
    term_date.day.month     = 1;
    term_date.day.day       = 1;
    term_date.day.year      = 2019;
    term_date.time.hour     = 0;
    term_date.time.minute   = 0;

    for(int i=0; i<database->num_employees; ++i) {
        if(database->employee_list[i].hire_status != reHired && database->employee_list[i].hire_date.day.year < 2019) { 
            database->employee_list[i].hire_status = terminated;
            database->employee_list[i].term_date = term_date;
        }
    }

    printf("\r\nDatabase generated successfully. %d unique employees found in %d departments\r\n", database->num_employees, database->num_dpts);
    return;
}

/*
// @brief Destroy the entire employee database.
// @param database Database to destroy
*/
void destroyDatabase(Database* database) { 
    if(database != NULL) { 
        if(database->dpt_list != NULL)      free(database->dpt_list);
        if(database->employee_list != NULL) free(database->employee_list);

        database->num_employees = 0;
        database->num_dpts = 0;
        database->dpts_allocated = 0;
        database->employees_allocated = 0;
    }
}

/*
// @brief Add an employee to the database. Allocated on heap.
// @param employee Temporary allocation to ease passage of data to heap
// @param database Database to which employee is added
// @year Fiscal year of employee tabulation
*/
unsigned int addEmployee(const Employee *employee, Database *database, int year) {
   
    /////////////////////////////////
    // ALLOCATE SPACE FOR EMPLOYEES
    //
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

    /////////////////////////////////
    // ALLOCATE SPACE FOR DEPARTMENTS
    //
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

    /////////////////////////////////
    // ADD EMPLOYEE INFORMATION TO DATABASE
    //
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

    /////////////////////////////////
    // ADD DEPARTMENT INFORMATION TO DATABASE
    //
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
// @param date Date to be converted
// @param str String to store output
// @return Converted string (same string as str)
*/
const char* dateToString(const Date *date, char* str) {
    sprintf(str, "%d/%d/%d %d:%d", date->day.month, date->day.day, date->day.year, date->time.hour, date->time.minute);
    return str;
}

/*
// @brief Convert Employee struct to string
// @param employee Employee to be converted
// @param str String to store output
// @return Converted string (same string as str)
*/
const char* employeeToString(const Employee *employee, char* str) {
    char tmp_str[20];
    sprintf(str, "%35s\t", employee->full_name);
    sprintf(str + strlen(str), "%40s",      employee->job_title);
    sprintf(str + strlen(str), "%10s",      employee->dpt_id);
    sprintf(str + strlen(str), "%20s",      employee->dpt_descr);
    sprintf(str + strlen(str), "%15s",      dateToString(&employee->hire_date, tmp_str));
    sprintf(str + strlen(str), "%15s",      dateToString(&employee->term_date, tmp_str));
    sprintf(str + strlen(str), "%10.2f",    employee->grs_salary);
    sprintf(str + strlen(str), "%10.2f\r\n",employee->net_salary);

    return str;
}

/* 
//  @brief Convert Department struct to string
//  @param dpt Department to be converted
//  @param str String to store output
//  @return Converted string (same string as str)
*/
const char* dptToString(const Department *dpt, char *str) {
    char temp[20];
    sprintf(str, "Dpt ID:\t%s\r\n", dpt->dpt_id);
    sprintf(str + strlen(str), "Dpt Descr:\t%s\r\n", dpt->dpt_descr);
    sprintf(str + strlen(str), "Salary FY2018:\t%f\r\n", dpt->annual_salary_2018);
    sprintf(str + strlen(str), "Salary FY2019:\t%f\r\n", dpt->annual_salary_2019);
    sprintf(str + strlen(str), "Salary Net Change:\t%f\r\n", dpt->net_change);

    return str;
}

/*
// @brief Remove whitespace from input, return as output
// @param input String with whitespace to be removed
// @param output String to store output
// @return output
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
// @param database Database to check against
// @param id ID of employee to check
// @return If not found, -1. Else, index of employee in database.
*/
int checkEmployee(const Database* database, const char* id) {
    char employee_id_no_whitespace[200];
    removeWhitespace(id, employee_id_no_whitespace);

    for(int i=0; i<database->num_employees; ++i) {
        char database_id_no_whitespace[200];
        removeWhitespace(database->employee_list[i].id, database_id_no_whitespace);
        if(strcmp(employee_id_no_whitespace, database_id_no_whitespace) == 0) return i;
    }

    return -1;
}

/*
// @brief Check if department is in database
// @param database Database to check
// @param dpt_descr Department description of department in question
// @return If not found, -1. Else, index of department in database
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

unsigned short mainMenu(const Database* database, char input_files[2][100]) {
    printf("\r\n|----- Main Menu -----|\r\n\r\n");

    if(database == NULL || database->employee_list == NULL || database->dpt_list == NULL) {
        printf("Database generated\tFALSE\r\n");
    } else {
        printf("Database generated\tTRUE\r\n");
    }
    printf("Loaded input files:\t");
    for(int i=0; i<2; ++i) {
        printf("%s\r\n\t\t\t", input_files[i]);
    }
    printf("\r\n");
    printf("0.)\tEnter <0> to exit\r\n");
    printf("1.)\tEnter <1> to load default input files\r\n");
    printf("2.)\tEnter <2> to enter/update list of input files relative to program path, delimtted by commas.\r\n");
    printf("3.)\tEnter <3> to generate/regenerate database\r\n\r\n");
    
    printf("4.)\tEnter <4> to display employees by department\r\n");
    printf("5.)\tEnter <5> to generate employeeRecord.txt\r\n");
    printf("6.)\tEnter <6> to generate deparmentBudget.txt\r\n");

    int user_input;
    printf("\r\nEnter here-> ");
    scanf("%d", &user_input);

    if(user_input >= 0 && user_input <= 6) return user_input;
    

    printf("\r\nInput unregonized, please try again.\r\n");
    return 6;
}

unsigned short displayDepartments(const Database* database) {
    system("clear");
    printf("\r\n|----- Choose Department -----|\r\n\r\n");

    printf("0.)\tEnter <0> to return to the main menu\r\n\r\n");
    for(int i=0; i<database->num_dpts; ++i) {
        printf("%d.)\tEnter <%d> to select %s\r\n", i+1, i+1, database->dpt_list[i].dpt_descr);
    }

    int user_input;
    printf("\r\nEnter here-> ");
    scanf("%d", &user_input);
    
    if(user_input == 0) return 0;

    if(user_input-1 >= database->num_dpts || user_input-1 < 0) {
        printf("\r\nInput must be between 1 and %d, inclusive. Please try again.", database->num_dpts);
        return 1;
    }

    displayEmployeesForDpt(database, &database->dpt_list[user_input-1]);
    return 2;
}

unsigned short displayEmployeesForDpt(const Database* database, const Department* dpt) {
    system("clear");
    printf("\r\n|----- Display Employees for %s ----- |\r\n\r\n", dpt->dpt_descr);

    for(int i=0; i<database->num_employees; ++i) {
        if(strcmp(database->employee_list[i].dpt_descr, dpt->dpt_descr) == 0) {
            char buff[400];
            printf("%s\r\n", employeeToString(&database->employee_list[i], buff));
        }
    }

    printf("\r\n\r\nEnter any key to return to main menu\r\n");
    printf("\r\nEnter here-> ");
    char input[10];
    scanf("%s", input);

    return 0;
}