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

void IFsubmission_folder(String assignment_folder, String *zipfile)
{
	String *submission_folder;

	submission_folder = make_String("../../Server/");
	submission_folder = attach_String(submission_folder->str, getCurrentSubject()->str);
	submission_folder = attach_String(submission_folder->str, "/");
	submission_folder = attach_String(submission_folder->str, assignment_folder.str);
	submission_folder = attach_String(submission_folder->str, "/submissions");

	if (!folderExists(*submission_folder))
	{
		createFolder(*submission_folder);
	}
}

int zipexists(String folder)
{
	DIR *d;
	struct dirent *dir;
	d = opendir("../Server");

	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
			{
				if (strcmp(folder.str, dir->d_name) == 0)
				{
					return 1;
				}
			}
		}

		closedir(d);
	}
	return 0;
}

int copy_to_server(String *zipfile, String assignment_folder)
{
	String *home_path = make_empty_String();

	getcwd(home_path->str, MAX_LEN);

	String *path = make_empty_String();
	path->str = "../../Server/";

	path = attach_String(path->str, getCurrentSubject()->str);
	path = attach_String(path->str, "/");
	path = attach_String(path->str, assignment_folder.str);
	path = attach_String(path->str, "/submissions/");

	path = attach_String(path->str, zipfile->str);

	if (fileExists(*zipfile))
	{
		while (1)
		{
			String* prompt = make_empty_String();
			printf("\n\tThe zip file already exists!\n\tEnter Overwrite to replace existing file or Return to leave as it is: ");
            int i = 0;
            char temp;
            while (1)
            {
                temp = (char)getchar();
                if (temp != '\n')
                    prompt->str[i++] = temp;
                else
                {
                    prompt->str[i] = '\0';
                    break;
                }
            }

			if (strcmp(prompt->str, "Overwrite") == 0)
			{
				deleteFile(*path);
				String *command = make_empty_String();

				sprintf(command->str, "cp %s %s > /dev/null", zipfile->str, path->str);
				system(command->str);
				return 1;
			}
			else if (strcmp(prompt->str, "Return") == 0)
			{
				return 0;
			}
			else
			{
				printf("\n\tWrong Command, please enter again!\n\n");
			}
		}
	}
	else
	{
		String *command = make_empty_String();

		sprintf(command->str, "cp %s %s > /dev/null", zipfile->str, path->str);

		system(command->str);
		return 1;
	}
}