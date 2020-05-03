#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

//#define debug printf
#define debug
#define die(msg, err) \
  do                  \
  {                   \
    perror(msg);      \
    return err;       \
  } while (0)
#define master 0

int world_rank;
int world_size;

void bytes(int size, int *indexes, int *count, int *skip, int *out_count_bytes, int *out_skip_bytes)
{
  int i;
  for (i=0; i<size; i++)
  {
    if (i > 0)
      out_skip_bytes[i] = indexes[skip[i] - 1];
    out_count_bytes[i] = indexes[skip[i] + count[i] - 1];
  }

  for (i=--size; i>0; i--)
    out_count_bytes[i] -= out_count_bytes[i - 1];
}

void divide(int size, int nwords, int *out_count, int *out_skip)
{
  int i, j;
  int part = nwords / size;
  int reminder = nwords % size;

  for (i=0; i<size; i++)
  {
    out_count[i] = part;

    if (reminder-- > 0)
      out_count[i]++;

    out_skip[i] = 0;

    for (j=0; j<i; j++)
      out_skip[i] += out_count[j];
  }
}

void reduce(int nwords, char *words, int *indexes, int *list)
{
  int nbytes;
  int i, j;
  int *count = NULL;
  int *skip = NULL;
  int *count_bytes = NULL;
  int *skip_bytes = NULL;

  if (world_rank == master)
  {
    count = calloc(world_size, sizeof(int));
    skip = calloc(world_size, sizeof(int));
    count_bytes = calloc(world_size, sizeof(int));
    skip_bytes = calloc(world_size, sizeof(int));

    divide(world_size, nwords, count, skip);
    bytes(world_size, indexes, count, skip, count_bytes, skip_bytes);

    for (i=nwords-1, j=world_size-1; i>=0; i--)
    {
      if (i < skip[j])
        j--;
      indexes[i] -= indexes[skip[j] - 1];
    }
    for (i=0; i<world_size; i++)
      debug("R count[%d] = %d, count_bytes[%d] = %d, skip[%d] = %d, skip_bytes[%d] = %d\n", i, count[i], i, count_bytes[i], i, skip[i], i, skip_bytes[i]);
    for (i=0; i<nwords; i++)
      debug("R indexes[%d] = %d\n", i, indexes[i]);
  }

  MPI_Scatter(count, 1, MPI_INT, &nwords, 1, MPI_INT, master, MPI_COMM_WORLD);
  debug("[%d] Got %d words to reduce\n", world_rank, nwords);
  if (world_rank != master)
    indexes = calloc(nwords, sizeof(int));

  MPI_Scatter(count_bytes, 1, MPI_INT, &nbytes, 1, MPI_INT, master, MPI_COMM_WORLD);
  if (world_rank != master)
    list = calloc(nbytes, sizeof(int));
  debug("[%d] Got %d bytes of list to reduce\n", world_rank, nbytes);

  MPI_Scatterv(list, count_bytes, skip_bytes, MPI_INT, list, nbytes, MPI_INT, master, MPI_COMM_WORLD);
  MPI_Scatterv(indexes, count, skip, MPI_INT, indexes, nwords, MPI_INT, master, MPI_COMM_WORLD);
  for (i = 0; i < nwords; i++)
    debug("[%d] Index %d\n", world_rank, indexes[i]);

  for (i=0, j=0; i<nwords; i++)
  {
    int sum = 0;
    for (; j<indexes[i]; j++)
      sum += list[j];
    indexes[i] = sum;
    debug("[%d] SUM %d = %d\n", world_rank, i, indexes[i]);
  }
  MPI_Gatherv(indexes, nwords, MPI_INT, indexes, count, skip, MPI_INT, master, MPI_COMM_WORLD);
}

