/*
 * ============================================================================
 * Project Name : CareWell Hospital Management System (HMS)
 * Subject      : C Programming Mini-Project (B.Tech 1st / 2nd Year)
 * Technology   : 100% C Programming (Structures, Arrays, Native Winsock HTTP Server)
 * Description  : An end-to-end Hospital Management Application powered entirely
 *                by C. The C backend serves a modern web interface, stores data
 *                in C arrays of structures, persists records to disk, and
 *                automatically launches the browser on execution!
 * Compilation  : gcc main.c -o hospital.exe -lws2_32
 * Execution    : .\hospital.exe
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #define SOCKET int
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define closesocket close
#endif

#define PORT 8080
#define BUFFER_SIZE 65536
#define MAX_PATIENTS 100
#define MAX_DOCTORS 50
#define MAX_APPOINTMENTS 100
#define MAX_BILLS 100

/* ============================================================================
 * 1. DATA STRUCTURE DEFINITIONS (C STRUCTURES)
 * ============================================================================ */

typedef struct {
    int id;
    char name[64];
    int age;
    char gender[16];
    char phone[20];
    char disease[64];
} Patient;

typedef struct {
    int id;
    char name[64];
    char specialization[64];
    char phone[20];
} Doctor;

typedef struct {
    int id;
    int patientId;
    char patientName[64];
    int doctorId;
    char doctorName[64];
    char date[20];
} Appointment;

typedef struct {
    int id;
    int patientId;
    char patientName[64];
    float consultationFee;
    float medicineCharges;
    float treatmentCharges;
    float totalBill;
    char date[20];
} Billing;

/* ============================================================================
 * 2. GLOBAL ARRAYS OF STRUCTURES (PRIMARY STORAGE MECHANISM IN C)
 * ============================================================================ */
Patient patients[MAX_PATIENTS];
int patientCount = 0;

Doctor doctors[MAX_DOCTORS];
int doctorCount = 0;

Appointment appointments[MAX_APPOINTMENTS];
int appointmentCount = 0;

Billing bills[MAX_BILLS];
int billCount = 0;

/* Auto-incrementing IDs */
int nextPatientId = 101;
int nextDoctorId = 201;
int nextApptId = 301;
int nextBillId = 401;

/* ============================================================================
 * 3. BUSINESS LOGIC (CRUD OPERATIONS IN C)
 * ============================================================================ */

void setPatient(int idx, int id, const char *name, int age, const char *gender, const char *phone, const char *disease) {
    patients[idx].id = id;
    strncpy(patients[idx].name, name, sizeof(patients[idx].name) - 1);
    patients[idx].name[sizeof(patients[idx].name) - 1] = '\0';
    patients[idx].age = age;
    strncpy(patients[idx].gender, gender, sizeof(patients[idx].gender) - 1);
    patients[idx].gender[sizeof(patients[idx].gender) - 1] = '\0';
    strncpy(patients[idx].phone, phone, sizeof(patients[idx].phone) - 1);
    patients[idx].phone[sizeof(patients[idx].phone) - 1] = '\0';
    strncpy(patients[idx].disease, disease, sizeof(patients[idx].disease) - 1);
    patients[idx].disease[sizeof(patients[idx].disease) - 1] = '\0';
}

void setDoctor(int idx, int id, const char *name, const char *spec, const char *phone) {
    doctors[idx].id = id;
    strncpy(doctors[idx].name, name, sizeof(doctors[idx].name) - 1);
    doctors[idx].name[sizeof(doctors[idx].name) - 1] = '\0';
    strncpy(doctors[idx].specialization, spec, sizeof(doctors[idx].specialization) - 1);
    doctors[idx].specialization[sizeof(doctors[idx].specialization) - 1] = '\0';
    strncpy(doctors[idx].phone, phone, sizeof(doctors[idx].phone) - 1);
    doctors[idx].phone[sizeof(doctors[idx].phone) - 1] = '\0';
}

