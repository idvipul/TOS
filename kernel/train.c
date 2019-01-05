/*
Name: Vipul Karanjkar
SFSU ID: 917924532
*/

#include <kernel.h>

#define SHELL_WIDTH 60
#define SHELL_HEIGHT 15
#define TRAIN_ID "20"
#define CR '\015'

// **************************
// run the train application
// **************************

// send command to train
void send_command_to_train(int window_id, char* command, char* input_buffer, int input_length) {
  COM_Message message;

  message.output_buffer = command;
  message.input_buffer = input_buffer;
  message.len_input_buffer = input_length;

  sleep(4);
  send(com_port, &message);
}

// initializing switches
void init_train_switches(int window_id) {
  wm_print(window_id, "\nInitializing Switches\n");

  change_switch ('4', 'G', window_id);
  change_switch ('1', 'G', window_id);
  change_switch ('9', 'R', window_id);
  change_switch ('5', 'G', window_id);
  change_switch ('8', 'G', window_id);
}

// clears s88 memory buffer
void clear_memory_buffer(window_id) {
  char command[5];
  char *cmd = command;

  command[0] = 'R';
  command[1] = '\015';
  command[2] = '\0';

  wm_print(window_id, "\ns88 memory buffer cleaned. (R)\n");
  send_command_to_train(window_id, cmd, NULL, 0);
}

// probe a segment to detect if anything is present on the segment
char probe_segment(char* segment, int window_id) {
  clear_memory_buffer(window_id);

	char input_buffer[3];
	char command[10];
	char *cmd = command;
	int len = k_strlen (segment);

	command[0] = 'C';
	k_memcpy (&command[1], segment, len);
	command[++len] = '\015';
	command[++len] = '\0';

  wm_print(window_id, "Probbing segment: %s\n", segment);
	send_command_to_train(window_id, cmd, &input_buffer[0], 3);
	return input_buffer[1];
}

// probing a segment until some something is detected
void continue_probing (char* segment, int window_id) {
	while (42) {
    wm_print(window_id, "Probbing segment: %s\n", segment);
		if (probe_segment (segment, window_id) == '1') return;
	}
}

// change train's direction
void change_direction (int window_id) {
	char command[8];
	char *cmd = command;
	int len = k_strlen (TRAIN_ID);

	command[0] = 'L';
	k_memcpy (&command[1], TRAIN_ID, len);
	command[++len] = 'D';
	command[++len] = '\0';
	wm_print (window_id, "\nReversed direction of train (%s)\n", command);

	command[len] = CR;
	command[++len] = '\0';
	send_command_to_train(window_id, cmd, NULL, 0);
}

// change train's speed
void change_speed (char speed, int window_id) {
	char input_buffer;
	char command[8];
	char *cmd = command;
	int len = k_strlen (TRAIN_ID);

	command[0] = 'L';
	k_memcpy (&command[1], TRAIN_ID, len);
	command[++len] = 'S';
	command[++len] = speed;
	command[++len] = '\0';
	wm_print (window_id, "\nChanged train velocity to %c (%s)\n", speed, command);

	command[len] = CR;
	command[++len] = '\0';
	send_command_to_train(window_id, cmd, &input_buffer, 0);
}

// 	change switch
void change_switch (char switch_id, char color, int window_id) {
	char input_buffer;
	char command[8];
	char *cmd = command;

	command[0] = 'M';
	command[1] = switch_id;
	command[2] = color;
	command[3] = '\0';
	wm_print (window_id, "\nChanged switch %c to %c (%s)\n", switch_id, color, command);

	command[3] = CR;
	command[4] = '\0';
	send_command_to_train(window_id, cmd, &input_buffer, 0);
}

