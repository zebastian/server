/* Make a db.h that will be link-time compatible with Sleepycat's Berkeley DB. */

#include "../../mysql-5.0.27/mysql-5.0.27/bdb/build_win32/db.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define DECL_LIMIT 100
#define FIELD_LIMIT 100
struct fieldinfo {
    char decl[DECL_LIMIT];
    unsigned int off;
    unsigned int size;
} fields[FIELD_LIMIT];
int field_counter=0;


int compare_fields (const void *av, const void *bv) {
    const struct fieldinfo *a = av;
    const struct fieldinfo *b = bv;
    if (a->off < b->off) return -1;
    if (a->off > b->off) return 1;
    return 0;
}				      

#define STRUCT_SETUP(typ, name, fstring) ({ snprintf(fields[field_counter].decl, DECL_LIMIT, fstring, #name); \
	    fields[field_counter].off = __builtin_offsetof(typ, name);       \
            { typ dummy;                                           \
		fields[field_counter].size = sizeof(dummy.name); } \
	    field_counter++; })

FILE *outf;
void open_file (void) {
    char fname[100];
    snprintf(fname, 100, "sample_offsets_%d.h", __WORDSIZE);
    outf = fopen(fname, "w");
    assert(outf);

}

void sort_and_dump_fields (const char *structname) {
    int i;
    qsort(fields, field_counter, sizeof(fields[0]), compare_fields);
    fprintf(outf, "struct fieldinfo %s_fields%d[] = {\n", structname, __WORDSIZE);
    for (i=0; i<field_counter; i++) {
	fprintf(outf, "  {\"%s\", %d, %d}", fields[i].decl, fields[i].off, fields[i].size);
	if (i+1<field_counter) fprintf(outf, ",");
	fprintf(outf, "\n");
    }
    fprintf(outf, "};\n");
}

void sample_db_offsets (void) {
    /* Do these in alphabetical order. */
    field_counter=0;
    STRUCT_SETUP(DB,app_private,    "void *%s");
    STRUCT_SETUP(DB,close,          "int (*%s) (DB*, u_int32_t)");
    STRUCT_SETUP(DB,cursor,         "int (*%s) (DB *, DB_TXN *, DBC **, u_int32_t)");
    STRUCT_SETUP(DB,del,            "int (*%s) (DB *, DB_TXN *, DBT *, u_int32_t)");
    STRUCT_SETUP(DB,get,            "int (*%s) (DB *, DB_TXN *, DBT *, DBT *, u_int32_t)");
    STRUCT_SETUP(DB,key_range,      "int (*%s) (DB *, DB_TXN *, DBT *, DB_KEY_RANGE *, u_int32_t)");
    STRUCT_SETUP(DB,open,           "int (*%s) (DB *, DB_TXN *, const char *, const char *, DBTYPE, u_int32_t, int)");
    STRUCT_SETUP(DB,put,            "int (*%s) (DB *, DB_TXN *, DBT *, DBT *, u_int32_t)");
    STRUCT_SETUP(DB,remove,         "int (*%s) (DB *, const char *, const char *, u_int32_t)");
    STRUCT_SETUP(DB,rename,         "int (*%s) (DB *, const char *, const char *, const char *, u_int32_t)");
    STRUCT_SETUP(DB,set_bt_compare, "int (*%s) (DB *, int (*)(DB *, const DBT *, const DBT *))");
    STRUCT_SETUP(DB,set_flags,      "int (*%s) (DB *, u_int32_t)");
    STRUCT_SETUP(DB,stat,           "int (*%s) (DB *, void *, u_int32_t)");
    sort_and_dump_fields("db");
}
void sample_dbt_offsets (void) {
    field_counter=0;
    STRUCT_SETUP(DBT,app_private, "void*%s");
    STRUCT_SETUP(DBT,data,        "void*%s");
    STRUCT_SETUP(DBT,flags,       "u_int32_t %s");
    STRUCT_SETUP(DBT,size,        "u_int32_t %s");
    STRUCT_SETUP(DBT,ulen,        "u_int32_t %s");
    sort_and_dump_fields("dbt");
}
int main (int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) {
    open_file();
    fprintf(outf, "/* BDB offsets on a %d-bit machine */\n", __WORDSIZE);
    sample_db_offsets();
    sample_dbt_offsets();
    return 0;
}