void setAppointment(int idx, int id, int pId, const char *pName, int dId, const char *dName, const char *date) {
    appointments[idx].id = id;
    appointments[idx].patientId = pId;
    strncpy(appointments[idx].patientName, pName, sizeof(appointments[idx].patientName) - 1);
    appointments[idx].patientName[sizeof(appointments[idx].patientName) - 1] = '\0';
    appointments[idx].doctorId = dId;
    strncpy(appointments[idx].doctorName, dName, sizeof(appointments[idx].doctorName) - 1);
    appointments[idx].doctorName[sizeof(appointments[idx].doctorName) - 1] = '\0';
    strncpy(appointments[idx].date, date, sizeof(appointments[idx].date) - 1);
    appointments[idx].date[sizeof(appointments[idx].date) - 1] = '\0';
}

void setBilling(int idx, int id, int pId, const char *pName, float consult, float med, float treat, const char *date) {
    bills[idx].id = id;
    bills[idx].patientId = pId;
    strncpy(bills[idx].patientName, pName, sizeof(bills[idx].patientName) - 1);
    bills[idx].patientName[sizeof(bills[idx].patientName) - 1] = '\0';
    bills[idx].consultationFee = consult;
    bills[idx].medicineCharges = med;
    bills[idx].treatmentCharges = treat;
    bills[idx].totalBill = consult + med + treat;
    strncpy(bills[idx].date, date, sizeof(bills[idx].date) - 1);
    bills[idx].date[sizeof(bills[idx].date) - 1] = '\0';
}

void initializeSampleData(void) {
    patientCount = 0;
    setPatient(patientCount++, 101, "Rahul Sharma", 32, "Male", "9876543210", "Viral Fever & Fatigue");
    setPatient(patientCount++, 102, "Priya Patel", 27, "Female", "9823456789", "Migraine & Tension");
    setPatient(patientCount++, 103, "Amit Verma", 54, "Male", "9123456780", "Hypertension & Diabetes");
    setPatient(patientCount++, 104, "Sneha Kulkarni", 41, "Female", "9988776655", "Orthopedic Knee Pain");
    nextPatientId = 105;

    doctorCount = 0;
    setDoctor(doctorCount++, 201, "Aditi Roy", "Cardiology", "9845011223");
    setDoctor(doctorCount++, 202, "Vikram Mehta", "Neurology", "9819022334");
    setDoctor(doctorCount++, 203, "Rajesh Nair", "Orthopedics", "9769033445");
    setDoctor(doctorCount++, 204, "Sunita Rao", "General Medicine", "9821044556");
    nextDoctorId = 205;

    appointmentCount = 0;
    setAppointment(appointmentCount++, 301, 101, "Rahul Sharma", 204, "Sunita Rao", "2026-11-05");
    setAppointment(appointmentCount++, 302, 102, "Priya Patel", 202, "Vikram Mehta", "2026-11-06");
    setAppointment(appointmentCount++, 303, 104, "Sneha Kulkarni", 203, "Rajesh Nair", "2026-11-07");
    nextApptId = 304;

    billCount = 0;
    setBilling(billCount++, 401, 101, "Rahul Sharma", 500.0f, 650.0f, 250.0f, "2026-11-02");
    setBilling(billCount++, 402, 103, "Amit Verma", 600.0f, 1200.0f, 800.0f, "2026-11-02");
    nextBillId = 403;
}

void saveDataToFile(void) {
    FILE *fp = fopen("hospital_data.txt", "w");
    int i;
    if (!fp) return;

    fprintf(fp, "[PATIENTS] %d\n", patientCount);
    for (i = 0; i < patientCount; i++) {
        fprintf(fp, "%d|%s|%d|%s|%s|%s\n",
                patients[i].id, patients[i].name, patients[i].age,
                patients[i].gender, patients[i].phone, patients[i].disease);
    }

    fprintf(fp, "[DOCTORS] %d\n", doctorCount);
    for (i = 0; i < doctorCount; i++) {
        fprintf(fp, "%d|%s|%s|%s\n",
                doctors[i].id, doctors[i].name, doctors[i].specialization, doctors[i].phone);
    }

    fprintf(fp, "[APPOINTMENTS] %d\n", appointmentCount);
    for (i = 0; i < appointmentCount; i++) {
        fprintf(fp, "%d|%d|%s|%d|%s|%s\n",
                appointments[i].id, appointments[i].patientId, appointments[i].patientName,
                appointments[i].doctorId, appointments[i].doctorName, appointments[i].date);
    }

    fprintf(fp, "[BILLS] %d\n", billCount);
    for (i = 0; i < billCount; i++) {
        fprintf(fp, "%d|%d|%s|%.2f|%.2f|%.2f|%.2f|%s\n",
                bills[i].id, bills[i].patientId, bills[i].patientName,
                bills[i].consultationFee, bills[i].medicineCharges,
                bills[i].treatmentCharges, bills[i].totalBill, bills[i].date);
    }

    fclose(fp);
}