// search train and wagon's posotion
int search_train_and_wagon(int window_id, int zamboni) {
  wm_print(window_id, "\nSearching for Train and Wagon\n");
  int conf;

  if (probe_segment("8", window_id) == '1' && probe_segment("11", window_id) == '1') {
    if (zamboni) {
      return 1;
    }
    return 2;
  } else if (probe_segment("12", window_id) == '1' && probe_segment("2", window_id) == '1') {
    if (zamboni) {
      return 3;
    }
    return 4;
  } else if (probe_segment("2", window_id) == '1' && probe_segment("11", window_id) == '1') {
    if (zamboni) {
      return 5;
    }
    return 6;
  } else if (probe_segment("5", window_id) == '1' && probe_segment("12", window_id) == '1') {
    if (zamboni) {
      return 7;
    }
    return 8;
  } else {
		return -1;
	}
}

// search zamboni
BOOL find_zamboni (int window_id) {
	wm_print (window_id, "\nSearching Zamboni\n");

	for (int i = 0; i < 20; i++){
		if (probe_segment("7", window_id) == '1') {
			wm_print (window_id, "\nZamboni Found\n");
			return TRUE;
		}
	}
	wm_print (window_id, "\nZamboni not found\n");
	return FALSE;
}

// identifies configuration
int identify_configuration(int window_id) {
  wm_print(window_id, "\nIdentifying Configuration\n");

  BOOL zamboni;
  int conf;

	zamboni = find_zamboni(window_id);

  conf = search_train_and_wagon(window_id, zamboni);
  return conf;
}

