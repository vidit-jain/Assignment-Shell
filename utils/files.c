#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <regex.h>
#include "files.h"
#include "string.h"

#define MAX_LEN 2000

int folderExists(String path)
{
	struct stat sb;
	return stat(path.str, &sb) == 0 && S_ISDIR(sb.st_mode);
}

int fileExists(String path)
{
	struct stat sb;
	return stat(path.str, &sb) == 0 && S_ISREG(sb.st_mode);
}

int deleteFile(String path)
{
	if (!fileExists(path))
	{
		return 0;
	}
	String *command = make_empty_String();
	sprintf(command->str, "rm %s > /dev/null", path.str);
	system(command->str);
	return 1;
}

int deleteFolder(String path)
{
	if (!folderExists(path))
	{
		return 0;
	}
	String *command = make_empty_String();
	sprintf(command->str, "rm -r %s > /dev/null", path.str);
	system(command->str);
	return 1;
}

String *find_zip(String *file)
{
	if (fileExists(*file))
	{
		return file;
	}
}

int createZip(String path, String zipName)
{
	if (!folderExists(path))
	{
		return 0;
	}
	String *command = make_empty_String();
	sprintf(command->str, "zip -r %s.zip %s > /dev/null", zipName.str, path.str);
	system(command->str);
	return 1;
}
int unzipToDirectory(String zipName, String path)  {
    if (!fileExists(zipName)){
        return 0;
    }
    String* command = make_empty_String();
    sprintf(command->str, "unzip %s -d %s > /dev/null", zipName.str, path.str);
    system(command->str);
    return 1;
}

void folderHash(String assignmentName, String hashName) {
    String* command = make_empty_String();
    sprintf(command->str,"find %s -type f -print0 | sort -z | xargs -r0 md5sum > %s", assignmentName.str, hashName.str);
    system(command->str);
}
void zipHash(String fileName, String hashName) {
    unzipToDirectory(fileName, *make_String("TemporaryMD5/"));
    folderHash(*make_String("TemporaryMD5/"), hashName);
    deleteFolder(*make_String("TemporaryMD5"));
}

int createFolder(String folder)
{
	if (folderExists(folder))
	{
		return 0;
	}

	String *command = make_empty_String();
	sprintf(command->str, "mkdir %s > /dev/null", folder.str);
	system(command->str);
	return 1;
}
int validFileName(String name)
{
	String *fileRegex = make_String("[^-_.A-Za-z0-9]");
	regex_t regex;
	int value = regcomp(&regex, fileRegex->str, 0);
	value = regexec(&regex, name.str, 0, NULL, 0) == 0;
	return !value;
}

int countLines(String fileName)
{

	if (!fileExists(fileName))
		return -1;

	FILE *fp = fopen(fileName.str, "r");

	String *dummyString = make_empty_String();
	size_t stringLength = 0;
	ssize_t readFile;

	int numberOfLines = 0;

	while ((readFile = getline(&(dummyString->str), &stringLength, fp)) != -1)
	{
		numberOfLines++;
	}
	fclose(fp);

	return numberOfLines;
}
String *getCurrentSubject()
{
	String *homePath;
	homePath = make_empty_String();

	getcwd(homePath->str, MAX_LEN);
	int strLen = strlen(homePath->str);
	String *currSubject = make_empty_String();

	if (isInSubject)
	{
		int lastForward = -1;
		for (int i = 0; i < strLen; i++)
		{
			if (homePath->str[i] == '/')
				lastForward = i;
		}
		int counter = 0;
		for (int i = lastForward + 1; i < strLen; i++)
		{
			currSubject->str[counter] = homePath->str[i];
			counter++;
		}
		currSubject->str[counter] = '\0';
	}
	return currSubject;
}
void enterSubjectDirectory()
{
	chdir("Subjects");
}

void get_serverpath()
{
	if (isInSubject)
	{
		server_path = make_String("../../Server");
	}

	else
	{
		server_path = make_String("../Server");
	}
}

int copy_to_server(String *zipfile)
{
	String *home_path = make_empty_String();

	getcwd(home_path->str, MAX_LEN);

	get_serverpath();

	String *command = make_empty_String();

	sprintf(command->str, "cp %s %s/%s > /dev/null",zipfile->str, server_path->str, getCurrentSubject()->str);
	system(command->str);

	return 1;
}