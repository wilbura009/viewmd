#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

void preview_file_changes(const char* filename)
{
  int length, i = 0;
  int fd;
  int wd;
  char buffer[BUF_LEN];

  // Initialize inotify
  fd = inotify_init();
  if (fd < 0) {
    perror("inotify_init");
    return;
  }

  // Add watch for file modifications
  wd = inotify_add_watch(fd, filename, IN_MODIFY);
  if (wd < 0) {
    perror("inotify_add_watch");
    return;
  }

  printf("Watching file: %s\n", filename);

  while (1) {
    // Read events
    length = read(fd, buffer, BUF_LEN);
    if (length < 0) {
      perror("read");
      break;
    }

    // Process each event
    while (i < length) {
      struct inotify_event* event = (struct inotify_event*)&buffer[i];
      if (event->mask & IN_MODIFY) {
        printf("File modified: %s\n", event->name);
        FILE* file = fopen(filename, "r");
        if (file != NULL) {
          char ch;
          while ((ch = fgetc(file)) != EOF) {
            putchar(ch);
          }
          fclose(file);
          printf("\n");
        }
      }
      i += EVENT_SIZE + event->len;
    }
    i = 0;
  }

  // Clean up
  inotify_rm_watch(fd, wd);
  close(fd);
}

int main()
{
  char filename[256];
  printf("Enter the filename to preview: ");
  fgets(filename, sizeof(filename), stdin);
  filename[strcspn(filename, "\n")] = '\0';  // Remove trailing newline

  preview_file_changes(filename);

  return 0;
}