void loadDataFromFile(void) {
    FILE *fp = fopen("hospital_data.txt", "r");
    char line[256];
    char *token;
    int count, i;

    if (!fp) return;

    if (fscanf(fp, "[PATIENTS] %d\n", &count) == 1) {
        patientCount = 0;
        for (i = 0; i < count && i < MAX_PATIENTS; i++) {
            if (fgets(line, sizeof(line), fp)) {
                Patient p;
                line[strcspn(line, "\r\n")] = '\0';
                token = strtok(line, "|"); if (token) p.id = atoi(token);
                token = strtok(NULL, "|"); if (token) strncpy(p.name, token, sizeof(p.name));
                token = strtok(NULL, "|"); if (token) p.age = atoi(token);
                token = strtok(NULL, "|"); if (token) strncpy(p.gender, token, sizeof(p.gender));
                token = strtok(NULL, "|"); if (token) strncpy(p.phone, token, sizeof(p.phone));
                token = strtok(NULL, "|"); if (token) strncpy(p.disease, token, sizeof(p.disease));
                patients[patientCount++] = p;
                if (p.id >= nextPatientId) nextPatientId = p.id + 1;
            }
        }
    }

    if (fscanf(fp, "[DOCTORS] %d\n", &count) == 1) {
        doctorCount = 0;
        for (i = 0; i < count && i < MAX_DOCTORS; i++) {
            if (fgets(line, sizeof(line), fp)) {
                Doctor d;
                line[strcspn(line, "\r\n")] = '\0';
                token = strtok(line, "|"); if (token) d.id = atoi(token);
                token = strtok(NULL, "|"); if (token) strncpy(d.name, token, sizeof(d.name));
                token = strtok(NULL, "|"); if (token) strncpy(d.specialization, token, sizeof(d.specialization));
                token = strtok(NULL, "|"); if (token) strncpy(d.phone, token, sizeof(d.phone));
                doctors[doctorCount++] = d;
                if (d.id >= nextDoctorId) nextDoctorId = d.id + 1;
            }
        }
    }

    if (fscanf(fp, "[APPOINTMENTS] %d\n", &count) == 1) {
        appointmentCount = 0;
        for (i = 0; i < count && i < MAX_APPOINTMENTS; i++) {
            if (fgets(line, sizeof(line), fp)) {
                Appointment a;
                line[strcspn(line, "\r\n")] = '\0';
                token = strtok(line, "|"); if (token) a.id = atoi(token);
                token = strtok(NULL, "|"); if (token) a.patientId = atoi(token);
                token = strtok(NULL, "|"); if (token) strncpy(a.patientName, token, sizeof(a.patientName));
                token = strtok(NULL, "|"); if (token) a.doctorId = atoi(token);
                token = strtok(NULL, "|"); if (token) strncpy(a.doctorName, token, sizeof(a.doctorName));
                token = strtok(NULL, "|"); if (token) strncpy(a.date, token, sizeof(a.date));
                appointments[appointmentCount++] = a;
                if (a.id >= nextApptId) nextApptId = a.id + 1;
            }
        }
    }

    if (fscanf(fp, "[BILLS] %d\n", &count) == 1) {
        billCount = 0;
        for (i = 0; i < count && i < MAX_BILLS; i++) {
            if (fgets(line, sizeof(line), fp)) {
                Billing b;
                line[strcspn(line, "\r\n")] = '\0';
                token = strtok(line, "|"); if (token) b.id = atoi(token);
                token = strtok(NULL, "|"); if (token) b.patientId = atoi(token);
                token = strtok(NULL, "|"); if (token) strncpy(b.patientName, token, sizeof(b.patientName));
                token = strtok(NULL, "|"); if (token) b.consultationFee = (float)atof(token);
                token = strtok(NULL, "|"); if (token) b.medicineCharges = (float)atof(token);
                token = strtok(NULL, "|"); if (token) b.treatmentCharges = (float)atof(token);
                token = strtok(NULL, "|"); if (token) b.totalBill = (float)atof(token);
                token = strtok(NULL, "|"); if (token) strncpy(b.date, token, sizeof(b.date)); else strcpy(b.date, "2026-11-02");
                bills[billCount++] = b;
                if (b.id >= nextBillId) nextBillId = b.id + 1;
            }
        }
    }

    fclose(fp);
}

