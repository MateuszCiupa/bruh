#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 1024

void read_data(char *fname, char **words, int **indexes, int log_data)
{
  FILE *f;
  char c;
  int bytes;
  int buffer_size = 1024;
  int index_size = 1024;
  int word_no = 0, colon_no = 0, quotation_no = 0, space_no = 0;
  int is_square_bracket = 0;
  
  *words = calloc(buffer_size, sizeof(char));
  *indexes = calloc(index_size, sizeof(int));
  
  if (!(f = fopen(fname, "r")))
  {
    perror("Problem while loading file\n");
    exit(EXIT_FAILURE);
  }

  for (bytes = 0; (c = getc(f)) != EOF; bytes++)
  {
    switch (log_data)
    {
      case 0: // time
        if (!is_square_bracket)
        {
          while (getc(f) != '[');
          is_square_bracket++;
          bytes--;
          continue;
        }
        
        if (c == ':') colon_no++;
        
        if (colon_no == 3)
        {
          word_no++;
          while (getc(f) != '\n');
          is_square_bracket = 0;
          colon_no = 0;
          continue;
        }
        break;

      case 1: // addr
        if (c == ' ')
        {
          word_no++;
          while (getc(f) != '\n');
          continue;
        }
        break;

      case 2: // status
        if (c == ' ') space_no++;
        if (space_no == 6)
        {
          word_no++;
          space_no = 0;
          quotation_no = 0;
          while (getc(f) != '\n');
          continue;
        }
        if (space_no < 5)
        {
          bytes--;
          continue;
        } 
        else
        {
          if (!quotation_no)
          {
            getc(f);
            quotation_no++;
            bytes--;
            continue;
          }
        }
        
        break;

      default:
        perror("Wrong log_data value, it should be 0, 1 or 2\n");
        exit(EXIT_FAILURE);
        break;
    }
    
    if (bytes > buffer_size)
    {
      buffer_size += BUFF_SIZE;
      *words = realloc(*words, buffer_size * sizeof(char));
    }

    if (word_no > index_size)
    {
      index_size += BUFF_SIZE;
      *indexes = realloc(*indexes, index_size * sizeof(int));
    }
    
    (*words)[bytes] = c;
  }

  fclose(f);
}