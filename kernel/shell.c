/*
Name: Vipul Karanjkar
SFSU ID: 917924532
*/
#include <kernel.h>

#define TOTAL_SHELL_COMMANDS 7
#define SHELL_SYMBOL ">>"
#define SHELL_WIDTH 70
#define SHELL_HEIGHT 20

// list of supported shell commands
static char* shell_comands[] = {"help", "cls", "shell", "pong", "ps", "about", "history"};

// created a struct store history of the commands typed
typedef struct _shell_history {
  char* cmd;
  struct _shell_history* next;
} shell_history;

// prints all the available commands -- help
void print_help(int window_id) {
  wm_print(window_id, "Available TOS commands:\n");
  wm_print(window_id, "help    - Prints all supported TOS commands\n");
  wm_print(window_id, "cls     - Clears the shell screen\n");
  wm_print(window_id, "shell   - Launches another shell\n");
  wm_print(window_id, "pong    - Launches pong game\n");
  wm_print(window_id, "ps      - Prints the PROCESS table\n");
  wm_print(window_id, "about   - Prints author's information\n");
  wm_print(window_id, "history - Prints all the commands that have been typed into the shell\n");
  wm_print(window_id, "!<num>  - Repeats the command with the given number\n");
  wm_print(window_id, "echo    - Echoes to the console the string that followed the command\n");
}

// prints author's information -- about
void print_about(int window_id) {
  wm_print(window_id, "Shell Commands Implemented by Vipul Karanjkar\n");
}

// copy newely typed command to history
void string_copy (int window_id, char* history_cmd, char* new_cmd) {
  int index = 0;

  while(new_cmd[index] != '\0') {
    // store value of new_cmd in history_cmd
    history_cmd[index] = new_cmd[index];
    index++;
  }

  history_cmd[index + 1] = '\0';
}

// get all the commands typed -- history
void get_history(int window_id, shell_history* head) {
  int index = 0;
  shell_history* temp = head;

  while(temp != NULL) {
    wm_print(window_id, "%d: %s \n", index, temp->cmd);
    index++;
    temp = temp->next;
  }
}

// execute pling(exclamation) command -- !num
void execute_pling_command(int window_id, int history_index, shell_history* head) {
  int index = 0;

  while(head->next != NULL) {
    if (index == history_index) {
      wm_print(window_id, "%s\n", head->cmd);
      find_shell_command(window_id, head->cmd, head);
      return;
    }
    index++;
    head = head->next;
  }
}

// check if the !num exists in the history stored
void check_pling_command(int window_id, char* command, int command_index, int history_index, shell_history* head) {
  // since 1st char is ! -- start from index 1
  int index = 1;
  int cmd_num = 0;

  while (command[index] != '\0') {
    if (command[index] >= '0' && command[index] <= '9') {
      cmd_num = cmd_num * 10 + (command[index] - '0');
      index++;
    } else {
      wm_print(window_id, "Event not found");
      return;
    }
  }

  if (cmd_num > history_index) {
    wm_print(window_id, "Event not found");
    return;
  } else {
    execute_pling_command(window_id, cmd_num, head);
  }
}

// print process heading
void print_processes_heading(int window_id) {
  wm_print(window_id, "State            Active        Prio   Name\n");
  wm_print(window_id, "----------------------------------------------------------\n");
}

// print process details
void print_processes_details(int window_id, PROCESS p) {
  static const char *state[] = { "READY          ",
      "ZOMBIE         ",
      "SEND_BLOCKED   ",
      "REPLY_BLOCKED  ",
      "RECEIVE_BLOCKED",
      "MESSAGE_BLOCKED",
      "INTR_BLOCKED   "
  };
  if (!p->used) {
      wm_print(window_id, "PCB slot unused!\n");
      return;
  }
  /* State */
  wm_print(window_id, state[p->state]);
  /* Check for active_proc */
  if (p == active_proc)
      wm_print(window_id, "     *      ");
  else
      wm_print(window_id, "            ");
  /* Priority */
  wm_print(window_id, "   %2d", p->priority);
  /* Name */
  wm_print(window_id, "      %s\n", p->name);
}

// print process -- ps
void print_processes(int window_id) {
  int i;
  PCB* p = pcb;

  print_processes_heading(window_id);
  for (i = 0; i < MAX_PROCS; i++, p++) {
      if (!p->used)
          continue;
      print_processes_details(window_id, p);
    }
}

// compare strings
int string_compare (char* str1, char* str2) {
  int index = 0;

  while(str1[index] == str2[index] && str1[index] != '\0') {
    index++;
  }
  if (str1[index] != str2[index]) {
    return 0; // strings not equal
  } else {
    return 1; // strings equal
  }
}