/* ============================================================================
 * 4. C JSON SERIALIZER & HTTP RESPONSES
 * ============================================================================ */

/* Serializes the current C arrays into a single JSON object */
int serializeDatabaseToJson(char *buffer, int maxLen) {
    int offset = 0;
    int i;

    offset += snprintf(buffer + offset, maxLen - offset, "{\"patients\":[");
    for (i = 0; i < patientCount; i++) {
        offset += snprintf(buffer + offset, maxLen - offset,
                           "{\"id\":%d,\"name\":\"%s\",\"age\":%d,\"gender\":\"%s\",\"phone\":\"%s\",\"disease\":\"%s\"}%s",
                           patients[i].id, patients[i].name, patients[i].age,
                           patients[i].gender, patients[i].phone, patients[i].disease,
                           (i < patientCount - 1) ? "," : "");
    }

    offset += snprintf(buffer + offset, maxLen - offset, "],\"doctors\":[");
    for (i = 0; i < doctorCount; i++) {
        offset += snprintf(buffer + offset, maxLen - offset,
                           "{\"id\":%d,\"name\":\"%s\",\"specialization\":\"%s\",\"phone\":\"%s\"}%s",
                           doctors[i].id, doctors[i].name, doctors[i].specialization, doctors[i].phone,
                           (i < doctorCount - 1) ? "," : "");
    }

    offset += snprintf(buffer + offset, maxLen - offset, "],\"appointments\":[");
    for (i = 0; i < appointmentCount; i++) {
        offset += snprintf(buffer + offset, maxLen - offset,
                           "{\"id\":%d,\"patientId\":%d,\"patientName\":\"%s\",\"doctorId\":%d,\"doctorName\":\"%s\",\"date\":\"%s\"}%s",
                           appointments[i].id, appointments[i].patientId, appointments[i].patientName,
                           appointments[i].doctorId, appointments[i].doctorName, appointments[i].date,
                           (i < appointmentCount - 1) ? "," : "");
    }

    offset += snprintf(buffer + offset, maxLen - offset, "],\"bills\":[");
    for (i = 0; i < billCount; i++) {
        offset += snprintf(buffer + offset, maxLen - offset,
                           "{\"id\":%d,\"patientId\":%d,\"patientName\":\"%s\",\"consultation\":%.2f,\"medicine\":%.2f,\"treatment\":%.2f,\"total\":%.2f,\"date\":\"%s\"}%s",
                           bills[i].id, bills[i].patientId, bills[i].patientName,
                           bills[i].consultationFee, bills[i].medicineCharges, bills[i].treatmentCharges,
                           bills[i].totalBill, bills[i].date,
                           (i < billCount - 1) ? "," : "");
    }
    offset += snprintf(buffer + offset, maxLen - offset, "]}");

    return offset;
}

void sendHttpResponse(SOCKET clientSocket, const char *contentType, const char *body, int bodyLen) {
    char header[512];
    int headerLen = sprintf(header,
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: %s\r\n"
                            "Content-Length: %d\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                            "Access-Control-Allow-Headers: Content-Type\r\n"
                            "Connection: close\r\n\r\n",
                            contentType, bodyLen);

    send(clientSocket, header, headerLen, 0);
    send(clientSocket, body, bodyLen, 0);
}

