/* FUSE File System with JSON Files */

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


void removeChar(char *s, char c){	// Removing specific char (space)...
    int j, n = strlen(s);			// ...from the lines of the json file.
    for (int i=j=0; i<n; i++)
       if (s[i] != c)
          s[j++] = s[i];
    s[j] = '\0';
}

char full_path[100][100][200];		// Full path of each path name.
char root_path[100][100][200];		// Each single path name as elements.
char value[100][200];				// File content of each last element.
int indarr[100]={0};	// How much single path name each line will have.
int ind=0;				// How much lines will be extracted from the json file.
int key;				// Prevents filler function from re-adding the same folders.
	

static int hello_getattr(const char *path, struct stat *stbuf)
{
    printf("%s: %s\n", __FUNCTION__, path);
    memset(stbuf, 0, sizeof(struct stat));

    int retval = -ENOENT;
    
		for(int x=0; x<ind; x++){							// Rows of 2D full_path string array.
			for(int y=0; y<indarr[x]; y++){					// Columns of 2D full_path string array.
				if(strcmp(path, full_path[x][y]) == 0){		// If the path is one of the full paths;
					if(y < indarr[x]-1){					// If the path does not belong to a file;
						stbuf->st_mode = S_IFDIR | 0755;
						stbuf->st_nlink = 2;
						retval = 0;
					}
					else{				// If it is a file path (last element of the full_path array);
						stbuf->st_mode = S_IFREG | 0444;
						stbuf->st_nlink = 1;
						stbuf->st_size = strlen(value[x]);				// Value of the file element.
						retval = 0;
					}
				}
			}
		}
		
    return retval;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
         off_t offset, struct fuse_file_info *fi)
{
    printf("%s: %s\n", __FUNCTION__, path);
    
    (void) offset;
    (void) fi;
	
	int retval = -ENOENT;
	char *ptr;	// For removing the '/' character from the "/folder" name in filler() function.
	
	filler(buf, ".", NULL, 0);		// Each folder must have.
	filler(buf, "..", NULL, 0);		// Each folder must have.
	
	
	for(int x=0; x<ind; x++){				// For rows of full_path[][].
		for(int y=0; y<indarr[x]-1; y++){	// For columns of full_path[][] (except the last one).
			if(strcmp(path, full_path[x][y]) == 0){			// If the path is a full_path element;
				key=1;						// The key for deciding if a folder-file already exists. 
				for(int z=0; z<x; z++){						// For previous rows of the full_path[][];
					for(int w=0; w<=y; w++){				// For the columns of the previous rows;
						if(strcmp(full_path[x][y+1], full_path[z][w+1])==0)	// If it was already filled;
							key=0;							// Do not fill again. 
					}
				}
					if(key){					// Without re-adding a file-folder name in a directory;
						ptr = root_path[x][y+1];	
						ptr++;					// ptr changes from "/folder" to "folder"
						filler(buf, ptr, NULL, 0);
						retval = 0;
					}	
			}
		}
	}

    return retval;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    printf("%s: %s\n", __FUNCTION__, path);
    
    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    for(int x=0; x<ind; x++){			// If the path is equal to any last element of the full_path;
		if (strcmp(path, full_path[x][indarr[x]-1]) == 0){					// They are file paths.
			return 0;
		}
	}

    return -ENOENT;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
          struct fuse_file_info *fi)
{
    printf("%s: %s\n", __FUNCTION__, path);
    size_t len;
    (void) fi;
    
    for(int x=0; x<ind; x++){								// For each row of the full_path;
		if(strcmp(path, full_path[x][indarr[x]-1]) == 0){	// If the path is equal to the last element;
			len = strlen(value[x]);
			if (offset < len) {
				if (offset + size > len)
					size = len - offset;
				memcpy(buf, value[x] + offset, size);		// Get the value of the files.
			}
			else
				size = 0;
				
			return size;
		}
	}
    
	return -ENOENT;
}

static struct fuse_operations hello_oper = {
    .getattr    = hello_getattr,
    .readdir    = hello_readdir,
    .open       = hello_open,
    .read       = hello_read,
};

int main(int argc, char *argv[])
{
    FILE *fp;
    char buff[100][200] = {{'\0'},{'\0'}};
    fp = fopen("data.json", "r");
    
    int i=0, j=0;
    char temp[100][200];
    char copybuff[200];
    char *ret;
    char hey[200];
    
    
    while(fgets(buff[i], sizeof(buff[i]), fp)){
        buff[i][strlen(buff[i])-1]='\0';
        removeChar(buff[i], ' ');		// Remove spaces from each line of the json file.
        //printf("%s\n", buff[i]);

        if(buff[i][0] == '\"' && buff[i][strlen(buff[i])-1] == '{'){		// If it is a new element.
            strcpy(copybuff, buff[i]);
            ret = strstr(copybuff, "\"");		// Getting the names from " " characters.
            ret++;
            ret[strlen(ret)-strlen(strstr(ret, "\""))] = '\0';
            strcpy(hey, "");
            strcat(hey, "/");
            strcat(hey, ret);
            strcpy(temp[j], hey);				// temp[j] becomes "/element".
            j++;
            
        }
        else if(buff[i][0] == '\"' && buff[i][strlen(buff[i])-1] != '{'){	// For the last values;
            strcpy(copybuff, buff[i]);
            ret = strstr(copybuff, "\"");
            ret++;
            ret[strlen(ret)-strlen(strstr(ret, "\""))] = '\0';
            strcpy(hey, "");
            strcat(hey, "/");
            strcat(hey, ret);
            strcpy(temp[j], hey);						// temp[j] becomes the file names.
			
			strcpy(root_path[ind][0], "/");
			for(int y=0; y<=j; y++){
                strcpy(root_path[ind][y+1], temp[y]);	// Assign temp[j]s to root_path[][].
            }
            
			j++;
            strcpy(copybuff, buff[i]);
            ret = strstr(copybuff, ":");
            ret++; ret++;
            ret[strlen(ret)-strlen(strstr(ret, "\""))] = '\0';
            strcpy(hey, "");
            //strcat(hey, ":");
            strcat(hey, ret);
            strcpy(temp[j], hey);					// Getting the content values of the file elements.
			
			
            strcpy(value[ind], temp[j]);			// Assigning file contents to the value[] array.
            
            j--;	// Index must go back from file content order to previous one for new file elements.
            ind++;	// Index of the next row.
        }
        else if(buff[i][0] == '}'){
            j--;	// Go back when '}' is detected.
        }
        else;
    i++;
    }


	for(int i=0; i<ind; i++){
		for(int j=0; j<ind; j++){
			if(root_path[i][j][0]=='/')
				indarr[i]++;						// Determining index array lengths
			
		}
	}printf("\n");


	for(int a=0; a<ind; a++){
		strcpy(full_path[a][0], root_path[a][0]);
		for(int b=1; b<indarr[a]; b++){
			for(int c=1; c<=b; c++){
				strcat(full_path[a][b], root_path[a][c]);	// Constructing full_path from root_path elements
			}
		}
	}
	
	for(int a=0; a<ind; a++){				// The full_path elements can be seen if this block is activated
		printf("%s", full_path[a][indarr[a]-1]);		// Getting the final full paths with [][indarr[a]-1]
		printf(":(%s)\n", value[a]);
		if(a < ind-1 && strcmp(full_path[a][1], full_path[a+1][1]) != 0){
			printf("\n");
		}
	}printf("\n");
	
	
	// Starting the fuse program...
    setbuf(stdout, NULL);
    return fuse_main(argc, argv, &hello_oper, NULL);
    
}

