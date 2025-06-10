#include "file.h"

#include <linux/limits.h>
#include <sys/inotify.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int inotifyfd = -1;
pthread_t watcher_thread;

struct watching_file {
	int wd;
	const char *name;
	void *callback;
};

size_t files_size = 0;
struct watching_file *files = NULL;

struct watching_file *get_watching_file(int wd)
{
	for (size_t i = 0; i < files_size; i++) {
		if (files[i].wd == wd) {
			return &files[i];
		}
	}

	return NULL;
}

void *watcher(void *arg)
{
	size_t size = sizeof(struct inotify_event) + NAME_MAX + 1;
	struct inotify_event *event = malloc(size);
	while (1) {
		read(inotifyfd, event, size);

		int wd = event->wd;
		struct watching_file *file = get_watching_file(wd);
		if (file == NULL) {
			continue;
		}

		int replaced = (event->mask & IN_IGNORED)
			       && (access(file->name, F_OK) == 0);

		if (replaced) {
			inotify_rm_watch(inotifyfd, wd);
			file->wd = inotify_add_watch(inotifyfd, file->name,
						     IN_MODIFY);
		}

		if ((event->mask & IN_MODIFY) || replaced) {
			((void (*)(const char *))file->callback)(file->name);
		}
	}
	return NULL;
}

void watch_file(const char *filename, void (*callback)(const char *))
{
	if (inotifyfd == -1) {
		inotifyfd = inotify_init();
		pthread_create(&watcher_thread, NULL, watcher, NULL);
		pthread_detach(watcher_thread);
	}

	if (files == NULL) {
		files = malloc(sizeof(struct watching_file));
	}

	int wd = inotify_add_watch(inotifyfd, filename, IN_MODIFY);

	files = realloc(files, (files_size + 1) * sizeof(struct watching_file));
	files[files_size].callback = callback;
	files[files_size].name = filename;
	files[files_size].wd = wd;
	files_size++;
}

void read_file(const char *filename, char **store, size_t *len)
{
	FILE *file = fopen(filename, "r");
	fseek(file, 0, SEEK_END);
	size_t llen = ftell(file);
	*store = calloc(llen + 1, sizeof(char));
	fseek(file, 0, SEEK_SET);
	fread(*store, llen, 1, file);
	fclose(file);
	if (len != NULL) {
		*len = llen;
	}
}

void read_until_char(char *store, char till, FILE *stream)
{
	uint32_t i = 0;
	for (;;) {
		char c = fgetc(stream);
		if (till != '\n' && c == '\n') {
			ungetc(c, stream);
			break;
		}
		if (c == till || c == '\n' || feof(stream)) {
			break;
		}
		store[i] = c;
		i++;
	}
	store[i] = '\0';
}
