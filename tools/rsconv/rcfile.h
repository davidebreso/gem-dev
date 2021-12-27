FILE *open_file  (BYTE *filename);
FILE *create_file(BYTE *filename);
VOID close_files();
WORD dos_error  (WORD tos_err);
WORD get_file(   BYTE *extnt, BYTE *got_file);
VOID parse_fname(BYTE *full, BYTE *path, BYTE *name, BYTE *extnt);
VOID get_path(   BYTE *tmp_path, BYTE *spec);
VOID new_ext(    BYTE *o_fname,  BYTE *n_fname, BYTE *ext);

