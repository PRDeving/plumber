#ifndef H_FILEEXPLORER
#define H_FILEEXPLORER

namespace fs {

  struct s_file {
    TCHAR name[128];
    TCHAR path[MAX_PATH];
  };

  struct s_folder;
  struct s_folder {
    TCHAR name[128];
    TCHAR path[MAX_PATH];
    int cfolders;
    int cfiles;
    struct s_folder * folders;
    struct s_file * files;
  };

  void ls(struct s_folder * folder, char * path) {
    char fpath[MAX_PATH];
    sprintf(fpath, "%s*.*", path);
    printf("ls in %s", fpath);

    HANDLE hFind;
    WIN32_FIND_DATA data;

    folder -> cfolders = 0;
    folder -> cfiles = 0;
    folder -> folders = (struct s_folder *) calloc(sizeof(struct s_folder), 1);
    folder -> files = (struct s_file *) calloc(sizeof(struct s_file), 1);
    strcpy(folder -> path, path);

    hFind = FindFirstFile(fpath, &data);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            folder -> cfolders++;
            folder -> folders = (struct s_folder*) realloc(folder -> folders, sizeof(struct s_folder) * folder -> cfolders);
            sprintf(folder -> folders[folder -> cfolders - 1].name, "%s", data.cFileName);
            sprintf(folder -> folders[folder -> cfolders - 1].path, "%s\\%s", path, data.cFileName);
        } else {
            folder -> cfiles++;
            folder -> files = (struct s_file*) realloc(folder -> files, sizeof(struct s_file) * folder -> cfiles);
            sprintf(folder -> files[folder -> cfiles - 1].name, "%s", data.cFileName);
            sprintf(folder -> files[folder -> cfiles - 1].path, "%s\\%s", path, data.cFileName);
        }
      } while (FindNextFile(hFind, &data));
      FindClose(hFind);
    }
  }

  void print(struct s_folder * folder) {
    printf("\n%s\n", folder -> path);
    int c = 0;
    do {
      printf(".\\%s\n", folder -> folders[c].name);
      c++;
    } while (c < folder -> cfolders);
    c = 0;
    do {
      printf("%s\n", folder -> files[c].name);
      c++;
    } while (c < folder -> cfiles);
  }

  char* serialize(struct s_folder * folder) {
    char *bf = (char*)malloc(sizeof(char) * 10240);
    sprintf(bf, "[");
    int c = 0;

    char i[64];
    do {
      sprintf(i, "\"./%s\",", folder -> folders[c].name);
      strcat(bf, i);
      c++;
    } while (c < folder -> cfolders);
    c = 0;
    do {
      sprintf(i, "\"%s\",", folder -> files[c].name);
      strcat(bf, i);
      c++;
    } while (c < folder -> cfiles);
    strcat(bf, "0]");
    return bf;
  }

  void test() {
    struct s_folder folder;
    ls(&folder, (char *)"C:\\");
    print(&folder);
  }
}

#endif
