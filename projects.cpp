#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>
#include <vector>
#include <fstream>
using namespace std;

string getPerm(mode_t m) {
    string p;
    p += (S_ISDIR(m)) ? 'd' : '-';
    p += (m & S_IRUSR) ? 'r' : '-';
    p += (m & S_IWUSR) ? 'w' : '-';
    p += (m & S_IXUSR) ? 'x' : '-';
    p += (m & S_IRGRP) ? 'r' : '-';
    p += (m & S_IWGRP) ? 'w' : '-';
    p += (m & S_IXGRP) ? 'x' : '-';
    p += (m & S_IROTH) ? 'r' : '-';
    p += (m & S_IWOTH) ? 'w' : '-';
    p += (m & S_IXOTH) ? 'x' : '-';
    return p;
}

void showDir() {
    char loc[1024];
    getcwd(loc, sizeof(loc));
    DIR *dp;
    struct dirent *ent;
    struct stat flStat;
    dp = opendir(loc);
    if (!dp) {
        perror("Unable to open directory");
        return;
    }
    cout << "\nCurrent Directory: " << loc << endl;
    cout << "---------------------------------------------------------------\n";
    cout << setw(25) << left << "Name" << setw(12) << "Type" << "Permissions" << endl;
    cout << "---------------------------------------------------------------\n";
    while ((ent = readdir(dp)) != nullptr) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        string pth = string(loc) + "/" + ent->d_name;
        stat(pth.c_str(), &flStat);
        string tp = (S_ISDIR(flStat.st_mode)) ? "DIR" : "FILE";
        string prm = getPerm(flStat.st_mode);
        cout << setw(25) << left << ent->d_name << setw(12) << tp << prm << endl;
    }
    closedir(dp);
}

void goToDir(const string &nm) {
    if (chdir(nm.c_str()) == 0)
        cout << "Moved into directory: " << nm << endl;
    else
        perror("Failed to change directory");
}

void cpFile(const string &s, const string &d) {
    ifstream in(s, ios::binary);
    ofstream out(d, ios::binary);
    if (!in) {
        cout << "Source file not found!\n";
        return;
    }
    if (!out) {
        cout << "Failed to create destination file!\n";
        return;
    }
    out << in.rdbuf();
    cout << "File copied successfully!\n";
}

void mvFile(const string &s, const string &d) {
    if (rename(s.c_str(), d.c_str()) == 0)
        cout << "File moved successfully!\n";
    else
        perror("Failed to move file");
}

void delFile(const string &f) {
    if (remove(f.c_str()) == 0)
        cout << "File deleted successfully!\n";
    else
        perror("Failed to delete file");
}

void newFile(const string &f) {
    ofstream fl(f);
    if (fl)
        cout << "File created successfully: " << f << endl;
    else
        cout << "Failed to create file!\n";
    fl.close();
}

void findFile(const string &base, const string &t) {
    DIR *dp;
    struct dirent *ent;
    dp = opendir(base.c_str());
    if (!dp) return;
    while ((ent = readdir(dp)) != nullptr) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        string pth = base + "/" + ent->d_name;
        if (strstr(ent->d_name, t.c_str()) != nullptr)
            cout << "Found: " << pth << endl;
        if (ent->d_type == DT_DIR)
            findFile(pth, t);
    }
    closedir(dp);
}

void seePerm(const string &f) {
    struct stat fs;
    if (stat(f.c_str(), &fs) == 0)
        cout << "Permissions for " << f << ": " << getPerm(fs.st_mode) << endl;
    else
        perror("Failed to get file info");
}

void setPerm(const string &f, const string &pr) {
    mode_t m = 0;
    if (pr.size() != 9) {
        cout << "Invalid format! Use 9 characters like rwxr-xr--\n";
        return;
    }
    if (pr[0] == 'r') m |= S_IRUSR;
    if (pr[1] == 'w') m |= S_IWUSR;
    if (pr[2] == 'x') m |= S_IXUSR;
    if (pr[3] == 'r') m |= S_IRGRP;
    if (pr[4] == 'w') m |= S_IWGRP;
    if (pr[5] == 'x') m |= S_IXGRP;
    if (pr[6] == 'r') m |= S_IROTH;
    if (pr[7] == 'w') m |= S_IWOTH;
    if (pr[8] == 'x') m |= S_IXOTH;
    if (chmod(f.c_str(), m) == 0)
        cout << "Permissions updated successfully!\n";
    else
        perror("Failed to change permissions");
}

int main() {
    char ch;
    string nm1, nm2;
    cout << "=== Console File Explorer ===" << endl;
    while (true) {
        showDir();
        cout << "\nOptions:\n";
        cout << " [C] Change directory\n";
        cout << " [B] Go back (Parent directory)\n";
        cout << " [1] Copy file\n";
        cout << " [2] Move file\n";
        cout << " [3] Delete file\n";
        cout << " [4] Create new file\n";
        cout << " [5] Search file/folder\n";
        cout << " [6] View file permissions\n";
        cout << " [7] Change file permissions\n";
        cout << " [Q] Quit\n";
        cout << "Enter choice: ";
        cin >> ch;
        switch (ch) {
            case 'C': case 'c':
                cout << "Enter directory name: ";
                cin >> nm1;
                goToDir(nm1);
                break;
            case 'B': case 'b':
                goToDir("..");
                break;
            case '1':
                cout << "Enter source filename: ";
                cin >> nm1;
                cout << "Enter destination filename: ";
                cin >> nm2;
                cpFile(nm1, nm2);
                break;
            case '2':
                cout << "Enter source filename: ";
                cin >> nm1;
                cout << "Enter destination filename: ";
                cin >> nm2;
                mvFile(nm1, nm2);
                break;
            case '3':
                cout << "Enter filename to delete: ";
                cin >> nm1;
                delFile(nm1);
                break;
            case '4':
                cout << "Enter new filename: ";
                cin >> nm1;
                newFile(nm1);
                break;
            case '5':
                cout << "Enter file/folder name to search: ";
                cin >> nm1;
                {
                    char loc[1024];
                    getcwd(loc, sizeof(loc));
                    cout << "Searching in: " << loc << endl;
                    findFile(loc, nm1);
                }
                break;
            case '6':
                cout << "Enter filename to view permissions: ";
                cin >> nm1;
                seePerm(nm1);
                break;
            case '7':
                cout << "Enter filename to change permissions: ";
                cin >> nm1;
                cout << "Enter new permissions (like rwxr-xr--): ";
                cin >> nm2;
                setPerm(nm1, nm2);
                break;
            case 'Q': case 'q':
                cout << "Exiting File Explorer. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    }
}