void serveStaticFile(SOCKET clientSocket, const char *filePath) {
    char fullPath[256];
    FILE *file;
    char buffer[4096];
    long fileSize;
    const char *mimeType = "text/plain";
    char header[512];
    size_t bytesRead;

    if (strcmp(filePath, "/") == 0 || strcmp(filePath, "") == 0) {
        strcpy(fullPath, "index.html");
    } else {
        if (filePath[0] == '/') strncpy(fullPath, filePath + 1, sizeof(fullPath) - 1);
        else strncpy(fullPath, filePath, sizeof(fullPath) - 1);
        fullPath[sizeof(fullPath) - 1] = '\0';
    }

    if (strstr(fullPath, ".html")) mimeType = "text/html; charset=UTF-8";
    else if (strstr(fullPath, ".css")) mimeType = "text/css; charset=UTF-8";
    else if (strstr(fullPath, ".js")) mimeType = "application/javascript; charset=UTF-8";
    else if (strstr(fullPath, ".png")) mimeType = "image/png";

    file = fopen(fullPath, "rb");
    if (!file) {
        const char *notFound = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
        send(clientSocket, notFound, (int)strlen(notFound), 0);
        return;
    }

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n\r\n",
            mimeType, fileSize);
    send(clientSocket, header, (int)strlen(header), 0);

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(clientSocket, buffer, (int)bytesRead, 0);
    }
    fclose(file);
}

/* Simple helper to extract string values from JSON request body */
int getJsonString(const char *json, const char *key, char *output, int maxLen) {
    char searchKey[64];
    snprintf(searchKey, sizeof(searchKey), "\"%s\":\"", key);
    char *pos = strstr(json, searchKey);
    if (!pos) {
        /* Try without quotes on key */
        snprintf(searchKey, sizeof(searchKey), "\"%s\": \"", key);
        pos = strstr(json, searchKey);
    }
    if (pos) {
        pos += strlen(searchKey);
        char *end = strchr(pos, '\"');
        if (end) {
            int len = (int)(end - pos);
            if (len >= maxLen) len = maxLen - 1;
            strncpy(output, pos, len);
            output[len] = '\0';
            return 1;
        }
    }
    output[0] = '\0';
    return 0;
}

int getJsonInt(const char *json, const char *key, int *output) {
    char searchKey[64];
    snprintf(searchKey, sizeof(searchKey), "\"%s\":", key);
    char *pos = strstr(json, searchKey);
    if (pos) {
        pos += strlen(searchKey);
        while (*pos == ' ' || *pos == '\"') pos++;
        *output = atoi(pos);
        return 1;
    }
    return 0;
}

int getJsonFloat(const char *json, const char *key, float *output) {
    char searchKey[64];
    snprintf(searchKey, sizeof(searchKey), "\"%s\":", key);
    char *pos = strstr(json, searchKey);
    if (pos) {
        pos += strlen(searchKey);
        while (*pos == ' ' || *pos == '\"') pos++;
        *output = (float)atof(pos);
        return 1;
    }
    return 0;
}

/* ============================================================================
 * 5. REQUEST ROUTER & API HANDLER
 * ============================================================================ */
