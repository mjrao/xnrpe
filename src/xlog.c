#include "xlog.h"

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


char *log_file = NULL;
FILE *log_fp = NULL;

char *strip(char *buffer)
{
	int x;
	int index;
	char *buf = buffer;

	for (x = strlen(buffer); x >= 1; x--) 
    {
		index = x - 1;
		if (buffer[index] == ' ' || buffer[index] == '\r' || buffer[index] == '\n'
			|| buffer[index] == '\t')
			buffer[index] = '\x0';
		else
			break;
	}

	while (*buf == ' ' || *buf == '\r' || *buf == '\n' || *buf == '\t') 
    {
		++buf;
		--x;
	}
	if (buf != buffer) 
    {
		memmove(buffer, buf, x);
		buffer[x] = '\x0';
	}

	return buffer;
}

void open_log_file()
{
	int fh;
	struct stat st;

	close_log_file();

	if(log_file == NULL)
	{
		log_file = strdup(LOG_DEFAULT_FILE);
	}
	if (!log_file)
		return;

	if ((fh = open(log_file, O_RDWR|O_APPEND|O_CREAT|O_NOFOLLOW, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1) 
    {
		printf("Warning: Cannot open log file '%s' for writing\n", log_file);
		logit(LOG_WARNING, "Warning: Cannot open log file '%s' for writing", log_file);
		return;
	}
	log_fp = fdopen(fh, "a+");
	if(log_fp == NULL) 
    {
		printf("Warning: Cannot open log file '%s' for writing\n", log_file);
		logit(LOG_WARNING, "Warning: Cannot open log file '%s' for writing", log_file);
		return;
		}

	if ((fstat(fh, &st)) == -1) 
    {
		log_fp = NULL;
		close(fh);
		printf("Warning: Cannot fstat log file '%s'\n", log_file);
		logit(LOG_WARNING, "Warning: Cannot fstat log file '%s'", log_file);
		return;
	}
	if (st.st_nlink != 1 || (st.st_mode & S_IFMT) != S_IFREG) 
    {
		log_fp = NULL;
		close(fh);
		printf("Warning: log file '%s' has an invalid mode\n", log_file);
		logit(LOG_WARNING, "Warning: log file '%s' has an invalid mode", log_file);
		return;
	}

	(void)fcntl(fileno(log_fp), F_SETFD, FD_CLOEXEC);
}

void logit(int priority, const char *format, ...)
{
	time_t	log_time = 0L;
	va_list	ap;
	char	*buffer = NULL;
	char timebuf[50];
	if (!format || !*format)
		return;

	va_start(ap, format);
	if(vasprintf(&buffer, format, ap) > 0) 
    {
		if (log_fp) 
        {
			time_t t = time(&log_time);
			/* strip any newlines from the end of the buffer */
			strip(buffer);
			strftime(timebuf, 50, "%Y%m%d%H%M%S", localtime(&t));
			/* write the buffer to the log file */
			//fprintf(log_fp, "[%llu] %s\n", (unsigned long long)log_time, buffer);
			fprintf(log_fp, "[%s] %s\n", timebuf, buffer);
			fflush(log_fp);

		} else
			syslog(priority, buffer);

		free(buffer);
	}
	va_end(ap);
}

void close_log_file()
{
	if(!log_fp)
		return;

	fflush(log_fp);
	fclose(log_fp);
	log_fp = NULL;
	if(log_file != NULL)
	{
		free(log_file);
		log_file = NULL;
	}
	return;
}