void map(int nwords, char *words, int *indexes, int *out_nwords, char *out_words, int *out_indexes, int *out_occurs)
{
  int all_words = 0;
  int *count = NULL;
  int *count_bytes = NULL;
  int *skip = NULL;
  int *skip_bytes = NULL;
  char *w = NULL;
  int i, j, len;
  int nbytes;

  indexes++;
  if (world_rank == master)
  {
    all_words = nwords;

    count = (int *)calloc(world_size, sizeof(int)); // ile elementów dla jakiego procesu
    count_bytes = (int *)calloc(world_size, sizeof(int)); // ile bajtów mają kolejne procesy z wektora "words"
    skip = (int *)calloc(world_size, sizeof(int)); // od którego elementu dany proces zaczyna (idąc od 0)
    skip_bytes = (int *)calloc(world_size, sizeof(int)); // od których przerw w wektorze "words" dany proces się zaczyna (poza 0, proces 0 zaczyna od początku)

    divide(world_size, nwords, count, skip);
    bytes(world_size, indexes, count, skip, count_bytes, skip_bytes);

    for (i=0; i<world_size; i++)
      debug("M count[%d] = %d, count_bytes[%d] = %d, skip[%d] = %d, skip_bytes[%d] = %d\n", i, count[i], i, count_bytes[i], i, skip[i], i, skip_bytes[i]);
    for (i=0; i<nwords; i++)
      debug("M indexes[%d] = %d\n", i, indexes[i]);
  }

  MPI_Scatter(count, 1, MPI_INT, &nwords, 1, MPI_INT, master, MPI_COMM_WORLD);
  debug("[%d] Got %d words to map\n", world_rank, nwords);

  MPI_Scatter(count_bytes, 1, MPI_INT, &nbytes, 1, MPI_INT, master, MPI_COMM_WORLD);
  if (world_rank != master)
    words = calloc(nbytes, sizeof(char));

  MPI_Scatterv(words, count_bytes, skip_bytes, MPI_CHAR, words, nbytes, MPI_CHAR, master, MPI_COMM_WORLD);

  {
    int i, j;
    int len = 0;
    int *occurs = NULL;

    MPI_Gatherv(words, nbytes, MPI_CHAR, words, count_bytes, skip_bytes, MPI_CHAR, master, MPI_COMM_WORLD);
    occurs = calloc((world_rank == master) ? all_words : nwords, sizeof(int));
    for (i=0; i<nwords; i++)
      occurs[i] = 1;

    MPI_Gatherv(occurs, nwords, MPI_INT, occurs, count, skip, MPI_INT, master, MPI_COMM_WORLD);

    if (world_rank == master)
    {
      int p, q;
      char *w = words;
      char *nw = out_words;

      indexes--;
      for (i=0, p=0, q=0; i<all_words; i++, p++)
      {
        char *a = words + indexes[i];
        if (*a == '\0')
        {
          p--;
          continue;
        }
        out_indexes[p] = p > 0 ? out_indexes[p - 1] : 0;

        for (j=i; j<all_words; j++)
        {
          char *b = words + indexes[j];
          if (strcmp(a, b) == 0)
          {
            out_indexes[p]++;
            out_occurs[q++] = occurs[j];
            if (j == i)
            {
              strcpy(nw, a);
              nw += strlen(a) + 1;
            }
            else
              *b = '\0';
          }
        }
      }
      *out_nwords = p;
    }
  }
}

int main(int argc, char *argv[])
{
  int i, j;
  int nwords = 0;
  int out_nwords = 0;
  char *words = NULL, *out_words = NULL;
  int *indexes = NULL;
  int *out_indexes = NULL;
  int *out_occurs = NULL;
  int buffer_size = 1024;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  if (world_rank == master)
  {
    int bytes;
    char c;
    int len;
    char *w;
    FILE *f = fopen("book", "r");
    words = calloc(buffer_size, sizeof(char));  /* "The last time I was"... */
    indexes = calloc(buffer_size, sizeof(int)); /* "3 8 13 15... */

    if (!f)
      die("Nie udalo sie otworzyc pliku", 1);

    /* pozbieraj slowa */
    for (bytes=0; (c = getc(f)) != EOF; bytes++)
    {
      if (!isalpha(c))
      {
        if (!words[bytes - 1])
          bytes--;
        continue;
      }
      if (bytes > buffer_size)
      {
        buffer_size *= 2;
        printf("Powiekszam bufor do %d bajtow.\n", buffer_size);
        words = realloc(words, buffer_size * sizeof(char));
        indexes = realloc(indexes, buffer_size * sizeof(int));
      }
      if (c == '\0')
        printf("Spacja!!!!!\n");
      words[bytes] = c;
    }

    // for (i = 0; i < buffer_size; i++)
    //   if (words[i] == '\0')
    //     printf("hhmm\n");
    //   else
    //     printf("%c", words[i]);
    // printf("\n");
    // printf("%d\n", strlen(words));

    /* policz slowa */
    w = words;
    len = strlen(w);
    while (len)
    {
      len = strlen(w);
      if (len == 0)
        break;
      indexes[++nwords] = w + len - words + 1;
      debug("%s on position %d\n", w, indexes[nwords]);
      w += len + 1;
    }

    debug("Wczytano %d bajtow (%d slow)\n", bytes, nwords);
    out_words = calloc(bytes, sizeof(char));
    out_indexes = calloc(nwords, sizeof(int));
    out_occurs = calloc(nwords, sizeof(int));
  }

  if (world_rank == master)
    printf("Faza mapowania %d slow na %d procesorach\n", nwords, world_size);

  map(nwords, words, indexes, &out_nwords, out_words, out_indexes, out_occurs);

  /* wyswietl wynik mapowania */
  if (world_rank == master)
  {
    char *w = out_words;
    for (i=0; i<out_nwords; i++, w += strlen(w) + 1)
    {
      int a = i > 0 ? out_indexes[i - 1] : 0;
      int b = out_indexes[i];
      printf("Mapped %s => [", w, a, b);
      for (j=a; j<b; j++)
        printf("%d ", out_occurs[j]);
      printf("]\n");
    }
  }

  if (world_rank == master)
    printf("Faza redukcji %d kluczy na %d procesorach\n", out_nwords, world_size);

  reduce(out_nwords, out_words, out_indexes, out_occurs);

  if (world_rank == master)
  {
    char *w = out_words;
    for (i=0; i<out_nwords; i++, w += strlen(w) + 1)
    {
      printf("Reduced %s => %d\n", w, out_indexes[i]);
    }
  }

  if (world_rank == master)
  {
    free(words);
    free(indexes);
    free(out_words);
    free(out_indexes);
    free(out_occurs);
  }

  MPI_Finalize();
  return 0;
}
