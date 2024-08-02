#include <stdio.h>
#include <oci.h>

void checkerr(OCIError *errhp, sword status) {
    text errbuf[512];
    sb4 errcode = 0;

    switch (status) {
        case OCI_SUCCESS:
            break;
        case OCI_SUCCESS_WITH_INFO:
            printf("OCI_SUCCESS_WITH_INFO\n");
            break;
        case OCI_NEED_DATA:
            printf("OCI_NEED_DATA\n");
            break;
        case OCI_NO_DATA:
            printf("OCI_NO_DATA\n");
            break;
        case OCI_ERROR:
            OCIErrorGet((void *)errhp, (ub4)1, (text *)NULL, &errcode, errbuf, (ub4)sizeof(errbuf), OCI_HTYPE_ERROR);
            printf("Error - %.*s\n", 512, errbuf);
            break;
        case OCI_INVALID_HANDLE:
            printf("OCI_INVALID_HANDLE\n");
            break;
        case OCI_STILL_EXECUTING:
            printf("OCI_STILL_EXECUTING\n");
            break;
        case OCI_CONTINUE:
            printf("OCI_CONTINUE\n");
            break;
        default:
            break;
    }
}

int main() {
    OCIEnv *envhp;
    OCIError *errhp;
    OCISvcCtx *svchp;
    OCIStmt *stmthp;
    OCISession *usrhp;
    OCIAuthInfo *authp;

    char *username = "minsung";
    char *password = "silcroad";
    char *dbname = "STUDENTS";

    if (OCIEnvCreate((OCIEnv **)&envhp, (ub4)OCI_THREADED | OCI_OBJECT, (dvoid *)0,
                      0, 0, 0, (size_t)0, (dvoid **)0)) {
        printf("OCIEnvCreate failed\n");
        return -1;
    }

    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&errhp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);
    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&svchp, OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0);
    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&authp, OCI_HTYPE_AUTHINFO, (size_t)0, (dvoid **)0);

    OCIAttrSet((dvoid *)authp, OCI_HTYPE_AUTHINFO, (dvoid *)username, (ub4)strlen(username), OCI_ATTR_USERNAME, errhp);
    OCIAttrSet((dvoid *)authp, OCI_HTYPE_AUTHINFO, (dvoid *)password, (ub4)strlen(password), OCI_ATTR_PASSWORD, errhp);

    if (OCILogon2(envhp, errhp, &svchp, authp, OCI_LOGON2_STMTCACHE | OCI_LOGON2_CPOOL)) {
        checkerr(errhp, OCI_ERROR);
        return -1;
    }

    printf("Connected to the database\n");

    addStudent(svchp, errhp, 1, "Minsung", 20, "Applied Physics");

    getStudent(svchp, errhp, 1);

    updateStudent(svchp, errhp, 1, "Minsung", 21, "Computer Science");
	
    getStudent(svchp, errhp, 1);
		
    deleteStudent(svchp, errhp, 1);

    OCILogoff(svchp, errhp);
    OCIHandleFree((dvoid *)authp, OCI_HTYPE_AUTHINFO);
    OCIHandleFree((dvoid *)errhp, OCI_HTYPE_ERROR);
    OCIHandleFree((dvoid *)svchp, OCI_HTYPE_SVCCTX);
    OCIEnvDestroy(envhp);

    return 0;
}

void addStudent(OCISvcCtx *svchp, OCIError *errhp, int student_id, char *name, int age, char *major) {
    OCIStmt *stmthp;
    const char *sql = "INSERT INTO Students (student_id, name, age, major) VALUES (:1, :2, :3, :4)";

    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
    OCIStmtPrepare(stmthp, errhp, (text *)sql, (ub4)strlen(sql), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);

    OCIBindByPos(stmthp, &bindhp[0], errhp, 1, (dvoid *)&student_id, (sb4)sizeof(student_id), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bindhp[1], errhp, 2, (dvoid *)name, (sb4)strlen(name) + 1, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bindhp[2], errhp, 3, (dvoid *)&age, (sb4)sizeof(age), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bindhp[3], errhp, 4, (dvoid *)major, (sb4)strlen(major) + 1, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT) != OCI_SUCCESS) {
        checkerr(errhp, OCI_ERROR);
    } else {
        printf("Student added successfully\n");
    }

    OCIStmtRelease(stmthp, errhp, (text *)NULL, 0, OCI_DEFAULT);
}

