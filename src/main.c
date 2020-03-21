#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "generator.h"
#include "linked_list.h"
#include "literal.h"

int process_generator_result(generator_output_t result);

int main(int argc, char** argv) {

	char *source_path, *outfile_path = 0;

	if (argc < 2) {
		puts("usage: svm-asm [-o outfile] filename");
		return 1;
	}

	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i],"-o"))
			outfile_path = argv[i+1];
	}
	source_path = argv[argc - 1];
	if (!outfile_path) outfile_path = "svm-asm.out";
	if (!strcmp(source_path, outfile_path)) {
		puts("svm-asm: error: file is both input and output file");
		return 2;
	}

	FILE *fp_source = fopen(source_path, "r");
	if (fp_source == 0) {
		puts("svm-asm: error: unable to open source file");
		return 3;
	}

	if (compile_regex()) {
		puts("svm-asm: error: could not compile regex");
		return 4;
	}

	read_file_output_t output = read_file(fp_source);
	fclose(fp_source);
	if (!output.list_tokens) return 5;

	FILE *fp_outfile = fopen(outfile_path, "wb");
	if (fp_outfile == 0) {
		puts("svm-asm: error: unable to open output file");
		delete_literal_list(output.list_literals);
		delete_list(output.list_tokens);
		return 6;
	}

	fseek(fp_outfile, HEADER, SEEK_SET);
	generator_output_t result = read_text(output.list_tokens->first_node, fp_outfile);
	if (process_generator_result(result)) {
		delete_literal_list(output.list_literals);
		delete_list(output.list_tokens);
		fclose(fp_outfile);
		return 7;
	}
	uint16_t text_len = ftell(fp_outfile) - HEADER;

	result = read_data(output.list_tokens->first_node, fp_outfile);
	if (process_generator_result(result)) {
		delete_literal_list(output.list_literals);
		delete_list(output.list_tokens);
		fclose(fp_outfile);
		return 8;
	}
	uint16_t data_len = ftell(fp_outfile) - HEADER;

	result = read_bss(output.list_tokens->first_node, ftell(fp_outfile));
	if (process_generator_result(result)) {
		delete_literal_list(output.list_literals);
		delete_list(output.list_tokens);
		fclose(fp_outfile);
		return 9;
	}
	result.bss_size += data_len;

	fseek(fp_outfile, 0, SEEK_SET);
	fwrite(&text_len, sizeof(uint16_t), 1, fp_outfile);
	fwrite(&data_len, sizeof(uint16_t), 1, fp_outfile);
	fwrite(&result.bss_size, sizeof(uint16_t), 1, fp_outfile);

	insert_literals_values(output.list_literals->first_node, fp_outfile);

	fclose(fp_outfile);

	delete_literal_list(output.list_literals);
	delete_list(output.list_tokens);

	return 0;
}

int process_generator_result(generator_output_t result) {
	int output;
	char *error_message;
	switch (result.return_code) {
		case 0: return 0;
		case -1:
			error_message = "instruction expected";
			output = 1;
			break;
		case -2:
			error_message = "invalid combination of opcode and operands";
			output = 2;
			break;
		default:
			error_message = "unknown error";
			output = 3;
			break;
	}
	printf("svm-asm: error: generator: (line %d) %s\n", result.line_num, error_message);
	return output;
}