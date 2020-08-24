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

struct Date {
    struct Day {
        short day, year, time;
    };
    struct Time {
        short hour, minute, second;
    };
};

struct EmployeeData {
    char name[];
    char job_title[];
    char dpt_id[];
    char dpt_name[];

    Date hire_date;
    Date term_date;

    float net_salary;
    float gross_salary;
};

struct Employee {
    EmployeeData* prev;
    EmployeeData* next;
    EmployeeData* curr;
};
Employee *HEAD, *TAIL;

void addEmployee(const EmployeeData &employee);
void delEmployee(const EmployeeData &employee);
void sortListByDpt


#endif // __EMPLOYEE_RECORD_H__