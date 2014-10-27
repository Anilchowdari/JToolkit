#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cextdecs.h>
#include <tal.h>

#include <ipc.h>

int main(int argc, char* argv[]) {
	short fnum = 0;
	_cc_status cc = 0;

	short receive_depth = 1;

	cc = FILE_OPEN_("$RECEIVE",
			8,
			&fnum,
			0,/*access*/
			,/*Exclusion*/
			,/*nowait depth*/
			receive_depth);

	if (_status_ne(cc)) {
		printf("Error opening the $RECEIVE file %d\n", cc);
		return 0;
	}
	if (receive_depth != 0) {
		char message[32000] = "";
		unsigned short count_read = 0;
		while (1) {
			memset(message,NULL,sizeof(message));
			cc = READUPDATEX(fnum, message, (short) sizeof(message),
					&count_read);
			if (_status_ne(cc)) {
				short error;
				FILE_GETINFO_(fnum, &error);
				if (cc == 6) {
					printf("This is a system message %d\n", error);
					REPLYX();
					continue;
				} else {
					printf("An error has occured in READUPDATEX. %d\n", error);
					return 0;
				}
			}
			message[count_read] = '\0';
			{
				input_def *input = (input_def *) NULL;
				input = (input_def *) message;
				input->firstname[sizeof(input->firstname)-1]='\0';
				input->lastname[sizeof(input->lastname)-1]='\0';
				printf("Input message:%s,%s,%d,%d,%c\n", input->firstname,
						input->lastname, input->empid, input->age,
						input->operation);
			}

			{
				output_def *output = (output_def *) NULL;
				output = (output_def *) message;
				memset(message,NULL,sizeof(message));
				sprintf(output->message, "%s",
						"This is just a sample server. It does not do any DB operations");
				output->retcode = 0;

				count_read = sizeof(output_def);
			}
			cc = REPLYX(message, count_read);
			if (_status_lt(cc)) {
				short error;
				FILE_GETINFO_(fnum, &error);
				printf("REPLYX failed with error code %d\n", error);
			}
			if (strcmp(message, "END") == 0) {
				printf("Shutting down the server\n");
				return 0;
			}
		}
	} else {
		char message[32000] = "";
		unsigned short count_read = 0;
		memset(message,NULL,sizeof(message));
		while (1) {
			cc = READX(fnum, message, (short) sizeof(message), &count_read);
			if (_status_ne(cc)) {
				short error;
				FILE_GETINFO_(fnum, &error);
				if (cc == 6) {
					printf("This is a system message %d\n", error);
					REPLYX();
					continue;
				} else {
					printf("An error has occured in READUPDATEX. %d\n", error);
					return 0;
				}
			}
			message[count_read] = '\0';
			{
				input_def *input = (input_def *) NULL;
				input = (input_def *) message;
				printf("Input message:%s,%s,%d,%d,%c\n", input->firstname,
						input->lastname, input->empid, input->age,
						input->operation);
			}

			cc = REPLYX(message, count_read);
			if (_status_lt(cc)) {
				short error;
				FILE_GETINFO_(fnum, &error);
				printf("REPLYX failed with error code %d\n", error);
			}
			if (strcmp(message, "END") == 0) {
				printf("Shutting down the server\n");
				return 0;
			}
		}
	}
}

