#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

bool is_type(char *tipo, char *file)
{
  if(tipo == NULL) return true;
  else
  {
    if(strcmp(tipo, "f") == 0)
    {
      struct stat path_stat;
      stat(file, &path_stat);
      if(S_ISREG(path_stat.st_mode) != 0) return true;
      else return false;
    }
    else if(strcmp(tipo, "d") == 0)
    {
      struct stat path_stat;
      stat(file, &path_stat);
      if(S_ISDIR(path_stat.st_mode) != 0) return true;
      else return false;  
    }
  }
}

bool is_gtsize(int size, char *file)
{
  if(size != -1)
  {
    struct stat path_stat;
    stat(file, &path_stat);
    if(S_ISDIR(path_stat.st_mode) != 0) return true;
    else
    {
      return path_stat.st_size >= size;
    }
  }
  return true;
}

bool is_attrib(int attr, char *file)
{
  struct stat path_stat;
  stat(file, &path_stat);
  if(attr == -1)
  {
    return true;
  }
  if(attr == 0)
  {
    return (path_stat.st_mode & S_ISUID);
  }
  else if(attr == 1)
  {
    return (path_stat.st_mode & S_ISGID);
  }
  else if(attr == 2)
  {
    return (path_stat.st_mode & S_ISVTX);
  }
  else if(attr == 3)
  {
    return (path_stat.st_mode & S_ISUID) && (path_stat.st_mode & S_ISGID) && (path_stat.st_mode & S_ISVTX);
  }
  else return false;
}

void list_dir(char * dir_name, char **actions, int n_azioni, char **files, char **tests, int n_test, int depth)
{

  DIR * d;
  //Opens the directory specified by "dir_name"
  d = opendir(dir_name);
  //Checks it was opened 
  if(! d)
  {
    int k = 0;	
    while((strcmp(files[k], dir_name) != 0) && (strcmp(actions[n_azioni-1], "-delete") == 0))
    {
      //Checks if the directory "dir_name" was cancelled due to on going delete 
      if(strstr(dir_name, files[k]) != NULL)
      {
	return;
      }
      k++;			
    }
    fprintf(stderr, "Cannot open directory %s because of %s\n", dir_name, strerror(errno));
    exit(10);
  }
  //Gets test variables
  char *type = NULL; 
  int gtsize = -1, attrib = -1;
  for(int j=n_test-1; j >= 0; j--)
  {
    if(strcmp(tests[j], "-type") == 0)
    { 
      if(type == NULL)
      {
        type = tests[j + 1];
      }
    }
    else if(strcmp(tests[j], "-gtsize") == 0 )
    {
      if(gtsize == -1)
      {
        gtsize = atoi(tests[j + 1]);
      }
    } 
    else if(strcmp(tests[j], "-attrib") == 0 )
    {
      if(attrib == -1)
      {
        attrib = atoi(tests[j + 1]);
      }
    }
  }
  //Do "while" until there is no entry in the directory
  while (1) 
  {
    struct dirent * entry;
    const char * d_name;
    //"Readdir" gets subsequent entries from "d".
    entry = readdir(d);
    if(! entry) 
    {
      /* There are no more entries in this directory, so break
	 out of the while loop. */
      break;
    }
    d_name = entry->d_name;
    //If the action is "delete"
    if(strcmp(actions[n_azioni-1], "-delete") == 0)
    {
      if(entry->d_type & DT_DIR) 
      {
        /* Checks that the directory is not "d" or d's parent. */  
        if(strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0) 
        {
	  int path_length;
	  char path[PATH_MAX];
	  sprintf(path, "%s/%s", dir_name, d_name);
          //Checks the depth
          if(depth > 1)
 	  {
	    list_dir(path, actions, n_azioni, files, tests, n_test, depth - 1);
	  }
 	  else if(depth == NULL)
          {
	    list_dir(path, actions, n_azioni, files, tests, n_test, depth);
	  }
        }
      }
      else
      {
        char elemento[PATH_MAX];
        sprintf(elemento, "%s/%s", dir_name, d_name);
        if( is_type(type, elemento) && is_gtsize(gtsize, elemento) && is_attrib(attrib, elemento) )
 	{
          if(unlink(elemento) != 0)
          {
            fprintf(stderr, "Cannot delete %s because of %s\n", elemento, strerror(errno));
          }	
	}
      }
    }
    //If the action is "print"
    else
    {
      if(strcmp(d_name, "..") != 0)
      {
	if(strcmp(d_name, ".") == 0)
  	{
	  if( is_type(type, dir_name) && is_gtsize(gtsize, dir_name) && is_attrib(attrib, dir_name) )
	  {
	    printf("%s\n", dir_name);
	  }
	}
        else
        {
	  if(entry->d_type & DT_DIR) 
          {
	     char new_path[PATH_MAX];
             sprintf(new_path, "%s/%s", dir_name, d_name);
	     if(depth > 1)
	     {
               list_dir(new_path, actions, n_azioni, files, tests, n_test, depth - 1);
	     }
	     else if(depth == NULL)
	     {
	       list_dir(new_path, actions, n_azioni, files, tests, n_test, depth);
	     }
	     else
	     {
 	       if( is_type(type, dir_name) && is_gtsize(gtsize, dir_name) && is_attrib(attrib, dir_name) )
	       {
	         printf("%s/%s\n", dir_name, d_name);
	       }
	     } 
          }
	  else
	  { 
            char el[PATH_MAX];
            sprintf(el, "%s/%s", dir_name, d_name);
            if( is_type(type, el) && is_gtsize(gtsize, el) && is_attrib(attrib, el) )
	    {
	      printf("%s/%s\n", dir_name, d_name);
	    }
	  }
	}
      }
    } 
  }
  //If the action is "delete" checks if the directory "dir_name" can be cancelled
  if(strcmp(actions[n_azioni-1], "-delete") == 0)
  {
    if( is_type(type, dir_name) && is_gtsize(gtsize, dir_name) && is_attrib(attrib, dir_name) )
    {
      if(rmdir(dir_name) != 0)
      {
        fprintf(stderr, "Cannot delete %s because of %s\n", dir_name, strerror(errno));
      }
    }
  }
  /* After going through all the entries, closes the directory. */
  if(closedir(d) != 0)
  {
    fprintf(stderr, "System call closedir failed because of %s\n", strerror(errno));
    exit(100);
  }
}