void handleApiRequest(SOCKET clientSocket, const char *method, const char *path, const char *body) {
    static char jsonResponse[BUFFER_SIZE];

    /* GET /api/data - Return all hospital arrays in JSON format */
    if (strcmp(path, "/api/data") == 0 && strcmp(method, "GET") == 0) {
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        printf("  [C-ENGINE] Dispatched hospital records to web interface (Patients: %d, Doctors: %d, Appts: %d, Bills: %d)\n",
               patientCount, doctorCount, appointmentCount, billCount);
        return;
    }

    /* POST /api/patients - Register patient into C struct array */
    if (strcmp(path, "/api/patients") == 0 && strcmp(method, "POST") == 0) {
        if (patientCount < MAX_PATIENTS) {
            char name[64], gender[16], phone[20], disease[64];
            int age = 0;
            getJsonString(body, "name", name, sizeof(name));
            getJsonInt(body, "age", &age);
            getJsonString(body, "gender", gender, sizeof(gender));
            getJsonString(body, "phone", phone, sizeof(phone));
            getJsonString(body, "disease", disease, sizeof(disease));

            setPatient(patientCount, nextPatientId++, name, age, gender, phone, disease);
            printf("  [C-ENGINE] + Added Patient #%d: '%s' (%d yrs, %s) into C array!\n",
                   patients[patientCount].id, patients[patientCount].name,
                   patients[patientCount].age, patients[patientCount].disease);
            patientCount++;
            saveDataToFile();
        }
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        return;
    }

    /* POST /api/patients/delete - Delete patient from C array */
    if (strcmp(path, "/api/patients/delete") == 0 && strcmp(method, "POST") == 0) {
        int delId = 0, i, found = -1;
        getJsonInt(body, "id", &delId);
        for (i = 0; i < patientCount; i++) {
            if (patients[i].id == delId) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            printf("  [C-ENGINE] - Deleted Patient #%d ('%s') from C array!\n", delId, patients[found].name);
            for (i = found; i < patientCount - 1; i++) {
                patients[i] = patients[i + 1];
            }
            patientCount--;
            saveDataToFile();
        }
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        return;
    }

    /* POST /api/doctors - Add doctor to C struct array */
    if (strcmp(path, "/api/doctors") == 0 && strcmp(method, "POST") == 0) {
        if (doctorCount < MAX_DOCTORS) {
            char name[64], spec[64], phone[20];
            getJsonString(body, "name", name, sizeof(name));
            getJsonString(body, "specialization", spec, sizeof(spec));
            getJsonString(body, "phone", phone, sizeof(phone));

            setDoctor(doctorCount, nextDoctorId++, name, spec, phone);
            printf("  [C-ENGINE] + Added Doctor #%d: Dr. '%s' (%s) into C array!\n",
                   doctors[doctorCount].id, doctors[doctorCount].name, doctors[doctorCount].specialization);
            doctorCount++;
            saveDataToFile();
        }
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        return;
    }

    /* POST /api/appointments - Book appointment */
    if (strcmp(path, "/api/appointments") == 0 && strcmp(method, "POST") == 0) {
        if (appointmentCount < MAX_APPOINTMENTS) {
            int pId = 0, dId = 0, i;
            char pName[64] = "Unknown", dName[64] = "Unknown", date[20];
            getJsonInt(body, "patientId", &pId);
            getJsonInt(body, "doctorId", &dId);
            getJsonString(body, "date", date, sizeof(date));

            for (i = 0; i < patientCount; i++) {
                if (patients[i].id == pId) {
                    strncpy(pName, patients[i].name, sizeof(pName));
                    break;
                }
            }
            for (i = 0; i < doctorCount; i++) {
                if (doctors[i].id == dId) {
                    strncpy(dName, doctors[i].name, sizeof(dName));
                    break;
                }
            }

            setAppointment(appointmentCount, nextApptId++, pId, pName, dId, dName, date);
            printf("  [C-ENGINE] + Confirmed Appointment #%d: '%s' with Dr. '%s' on %s\n",
                   appointments[appointmentCount].id, pName, dName, date);
            appointmentCount++;
            saveDataToFile();
        }
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        return;
    }

    /* POST /api/appointments/cancel - Cancel appointment */
    if (strcmp(path, "/api/appointments/cancel") == 0 && strcmp(method, "POST") == 0) {
        int apptId = 0, i, found = -1;
        getJsonInt(body, "id", &apptId);
        for (i = 0; i < appointmentCount; i++) {
            if (appointments[i].id == apptId) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            printf("  [C-ENGINE] - Cancelled Appointment #%d from C array!\n", apptId);
            for (i = found; i < appointmentCount - 1; i++) {
                appointments[i] = appointments[i + 1];
            }
            appointmentCount--;
            saveDataToFile();
        }
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        return;
    }

    /* POST /api/bills - Generate Patient Bill in C */
    if (strcmp(path, "/api/bills") == 0 && strcmp(method, "POST") == 0) {
        if (billCount < MAX_BILLS) {
            int pId = 0, i;
            float consult = 0.0f, med = 0.0f, treat = 0.0f;
            char pName[64] = "Unknown", date[20] = "2026-11-02";
            getJsonInt(body, "patientId", &pId);
            getJsonFloat(body, "consultation", &consult);
            getJsonFloat(body, "medicine", &med);
            getJsonFloat(body, "treatment", &treat);
            getJsonString(body, "date", date, sizeof(date));
            if (date[0] == '\0') strcpy(date, "2026-11-02");

            for (i = 0; i < patientCount; i++) {
                if (patients[i].id == pId) {
                    strncpy(pName, patients[i].name, sizeof(pName));
                    break;
                }
            }

            setBilling(billCount, nextBillId++, pId, pName, consult, med, treat, date);
            printf("  [C-ENGINE] + Generated Bill INV-%d for '%s': Total Rs. %.2f (Consult: %.2f, Med: %.2f, Treat: %.2f)\n",
                   bills[billCount].id, pName, bills[billCount].totalBill, consult, med, treat);
            billCount++;
            saveDataToFile();
        }
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        return;
    }

    /* POST /api/reset - Reset to default records */
    if (strcmp(path, "/api/reset") == 0 && strcmp(method, "POST") == 0) {
        initializeSampleData();
        saveDataToFile();
        printf("  [C-ENGINE] Reset database arrays to initial sample records.\n");
        int len = serializeDatabaseToJson(jsonResponse, sizeof(jsonResponse));
        sendHttpResponse(clientSocket, "application/json", jsonResponse, len);
        return;
    }

    /* Fallback */
    const char *emptyJson = "{}";
    sendHttpResponse(clientSocket, "application/json", emptyJson, (int)strlen(emptyJson));
}