void getStudent(OCISvcCtx *svchp, OCIError *errhp, int student_id) {
    OCIStmt *stmthp;
    OCIDefine *defnp[4];
    int id;
    char name[100];
    int age;
    char major[100];

    const char *sql = "SELECT student_id, name, age, major FROM Students WHERE student_id = :1";

    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
    OCIStmtPrepare(stmthp, errhp, (text *)sql, (ub4)strlen(sql), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);

    OCIBindByPos(stmthp, &bindhp[0], errhp, 1, (dvoid *)&student_id, (sb4)sizeof(student_id), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);

    OCIDefineByPos(stmthp, &defnp[0], errhp, 1, (dvoid *)&id, (sb4)sizeof(id), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &defnp[1], errhp, 2, (dvoid *)name, (sb4)sizeof(name), SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &defnp[2], errhp, 3, (dvoid *)&age, (sb4)sizeof(age), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
    OCIDefineByPos(stmthp, &defnp[3], errhp, 4, (dvoid *)major, (sb4)sizeof(major), SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT) == OCI_SUCCESS) {
        while (OCIStmtFetch2(stmthp, errhp, 1, OCI_DEFAULT, 1, OCI_DEFAULT) == OCI_SUCCESS) {
            printf("ID: %d, Name: %s, Age: %d, Major: %s\n", id, name, age, major);
        }
    } else {
        checkerr(errhp, OCI_ERROR);
    }

    OCIStmtRelease(stmthp, errhp, (text *)NULL, 0, OCI_DEFAULT);
}

void updateStudent(OCISvcCtx *svchp, OCIError *errhp, int student_id, char *name, int age, char *major) {
    OCIStmt *stmthp;
    const char *sql = "UPDATE Students SET name = :1, age = :2, major = :3 WHERE student_id = :4";

    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
    OCIStmtPrepare(stmthp, errhp, (text *)sql, (ub4)strlen(sql), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);

    OCIBindByPos(stmthp, &bindhp[0], errhp, 1, (dvoid *)name, (sb4)strlen(name) + 1, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bindhp[1], errhp, 2, (dvoid *)&age, (sb4)sizeof(age), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bindhp[2], errhp, 3, (dvoid *)major, (sb4)strlen(major) + 1, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);
    OCIBindByPos(stmthp, &bindhp[3], errhp, 4, (dvoid *)&student_id, (sb4)sizeof(student_id), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT) != OCI_SUCCESS) {
        checkerr(errhp, OCI_ERROR);
    } else {
        printf("Student updated successfully\n");
    }

    OCIStmtRelease(stmthp, errhp, (text *)NULL, 0, OCI_DEFAULT);
}

void deleteStudent(OCISvcCtx *svchp, OCIError *errhp, int student_id) {
    OCIStmt *stmthp;
    const char *sql = "DELETE FROM Students WHERE student_id = :1";

    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
    OCIStmtPrepare(stmthp, errhp, (text *)sql, (ub4)strlen(sql), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);

    OCIBindByPos(stmthp, &bindhp[0], errhp, 1, (dvoid *)&student_id, (sb4)sizeof(student_id), SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0, 0, (ub4 *)0, OCI_DEFAULT);

    if (OCIStmtExecute(svchp, stmthp, errhp, 1, 0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT) != OCI_SUCCESS) {
        checkerr(errhp, OCI_ERROR);
    } else {
        printf("Student deleted successfully\n");
    }

    OCIStmtRelease(stmthp, errhp, (text *)NULL, 0, OCI_DEFAULT);
}

