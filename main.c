#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>

void processBuffer(char* data, char key, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key;
    }
}

// ฟังก์ชันสำหรับเลือกไฟล์ผ่าน Windows Dialog
BOOL selectFile(char* filePath, DWORD flags, BOOL saveDialog) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = flags;
    ofn.lpstrDefExt = "oms";

    if (saveDialog) {
        return GetSaveFileName(&ofn);
    } else {
        return GetOpenFileName(&ofn);
    }
}

int main() {
    char key = 0x5A;
    int choice;

    printf("Choose mode:\n");
    printf("1. Encode (เข้ารหัส)\n");
    printf("2. Decode (ถอดรหัส)\n");
    printf("Enter choice (1/2): ");
    scanf("%d", &choice);
    getchar(); // clear newline

    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        return 1;
    }

    char openPath[MAX_PATH] = "";
    if (!selectFile(openPath, OFN_FILEMUSTEXIST, FALSE)) {
        printf("Cancelled selecting source file.\n");
        return 1;
    }

    FILE* inFile = fopen(openPath, "rb");
    if (!inFile) {
        perror("Failed to open input file");
        return 1;
    }

    fseek(inFile, 0, SEEK_END);
    long size = ftell(inFile);
    rewind(inFile);

    char* buffer = (char*)malloc(size);
    fread(buffer, 1, size, inFile);
    fclose(inFile);

    processBuffer(buffer, key, size);

    char savePath[MAX_PATH] = "";
    if (!selectFile(savePath, OFN_OVERWRITEPROMPT, TRUE)) {
        printf("Cancelled selecting save location.\n");
        free(buffer);
        return 1;
    }

    FILE* outFile = fopen(savePath, "wb");
    if (!outFile) {
        perror("Failed to save output file");
        free(buffer);
        return 1;
    }

    fwrite(buffer, 1, size, outFile);
    fclose(outFile);
    free(buffer);

    if (choice == 1) {
        printf("File encrypted and saved to: %s\n", savePath);
    } else {
        printf("File decrypted and saved to: %s\n", savePath);
    }

    return 0;
}