// run conf 1 with zamboni
void conf_1z(int window_id) {
	wm_print(window_id, "\nConf 1 with zamboni");

  change_switch('6', 'R', window_id);
  change_switch('5', 'R', window_id);
  change_speed('5', window_id);
  continue_probing("10", window_id);
  change_switch('8', 'R', window_id);
  continue_probing("12", window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_switch('7', 'G', window_id);
  change_switch('6', 'G', window_id);
  change_switch('5', 'R', window_id);
  change_speed('5', window_id);
  continue_probing("14", window_id);
  change_switch('8', 'R', window_id);
  change_switch('7', 'R', window_id);
  change_switch('2', 'R', window_id);
  change_switch('1', 'R', window_id);
  continue_probing("6", window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_switch('5', 'R', window_id);
  change_switch('6', 'R', window_id);
  change_speed('4', window_id);
  continue_probing("8", window_id);
  change_speed('0', window_id);
}

// run conf 1 without zamboni
void conf_1(int window_id) {
	wm_print(window_id, "\nConf 1 without zamboni");

  change_switch('2', 'R', window_id);
  change_speed('5', window_id);
  continue_probing("7", window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_switch('5', 'R', window_id);
  change_switch('6', 'G', window_id);
  change_speed('5', window_id);
  continue_probing("12", window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_switch('7', 'R', window_id);
  change_speed('5', window_id);
  continue_probing("13", window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_speed('5', window_id);
  continue_probing("7", window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_switch('6', 'R', window_id);
  change_speed('4', window_id);
  continue_probing("8", window_id);
  change_speed('0', window_id);
}

// run conf 2 with zamboni
void conf_2z(int window_id) {
	wm_print(window_id, "\nConf 2 with zamboni");

  change_direction (window_id);
  change_switch('7', 'G', window_id);
  change_switch('6', 'R', window_id);
  change_switch('5', 'R', window_id);
  change_switch('4', 'R', window_id);
  change_switch('3', 'G', window_id);
  change_speed('5', window_id);
	continue_probing("1", window_id);
	change_speed('0', window_id);
  change_switch('5', 'G', window_id);
  continue_probing("3", window_id);
  change_speed('5', window_id);
  change_switch('1', 'R', window_id);
  change_switch('9', 'R', window_id);
  change_switch('8', 'R', window_id);
  change_switch('7', 'R', window_id);
  continue_probing("12", window_id);
  change_speed('0', window_id);
  change_switch('1', 'G', window_id);
}

// run conf 2 without zamboni
void conf_2(int window_id) {
	wm_print(window_id, "\nConf 2 without zamboni\n");

  change_speed('5', window_id);
	continue_probing ("14", window_id);
	change_speed('0', window_id);
  change_direction (window_id);
  change_switch('1', 'R', window_id);
  change_switch('2', 'G', window_id);
  change_switch('5', 'R', window_id);
  change_switch('6', 'G', window_id);
  change_switch('7', 'G', window_id);
  change_speed('5', window_id);
	continue_probing("12", window_id);
	change_speed('0', window_id);
}

// run conf 3 with zamboni
void conf_3z(int window_id) {
	wm_print(window_id, "\nConf 3 with zamboni\n");

  change_switch('3', 'G', window_id);
  change_switch('4', 'R', window_id);
  change_switch('5', 'R', window_id);
  change_switch('6', 'G', window_id);
  change_switch('7', 'G', window_id);
  change_speed('5', window_id);
  continue_probing("12", window_id);
  change_switch('2', 'R', window_id);
  change_switch('5', 'G', window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_switch('7', 'R', window_id);
  change_speed('5', window_id);
  continue_probing("11", window_id);
  change_speed('0', window_id);
  continue_probing("3", window_id);
  change_switch('8', 'R', window_id);
  change_switch('9', 'R', window_id);
  change_switch('1', 'R', window_id);
  change_switch('2', 'G', window_id);
  change_speed('5', window_id);
  continue_probing("2", window_id);
  change_speed('0', window_id);
  change_switch('1', 'G', window_id);
}

// run conf 3 without zamboni
void conf_3(int window_id) {
	wm_print(window_id, "\nConf 3 without zamboni\n");

  change_switch('2', 'R', window_id);
  change_switch('5', 'R', window_id);
  change_switch('6', 'G', window_id);
  change_speed('4', window_id);
  continue_probing("12", window_id);
  change_speed('0', window_id);
  change_direction (window_id);
  change_switch('7', 'R', window_id);
  change_switch('1', 'R', window_id);
  change_switch('2', 'G', window_id);
  change_speed('5', window_id);
  continue_probing("2", window_id);
  change_speed('0', window_id);
}

// run conf 4 with zamboni
void conf_4z(int window_id) {
	wm_print(window_id, "\nConf 4 with zamboni\n");

  change_switch('3', 'R', window_id);
  change_switch('4', 'R', window_id);
  change_switch('5', 'R', window_id);
  change_switch('6', 'G', window_id);
  change_switch('7', 'G', window_id);
  change_speed('5', window_id);
  continue_probing("10", window_id);
  change_switch('4', 'G', window_id);
  continue_probing("4", window_id);
  change_switch('4', 'R', window_id);
  continue_probing("5", window_id);
  change_speed('0', window_id);
  change_switch('4', 'G', window_id);
}

// run conf 4 without zamboni
void conf_4(int window_id) {
	wm_print(window_id, "\nConf 4 without zamboni\n");

  change_switch('4', 'R', window_id);
  change_switch('3', 'R', window_id);
  change_switch('5', 'R', window_id);
  change_switch('6', 'G', window_id);
  change_speed('5', window_id);
  continue_probing("14", window_id);
  change_speed('0', window_id);
  change_speed('5', window_id);
  continue_probing("5", window_id);
  change_speed('0', window_id);
}

void train_process(PROCESS self, PARAM param) {
  int window_id = wm_create(10, 5, SHELL_WIDTH, SHELL_HEIGHT);
  wm_print(window_id, "\nTrain Process\n");

  // initailize train switches i.e. to move zamboni in a outer loop
  init_train_switches(window_id);

  // check configuration
  int conf = identify_configuration(window_id);

  switch(conf) {
    case 1: conf_1z(window_id);
    break;
    case 2: conf_1(window_id);
    break;
    case 3: conf_2z(window_id);
    break;
    case 4: conf_2(window_id);
    break;
    case 5: conf_3z(window_id);
    break;
    case 6: conf_3(window_id);
    break;
    case 7: conf_4z(window_id);
    break;
    case 8: conf_4(window_id);
    break;
    default: wm_print(window_id, "\nConf not found\n");
  }
  // to avoid fatal exception 6
  become_zombie();
}

void init_train(int window_id) {
  create_process(train_process, 5, 0, "Train Process");
  resign();
}
