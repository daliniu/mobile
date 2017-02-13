/* main.c */

/*
    This program demonstrates a simple application of JSON_parser. It reads
    a JSON text from STDIN, producing an error message if the text is rejected.

        % JSON_parser <test/pass1.json
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <locale.h>
#include <map>
#include <vector>

#include "JSON_parser.h"
#include "RequestDecoder.h"

using namespace std;

#if 0
int main(int argc, char* argv[]) {
    int count = 0, result = 0;
    //FILE* input;

    JSON_config config;

    struct JSON_parser_struct* jc = NULL;

    init_JSON_config(&config);

    config.depth                  = 19;
    config.callback               = &print;
    config.allow_comments         = 1;
    config.handle_floats_manually = 0;

    /* Important! Set locale before parser is created.
    if (argc >= 2) {
        if (!setlocale(LC_ALL, argv[1])) {
            fprintf(stderr, "Failed to set locale to '%s'\n", argv[1]);
        }
    } else {
        fprintf(stderr, "No locale provided, C locale is used\n");
    }
    */
    jc = new_JSON_parser(&config);

    //input = stdin;
    char input[2048] = "{\"indexs\":[\"succ_count\",\"timeout_count\",\"total_time\"], " \
		"\"filter\":[\"slave_name like '%%'\",\"f_tflag >='0030'\",\"f_tflag <='2350'\",\"f_date <= '20110120'\",\"f_date >= '20110119'\"]," \
		"\"groupby\":[\"f_date\",\"f_tflag\",\"slave_name\",\"master_name\",\"interface_name\"]" \
		"}" ;
    for (; input[count ] ; ++count) {
        //int next_char = fgetc(input);
        int next_char = input[count ];
		//printf("input %c\n",input[count] );
        if (next_char <= 0) {
            break;
        }
        if (!JSON_parser_char(jc, next_char)) {
            fprintf(stderr, "JSON_parser_char: syntax error, byte %d\n", count);
            result = 1;
            goto done;
        }
    }
    if (!JSON_parser_done(jc)) {
        fprintf(stderr, "JSON_parser_end: syntax error\n");
        result = 1;
        goto done;
    }

done:
    delete_JSON_parser(jc);
    return result;
}
#endif

int decodejson( const char * inxxx, map<string, vector<string> > * req)
{
    int count = 0, result = 0;
    //FILE* input;

    JSON_config config;

    struct JSON_parser_struct* jc = NULL;

    init_JSON_config(&config);

    config.depth                  = 19;
    config.callback               = &RequestDecoder::printd;
    config.allow_comments         = 1;
    config.handle_floats_manually = 0;
    config.callback_ctx = (void *)req;
    jc = new_JSON_parser(&config);

	const char *input = inxxx;
	/*
    char input[2048] = "{\"indexs\":[\"succ_count\",\"timeout_count\",\"total_time\"], " \
		"\"filter\":[\"slave_name like '%%'\",\"f_tflag >='0030'\",\"f_tflag <='2350'\",\"f_date <= '20110120'\",\"f_date >= '20110119'\"]," \
		"\"groupby\":[\"f_date\",\"f_tflag\",\"slave_name\",\"master_name\",\"interface_name\"]" \
		"}" ;
	*/
    for (; input[count] ; ++count) {
        int next_char = input[count];
        if (next_char <= 0) {
            break;
        }
        if (!JSON_parser_char(jc, next_char)) {
            fprintf(stderr, "JSON_parser_char: syntax error, byte %d\n", count);
            result = 1;
            goto done;
        }
    }
    if (!JSON_parser_done(jc)) {
        fprintf(stderr, "JSON_parser_end: syntax error\n");
        result = 1;
        goto done;
    }

done:
    delete_JSON_parser(jc);
    return result;
}


