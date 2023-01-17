/*
 * navigateOptions.h
 *
 *  Created on: 2 окт. 2022 г.
 *      Author: Maksim Ilyin (imax9.narod.ru)
 */

#ifndef INC_NAVIGATEOPTIONS_H_
#define INC_NAVIGATEOPTIONS_H_
#include "osd.h"
// the maximum of options to be shown
// DONT CHANGE UNLESS YOU REALLY KNOW WHAT YOU'RE DOING
#define MAX_OPTIONS 32

// the maximum length of an option label
#define OPTION_LEN 24
// the maximum length of selections (0xf8)
#define SELECT_LEN 8

#define MENU_MAX_LINES 8

struct NavigateOption {
  unsigned int idx;
  char label[SCREEN_WIDTH_IN_CHARS];
  int load_slot;
  int first_opt;
  enum option_type { option = 0, toggle = 1, info = 2, set_root = 96, mount_image = 97, load_new_core = 98, load_file = 99} option_type;

  int n_values;
  int sel_idx;
  int sel_opt;
  char sel_label[SELECT_LEN][OPTION_LEN];
};


#endif /* INC_NAVIGATEOPTIONS_H_ */
