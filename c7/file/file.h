#ifndef FILE_H_
#define FILE_H_

// log_data == 0 - time (data + godzina), 1 - addr (adres IP), 2 - stat (kod statusu)
void read_data(char *fname, char **words, int **indexes, int log_data);

#endif