// echoes to the console the string that followed the command -- echo
void print_echo(int window_id, char* command) {
  int read_index = 5;
  int write_index = 0;
  char echo_output[500];

  while(command[read_index] != '\0') {
    if (command[read_index] == 34 || command[read_index] == 39) {
      read_index++;
      continue;
    }
    echo_output[write_index] = command[read_index];
    read_index++;
    write_index++;
  }
  if (string_compare(command, "None")) {
    wm_print(window_id, "\n");
  } else {
  wm_print(window_id, "%s", echo_output);
  }
}

// find index of the command from the available list of shell commands
void find_shell_command(int window_id, char *command, shell_history* head) {
  int command_index = -1;
  for (int i = 0; i < TOTAL_SHELL_COMMANDS; i++) {
    char* available_commands = shell_comands[i];

    // if strings match with the available shell command --> run the respective shell command
    if (string_compare(command, available_commands) == 1) {
      command_index = i;
    }
  }
  execute_shell_command(window_id, command_index, head);
}

// run shell command based on the command typed by the user
void execute_shell_command(int window_id, int command_index, shell_history* head) {
  switch(command_index) {
    case 0: print_help(window_id);
    break;
    case 1: wm_clear(window_id);
    break;
    case 2: start_shell();
    break;
    case 3: start_pong();
    break;
    case 4: print_processes(window_id);
    break;
    case 5: print_about(window_id);
    break;
    case 6: get_history(window_id, head);
    break;
    default: wm_print(window_id, "Command Not Found");
  };
}

// clear command buffer
void clear_command_buffer(int window_id, char* command) {
  wm_print(window_id, "\n%s ", SHELL_SYMBOL);
  for (int i = 0; i < 500; i++) {
    command[i] = '\0';
  }
}

// entry point for shell process
void shell_process(PROCESS self, PARAM param) {
  char command[500];
  shell_history* head = NULL;
  shell_history* tail = NULL;
  int command_index = 0;
  int history_index = 0;
  int input_length = 0;

  int window_id = wm_create(4, 2, SHELL_WIDTH, SHELL_HEIGHT);
  wm_print(window_id, "%s ", SHELL_SYMBOL);

  while (42) {
    char key = keyb_get_keystroke(window_id, TRUE);

    // ignore initial whitespaces(ASCII Dec = 32)
    if (key == 32) {
      input_length++;
      command[command_index] = key;
      wm_print(window_id, "%c", key);
      continue;
    }

    // ignores empty enter(ASCII Dec = 13) key press
    if (key == 13) {
      wm_print(window_id, "\n%s ", SHELL_SYMBOL);
      continue;
    }

    // read the command until enter key is not pressed
    while(key != 13) {
      // when backspace(ASCII Dec = 8) is pressed - remove previous char
      if(key == 8) {
         if (command_index > 0) {
           input_length--;
           command_index--;
           command[command_index] = '\0';
           wm_print(window_id, "%c", key);
      }
      if (command_index == 0 && input_length > 0) {
        input_length--;
        command[command_index] = key;
        wm_print(window_id, "%c", key);
      }
    }

      if (key != 8) {
        command[command_index] = key;
        input_length++;
        command_index++;
        wm_print(window_id, "%c", key);
      }

      key = keyb_get_keystroke(window_id, TRUE);
    }
      // ignore trailing whitespaces
      for (int i = command_index - 1; i > 0; i--) {
        if (command[i] != 32) {
          break;
        }
        command_index--;
        command[i] = '\0';
      }

    wm_print(window_id, "\n");

    // create space for the newely typed command
    shell_history* new_command = (shell_history*) malloc(sizeof(shell_history));
    new_command->cmd = (char*) malloc((command_index + 1) * sizeof(char));
    string_copy(window_id, new_command->cmd, command);
    new_command->next = NULL;
    history_index++;

    if (head == NULL) {
      // first command
      head = new_command;
      tail = new_command;
    } else {
      // add to the tail
      tail->next = new_command;
      tail = tail->next;
    }

    if (command[0] == 33) {
      // pling(ASCII Dec = 33) -- British slang for exclamation mark
      check_pling_command(window_id, command, command_index, history_index, head);
    } else if(k_memcmp(command, "echo", 4) == 0) {
      print_echo(window_id, command);
    } else {
      find_shell_command(window_id, command, head);
    }

    clear_command_buffer(window_id, command);
    command_index = 0;
  }
}

// starts shell
void start_shell() {
  create_process(shell_process, 5, 0, "Shell Process");
  resign();
}
