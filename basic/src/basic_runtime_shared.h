#ifndef BASIC_RUNTIME_SHARED_H
#define BASIC_RUNTIME_SHARED_H

#include <stddef.h>
#include <stdint.h>

#include "basic_num.h"

void basic_print (basic_num_t);
void basic_print_str (const char *);

#ifndef BASIC_USE_FIXED64
basic_num_t basic_input (void);
basic_num_t basic_read (void);
basic_num_t basic_rnd (basic_num_t);
basic_num_t basic_pos (void);
basic_num_t basic_input_hash (int64_t);
#endif

char *basic_input_str (void);
char *basic_get (void);
char *basic_inkey (void);
void basic_put (const char *);
void basic_return_error (void);

void basic_profile_reset (void);
void basic_profile_dump (void);
void basic_profile_line (basic_num_t);
void basic_profile_func_enter (const char *);
void basic_profile_func_exit (const char *);

int basic_strcmp (const char *, const char *);
char *basic_read_str (void);
void basic_restore (void);
void basic_clear_array (void *, basic_num_t, basic_num_t);
void *basic_dim_alloc (void *, basic_num_t, basic_num_t);
char *basic_strdup (const char *);
void basic_free (char *);
basic_num_t basic_int (basic_num_t);
basic_num_t basic_timer (void);
basic_num_t basic_time (void);
basic_num_t basic_date (void);
char *basic_time_str (void);
char *basic_date_str (void);
char *basic_input_chr (int64_t);
basic_num_t basic_peek (basic_num_t);
void basic_poke (int64_t, int64_t);
void basic_open (int64_t, const char *);
void basic_close (int64_t);
void basic_print_hash (int64_t, basic_num_t);
void basic_print_hash_str (int64_t, const char *);
char *basic_input_hash_str (int64_t);
char *basic_get_hash (int64_t);
void basic_put_hash (int64_t, const char *);
basic_num_t basic_eof (int64_t);
void basic_stop (void);
void basic_set_error_handler (basic_num_t);
basic_num_t basic_get_error_handler (void);
void basic_set_line (basic_num_t);
basic_num_t basic_get_line (void);
void basic_enable_line_tracking (basic_num_t);
void basic_delay (int64_t);
void basic_beep (void);
void basic_sound (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
void basic_sound_off (void);
basic_num_t basic_system (const char *);
char *basic_system_out (void);

typedef struct BasicData {
  int is_str;
  basic_num_t num;
  char *str;
} BasicData;

extern BasicData *basic_data_items;
extern size_t basic_data_len;
extern size_t basic_data_pos;

void basic_home (void);
void basic_vtab (int64_t);
void basic_randomize (basic_num_t, basic_num_t);
basic_num_t basic_abs (basic_num_t);
basic_num_t basic_sgn (basic_num_t);
int64_t basic_iabs (int64_t);
int64_t basic_isgn (int64_t);
basic_num_t basic_sqr (basic_num_t);
basic_num_t basic_sin (basic_num_t);
basic_num_t basic_cos (basic_num_t);
basic_num_t basic_tan (basic_num_t);
basic_num_t basic_atn (basic_num_t);
basic_num_t basic_sinh (basic_num_t);
basic_num_t basic_cosh (basic_num_t);
basic_num_t basic_tanh (basic_num_t);
basic_num_t basic_asinh (basic_num_t);
basic_num_t basic_acosh (basic_num_t);
basic_num_t basic_atanh (basic_num_t);
basic_num_t basic_asin (basic_num_t);
basic_num_t basic_acos (basic_num_t);
basic_num_t basic_log (basic_num_t);
basic_num_t basic_log2 (basic_num_t);
basic_num_t basic_log10 (basic_num_t);
basic_num_t basic_exp (basic_num_t);
basic_num_t basic_fact (basic_num_t);
basic_num_t basic_pow (basic_num_t, basic_num_t);
basic_num_t basic_pi (void);

void basic_screen (int64_t);
void basic_cls (void);
void basic_color (int64_t);
void basic_key_off (void);
void basic_locate (int64_t, int64_t);
void basic_tab (int64_t);
void basic_htab (int64_t);
void basic_text (void);
void basic_inverse (void);
void basic_normal (void);
void basic_hgr2 (void);
void basic_hcolor (basic_num_t);
void basic_hplot (basic_num_t, basic_num_t);
void basic_hplot_to (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
void basic_hplot_to_current (basic_num_t, basic_num_t);
void basic_move (basic_num_t, basic_num_t);
void basic_draw (basic_num_t, basic_num_t);
void basic_draw_line (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
void basic_circle (basic_num_t, basic_num_t, basic_num_t);
void basic_rect (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
void basic_fill (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
void basic_mode (basic_num_t);

char *basic_chr (int64_t);
char *basic_unichar (int64_t);
char *basic_string (int64_t, const char *);
char *basic_concat (const char *, const char *);
char *basic_upper (const char *);
char *basic_lower (const char *);
char *basic_left (const char *, int64_t);
char *basic_right (const char *, int64_t);
char *basic_mid (const char *, int64_t, int64_t);
char *basic_mirror (const char *);
long basic_len (const char *);
basic_num_t basic_val (const char *);
char *basic_str (basic_num_t);
long basic_asc (const char *);
long basic_instr (const char *, const char *);

#endif /* BASIC_RUNTIME_SHARED_H */