/* ============================================================================
 * 6. MAIN SERVER & BROWSER LAUNCHER
 * ============================================================================ */
int main(void) {
    /* Step 1: Initialize C arrays with sample data and load existing records */
    initializeSampleData();
    loadDataFromFile();

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[!] Error: WSAStartup failed.\n");
        return 1;
    }
#endif

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("[!] Error: Could not create socket.\n");
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("[!] Error: Port %d is already in use.\n", PORT);
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(serverSocket, 20) == SOCKET_ERROR) {
        printf("[!] Error: Listen failed.\n");
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("\n");
    printf("====================================================================\n");
    printf("        CAREWELL HOSPITAL MANAGEMENT SYSTEM - C WEB ENGINE          \n");
    printf("====================================================================\n");
    printf("  [*] Backend Core        : 100%% C Programming\n");
    printf("  [*] Data Structures     : Arrays of Structures (Patient, Doctor, etc.)\n");
    printf("  [*] Server Status       : Running on http://localhost:%d\n", PORT);
    printf("  [*] Active Patients     : %d\n", patientCount);
    printf("  [*] Active Doctors      : %d\n", doctorCount);
    printf("  [*] Scheduled Appts     : %d\n", appointmentCount);
    printf("  [*] Total Invoices      : %d\n", billCount);
    printf("--------------------------------------------------------------------\n");
    printf("  [+] Launching your web browser automatically at http://localhost:%d ...\n", PORT);
    printf("  [+] Press Ctrl+C in this terminal anytime to stop the server.\n");
    printf("====================================================================\n\n");

    /* Automatically launch the website in default browser */
#ifdef _WIN32
    ShellExecuteA(NULL, "open", "http://localhost:8080", NULL, NULL, SW_SHOWNORMAL);
#else
    system("xdg-open http://localhost:8080 > /dev/null 2>&1 &");
#endif

    /* Server event loop */
    while (1) {
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket != INVALID_SOCKET) {
            char request[BUFFER_SIZE];
            int bytesReceived = recv(clientSocket, request, sizeof(request) - 1, 0);

            if (bytesReceived > 0) {
                request[bytesReceived] = '\0';

                char method[16], path[256];
                if (sscanf(request, "%15s %255s", method, path) == 2) {
                    /* Handle CORS preflight OPTIONS */
                    if (strcmp(method, "OPTIONS") == 0) {
                        const char *corsHeader = "HTTP/1.1 204 No Content\r\n"
                                                 "Access-Control-Allow-Origin: *\r\n"
                                                 "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                                                 "Access-Control-Allow-Headers: Content-Type\r\n"
                                                 "Content-Length: 0\r\n"
                                                 "Connection: close\r\n\r\n";
                        send(clientSocket, corsHeader, (int)strlen(corsHeader), 0);
                    }
                    /* Route API endpoints directly to C logic */
                    else if (strncmp(path, "/api/", 5) == 0) {
                        char *body = strstr(request, "\r\n\r\n");
                        if (body) body += 4;
                        else body = "";
                        handleApiRequest(clientSocket, method, path, body);
                    }
                    /* Route static files (index.html, style.css, script.js) */
                    else {
                        serveStaticFile(clientSocket, path);
                    }
                }
            }
            closesocket(clientSocket);
        }
    }

    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
