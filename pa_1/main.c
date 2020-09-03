// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Alg     -   Employe Record MAIN
//   Date    -   9-2-20
//
// ########################################## 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "employee_record.h"

/*
// @brief Manages database, menu state machine, and input files
*/
int main(int argc, char** argv) {
    // INITIALIZING DATABASE
    Database database;
    database.employee_list = NULL;
    database.dpt_list = NULL;
    database.num_employees = 0;
    database.num_dpts = 0;

    // INITIALIZING INPUT FILES
    const unsigned int num_input_files = 2;
    char input_files[2][100];
    strcpy(input_files[0], "./BaltimoreCityEmployeeSalariesFY2018.csv");
    strcpy(input_files[1], "./BaltimoreCityEmployeeSalariesFY2019.csv");

    printf("This is a menu driven program. Please follow the instructions to execute properly.\r\n\r\n");

    // MENU STATE MACHINE. RUN UNTIL COMMANDED EXIT OR MEMORY ALLOCATION ERROR
    while(1==1) {
        unsigned short mainMenuReturn = mainMenu(&database, input_files);

        switch(mainMenuReturn) {
            case 0:
                // EXIT PROGRAM
                destroyDatabase(&database);
                exit(0);
                break;
            case 1:
                // LOAD DEFAULT INPUT FILES
                system("clear");
                printf("\r\n|----- Loading Default Input Files -----|\r\n\r\n");
                strcpy(input_files[0], "./BaltimoreCityEmployeeSalariesFY2018.csv");
                printf("Loaded:\t%s\r\n", input_files[0]);

                strcpy(input_files[1], "./BaltimoreCityEmployeeSalariesFY2019.csv");
                printf("Loaded:\t%s\r\n", input_files[1]);
                
                destroyDatabase(&database);
                break;
            case 2:
            {
                // ENTER INPUT FILES
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
                destroyDatabase(&database);

                if(i != 2) printf("Incorrect number of input files detected\r\n");

                break;
            }
            case 3:
                // GENERATE NEW DATABASE
                destroyDatabase(&database);
                createDatabase(input_files, num_input_files, &database);
                break;
            case 4:
            {
                // DISPLAY EMPLOYEES BY DEPARTMENT
                unsigned int displayDptReturn = displayDepartments(&database);
                while(displayDptReturn == 1) displayDptReturn = displayDepartments(&database);
                
                break;
            }
            case 5:
                // GENERATE employeeRecord.txt
                if(generateEmployeeRecord(&database) == 0) break;

                printf("employeeRecord.txt generated successfully\r\n");
                break;
            case 6:
                // GENERATE departmentBudget.txt
                if(generateBudgetRecord(&database) == 0) break;

                printf("departmentBudget.txt generated successfully\r\n");
                break;
            default:
                break;
        }
    }

    return 0;
}