int main(int argc, char *argv[])
{
  //This section gets all the command line options, counts and saves them in variables
  int num_file = 0, num_test = 0, num_azioni = 0;
  bool is_maxdepth = false;
  for(int i=1; i < argc; i++)
  {
    if(strcmp(argv[i], "-maxdepth") == 0)
    {
	is_maxdepth = true;
        i++;
    }
    else if( (strcmp(argv[i], "-type") == 0 ) || (strcmp(argv[i], "-gtsize") == 0 ) || (strcmp(argv[i], "-attrib") == 0 ) ) 
    {
    	num_test+=2;
        i++;
    }
    else if( (strcmp(argv[i], "-delete") == 0 ) || (strcmp(argv[i], "-print") == 0) )
    {
   	num_azioni++;
    }
    else if(argv[i][0] != '-') num_file++;
  }
  if(num_azioni == 0) num_azioni++;
  if(num_file == 0)
  {
      fprintf(stderr, "Usage: ./1/1 dirs [-maxdepth N] [tests] [actions]\n");
      exit(20);
  }
  //Creates arrays of pointers and stores values in them
  char **file, **test, **azioni;
  int maxdepth = NULL;
  file = (char **)calloc(num_file, sizeof(char*)); 
  test = (char **)calloc(num_test, sizeof(char*)); azioni = (char **)calloc(num_azioni, sizeof(char*));
  int j=0, k=0, l=0;
  for(int i=1; i < argc; i++)
  {
    if(strcmp(argv[i], "-maxdepth") == 0)
    {
  	i++;
	maxdepth = atoi(argv[i]);
    }
    else if( (strcmp(argv[i], "-type") == 0 ) || (strcmp(argv[i], "-gtsize") == 0 ) || (strcmp(argv[i], "-attrib") == 0 ) ) 
    {

	test[k] = (char*)calloc(strlen(argv[i]) +1, sizeof(char));
        strcpy(test[k], argv[i]);
        i++; k++;
	test[k] = (char*)calloc(strlen(argv[i]) +1, sizeof(char));
        strcpy(test[k], argv[i]);
        k++;
    }
    else if( (strcmp(argv[i], "-delete") == 0 ) || (strcmp(argv[i], "-print") == 0) )
    {
   	azioni[l] = (char*)calloc(strlen(argv[i]) +1, sizeof(char));
        strcpy(azioni[l], argv[i]);
        l++;
    }
    else if(argv[i][0] != '-')
    {
	file[j] = (char*)calloc(strlen(argv[i]) +1, sizeof(char));
        strcpy(file[j], argv[i]);
        j++;
    }
  }
  if((num_azioni == 1) && (azioni[0] == NULL))
  {
	azioni[0] = (char*)calloc(7, sizeof(char));
        strcpy(azioni[0], "-print");
  }
  //For every file retrieved from the command line
  for(int i=0; i < num_file; i++)
  {	
	struct stat path_stat;
        stat(file[i], &path_stat);
        if( S_ISREG(path_stat.st_mode) == true)
        {
 	  //Gets test variables
          char *type = NULL; 
          int gtsize = -1, attrib = -1;
          for(int j=num_test-1; j >= 0; j--)
          {
            if(strcmp(test[j], "-type") == 0)
            { 
              if(type == NULL)
              {
                type = test[j + 1];
              }
            }
            else if(strcmp(test[j], "-gtsize") == 0)
            {
              if(gtsize == -1)
              {
                gtsize = atoi(test[j + 1]);
              }
            } 
            else if(strcmp(test[j], "-attrib") == 0)
            {
              if(attrib == -1)
              {
                attrib = atoi(test[j + 1]);
              }
            }
          }
          if(strcmp(azioni[num_azioni-1], "-delete") == 0)
          {
  	    if( is_type(type, file[i]) && is_gtsize(gtsize, file[i]) && is_attrib(attrib, file[i]) )
    	    {
  	      if(unlink(file[i]) != 0)
              {
                fprintf(stderr, "Cannot delete %s because of %s\n", file[i], strerror(errno));
              } 
            }
	  }
	  else
          {
            if( is_type(type, file[i]) && is_gtsize(gtsize, file[i]) && is_attrib(attrib, file[i]) )
            {
	      printf("%s\n", file[i]);
            }
	  }
 	}
	else
	{
	  list_dir(file[i], azioni, num_azioni, file, test, num_test, maxdepth);
	}
  }
  //Frees all the memory allocated
  for(int i=0; i < num_file; i++)
  {
    free(file[i]);
  }
  for(int i=0; i < num_test; i++)
  {
    free(test[i]);
  }
  for(int i=0; i < num_azioni; i++)
  {
    free(azioni[i]);
  }
  free(file); free(test); free(azioni);
  return(0);
}
