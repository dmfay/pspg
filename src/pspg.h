/*-------------------------------------------------------------------------
 *
 * pspg.h
 *	  themes initialization
 *
 * Portions Copyright (c) 2017-2022 Pavel Stehule
 *
 * IDENTIFICATION
 *	  src/pspg.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef PSPG_PSPG_H
#define PSPG_PSPG_H

#include <sys/types.h>
#include <stdio.h>

#include "commands.h"
#include "config.h"
#include "themes.h"
#include "st_menu.h"

#define LINEINFO_NONE					0
#define LINEINFO_BOOKMARK				1
#define LINEINFO_FOUNDSTR				2
#define LINEINFO_FOUNDSTR_MULTI			4
#define LINEINFO_UNKNOWN				8
#define LINEINFO_CONTINUATION			16
#define LINEINFO_HASNOT_CONTINUATION	32

#define			FILE_UNDEF			0
#define			FILE_CSV			1
#define			FILE_TSV			2
#define			FILE_MATRIX			3

#define PSPG_VERSION "5.5.8"

/* GNU Hurd does not define MAXPATHLEN */
#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

typedef struct LineInfo
{
	unsigned char	mask;
	short int		start_char;
	short int		recno_offset;
} LineInfo;

#define	LINEBUFFER_LINES		1000

typedef struct LineBuffer
{
	int		first_row;
	int		nrows;
	char   *rows[LINEBUFFER_LINES];
	LineInfo	   *lineinfo;
	struct LineBuffer *next;
	struct LineBuffer *prev;
} LineBuffer;

typedef struct
{
	LineBuffer	   *lnb;
	int				lnb_row;
} MappedLine;

typedef enum
{
	INFO_UNKNOWN,
	INFO_DOUBLE,
	INFO_STRXFRM
} SortDataInfo;

typedef struct
{
	SortDataInfo		info;
	double			d;
	char		   *strxfrm;
	LineBuffer	   *lnb;
	int				lnb_row;
} SortData;

/*
 * Column range
 */
typedef struct
{
	int		xmin;
	int		xmax;
	int		name_pos;				/* column's name position in chars */
	int		name_width;				/* size of column name in display chars */
	int		name_offset;			/* start of column's name in bytes */
	int		name_size;				/* size of column name in bytes */
} CRange;

/*
 * Available formats of headline chars
 *
 *  L, R   .. outer border
 *  I      .. inner border
 *  d      .. data
 */

/*
 * This structure should be immutable
 */
typedef struct
{
	int		border_top_row;			/* nrow of bottom outer border or -1 */
	int		border_head_row;		/* nrow of head outer (required) */
	int		border_bottom_row;		/* nrow of bottom outer border or -1 */
	int		border_type;			/* detected type of border: 0, 1, 2 */
	char	linestyle;				/* detected linestyle: a, u */
	bool	is_expanded_mode;		/* true when data are in expanded mode */
	bool	is_pgcli_fmt;			/* table was generated by pgcli */
	int		expanded_info_minx;		/* begin of info in \x mode .. RECORD x */
	char	title[65];				/* detected title (trimmed) or NULL */
	int		title_rows;				/* number of rows used as table title (skipped later) */
	char	filename[65];			/* filename (printed on top bar) */
	LineBuffer rows;				/* list of rows buffers */
	int		total_rows;				/* number of input rows */
	MappedLine *order_map;			/* maps sorted lines to original lines */
	int		order_map_items;		/* number of items of order map */
	int		maxy;					/* maxy of used pad area with data */
	int		maxx;					/* maxx of used pad area with data */
	int		maxbytes;				/* max length of line in bytes */
	char   *namesline;				/* header column's names line */
	char   *headline;				/* header separator line */
	int		headline_size;			/* size of headerline in bytes */
	char   *headline_transl;		/* translated headline */
	int		headline_char_size;		/* size of headerline in chars */
	CRange *cranges;				/* pairs of start, end of columns */
	int		columns;				/* number of columns */
	int		first_data_row;			/* fist data row line (starts by zero) */
	int		last_data_row;			/* last line of data row */
	int		footer_row;				/* nrow of first footer row or -1 */
	int		alt_footer_row;			/* alternative footer row (used when border = 1) */
	int		footer_char_size;		/* width of footer */
	int		last_row;				/* last not empty row */
	int		fixed_rows;				/* number of fixed rows */
	int		fixed_columns;			/* number of fixed columns */
	int		data_rows;				/* number of data rows */
	bool	freeze_two_cols;			/* detected system table with first oid column */
	bool	multilines_already_tested;	/* true, when we know where are multilines */
	bool	has_multilines;			/* true, when some field contains more lines */

	bool	initialized;			/* used as protection against unwanted initialization
									   in progress mode */
	bool	completed;				/* true, when last input row was processed */
	LineBuffer *last_buffer;		/* pointer to last LineBuffer */

	bool	load_data_rows;			/* true, when loaded rows holds data */
} DataDesc;

#define		PSPG_WINDOW_COUNT				10
#define		PSPG_WINDOW_THEMES_COUNT		13

/*
 * This structure can be mutable - depends on displayed data
 */
typedef struct
{
	int		fix_rows_rows;			/* current number of fixed rows in window rows */
	int		fix_cols_cols;			/* current number of fixed columns in window rows */
	int		rows_rows;				/* current number of data rows */
	int		footer_rows;			/* current number of footer rows */
	int		maxy;					/* max y size of screen */
	int		maxx;					/* max x size of screen */
	int		main_maxy;				/* max y size of main place (fixed + data + footer rows) */
	int		main_maxx;				/* max x size of main place (maxx - main_start_x) */
	int		main_start_y;			/* y position of first row of main place */
	int		main_start_x;			/* x position of first row of main place */
	int		top_bar_rows;			/* 1 or 0 when top bar is not used */
	WINDOW	*wins[PSPG_WINDOW_COUNT];
	Theme	themes[PSPG_WINDOW_THEMES_COUNT];
	char	searchterm[256];		/* currently active search input */
	int		searchterm_char_size;	/* size of searchterm in chars */
	int		searchterm_size;		/* size of searchterm in bytes */

	int		search_first_row;
	int		search_rows;
	int		search_first_column;
	int		search_columns;
	bool	search_selected_mode;	/* true, when searching is limitted by selected area */

	bool	has_upperchr;			/* true, when search term has upper char */
	bool	found;					/* true, when last search was successfull */
	int		found_start_x;			/* x position of found pattern */
	int		found_start_bytes;		/* bytes position of found pattern */
	int		found_row;				/* row of found pattern */
	int		first_rec_title_y;		/* y of first displayed record title in expanded mode */
	int		last_rec_title_y;		/* y of last displayed record title in expanded mode */
	char	searchcolterm[256];		/* last searched column patterm */
	int		searchcolterm_size;		/* length of searched column pattern in bytes */

	int		scrollbar_maxy;			/* max y of horisontal scrollbar */
	int		scrollbar_start_y;		/* start y dim of horisontal scrollbar */
	int		scrollbar_x;			/* x position of horisontal scrollbar */
	int		slider_min_y;			/* position of scrollbar slider */
	int		slider_size;			/* height of slider (vertical) */

	bool	scrollbar_mode;			/* true, when vertical scrollbar is active */
	bool	slider_has_position;	/* protection against visual defects from twice setting of *
									 * slider position. After leaving of scrollbar mode        *
									 * we should not to set slider position from cursor        *
									 * position */

	int		selected_first_row;
	int		selected_rows;
	int		selected_first_column;
	int		selected_columns;
} ScrDesc;

#define		w_luc(scrdesc)			((scrdesc)->wins[WINDOW_LUC])
#define		w_fix_rows(scrdesc)		((scrdesc)->wins[WINDOW_FIX_ROWS])
#define		w_fix_cols(scrdesc)		((scrdesc)->wins[WINDOW_FIX_COLS])
#define		w_rows(scrdesc)			((scrdesc)->wins[WINDOW_ROWS])
#define		w_footer(scrdesc)		((scrdesc)->wins[WINDOW_FOOTER])
#define		w_top_bar(scrdesc)		((scrdesc)->wins[WINDOW_TOP_BAR])
#define		w_bottom_bar(scrdesc)	((scrdesc)->wins[WINDOW_BOTTOM_BAR])
#define		w_rownum(scrdesc)		((scrdesc)->wins[WINDOW_ROWNUM])
#define		w_rownum_luc(scrdesc)	((scrdesc)->wins[WINDOW_ROWNUM_LUC])
#define		w_vscrollbar(scrdesc)	((scrdesc)->wins[WINDOW_VSCROLLBAR])

/*
 * Used for storing not yet formatted data
 */
typedef struct
{
	int		nfields;
	char   *fields[];
} RowType;

typedef struct _rowBucketType
{
	int			nrows;
	RowType	   *rows[1000];
	bool		multilines[1000];
	bool		allocated;
	struct _rowBucketType *next_bucket;
} RowBucketType;

/*
 * Used for formatting
 */
typedef struct
{
	int		nfields;
	int		nfields_all;
	bool	has_header;
	char	types[1024];			/* a or d .. content in column */
	int		widths[1024];			/* column's display width */
	bool	multilines[1024];		/* true if column has multiline row */
	int		columns_map[1024];		/* column numbers - used when some column is hidden */
} PrintDataDesc;

/*
 * holds pager state data
 */
typedef struct
{
	bool	only_for_tables;
	bool	no_interactive;
	bool	interactive;
	bool	ignore_file_suffix;
	bool	stream_mode;
	bool	no_alternate_screen;
	bool	quit_if_one_screen;

	int		reserved_rows;			/* used by dbcli */
	int		boot_wait;
	int		file_format_from_suffix;

	const char *errstr;				/* ptr to error string */
	int		_errno;					/* saved errno */

	char   *fmt;					/* format string for info when refresh first is required */
	char   *par;					/* parameter for info when refresh first is required */
	bool	beep;					/* beep for info when refresh is required */
	bool	applytimeout;			/* true, when saved info should be close after timeout */
	bool	is_error;				/* true, when saved info should be displayed as error */
	bool	refresh_scr;			/* force rewrite screen */

	ScrDesc *scrdesc;				/* used for redraw */
	DataDesc *desc;					/* used for input, for access to necessary data for tabcomplete */
	Options *opts;					/* used for redraw */

	int		theme_template;
	int		menu_template;
} StateData;


typedef struct
{
	LineBuffer	   *start_lb;
	MappedLine	   *order_map;
	int				order_map_items;

	LineBuffer	   *current_lb;
	int				current_lb_rowno;
	int				lineno;
} LineBufferIter;

typedef struct
{
	LineBuffer	   *lb;
	int				lb_rowno;
	int				lineno;
} LineBufferMark;

typedef struct
{
	LineBuffer	   *lb;
	int				lb_rowno;
} SimpleLineBufferIter;

typedef struct
{
	int		len;
	int		maxlen;
	char   *data;
} ExtStr;

/* from pspg.c */
extern void exit_handler(void);
extern void export_to_file(PspgCommand command, ClipboardFormat format, Options *opts, ScrDesc *scrdesc,
						   DataDesc *desc, int cursor_row, int cursor_column, int rows, double percent,
						   const char *pipecmd, bool *force_refresh);
extern void throw_searching(ScrDesc *scrdesc, DataDesc *desc);
extern void enable_xterm_mouse_mode(bool enable);
extern bool disable_xterm_mouse_mode(void);
extern void show_info_wait(const char *fmt, const char *par, bool beep,
						   bool refresh_first, bool applytimeout, bool is_error);

extern void current_time(time_t *sec, long *ms);
extern void refresh_terminal_size(void);
extern void refresh_layout_after_terminal_resize(void);
extern void redraw_screen(void);
extern int min_int(int a, int b);
extern int max_int(int a, int b);

#define time_diff(s1, ms1, s2, ms2)		((s1 - s2) * 1000 + ms1 - ms2)

#define UNUSED(expr) do { (void)(expr); } while (0)


/* from print.c */
extern void window_fill(int window_identifier, int srcy, int srcx, int cursor_row, int vcursor_xmin, int vcursor_xmax,
	int selected_xmin, int selected_xmax, DataDesc *desc, ScrDesc *scrdesc, Options *opts);
extern void draw_data(Options *opts, ScrDesc *scrdesc, DataDesc *desc, int first_data_row, int first_row, int cursor_col, int footer_cursor_col, int fix_rows_offset);
extern LineInfo *set_line_info(Options *opts, ScrDesc *scrdesc, DataDesc *desc, LineBufferMark *lbm, char *rowstr);

#define PSPG_ERRSTR_BUFFER_SIZE		2048
extern char pspg_errstr_buffer[PSPG_ERRSTR_BUFFER_SIZE];

extern bool is_expanded_header(char *str, int *ei_minx, int *ei_maxx);
extern int min_int(int a, int b);
extern const char *nstrstr(const char *haystack, const char *needle);
extern const char *nstrstr_ignore_lower_case(const char *haystack, const char *needle);
extern bool nstreq(const char *str1, const char *str2);

extern const char *pspg_search(Options *opts, ScrDesc *scrdesc, const char *str);

/* from menu.c */
extern void init_menu_config(Options *opts);
extern struct ST_MENU *init_menu(struct ST_MENU *current_menu, Options *opts);
extern struct ST_CMDBAR *init_cmdbar(struct ST_CMDBAR *current_cmdbar, Options *opts);
extern void post_menu(Options *opts, struct ST_MENU *current_menu);
extern void refresh_clipboard_options(Options *opts, struct ST_MENU *menu);
extern void refresh_copy_target_options(Options *opts, struct ST_MENU *menu);

/* from sort.c */
extern void sort_column_num(SortData *sortbuf, int rows, bool desc);
extern void sort_column_text(SortData *sortbuf, int rows, bool desc);

/* from pretty-csv.c */
extern bool read_and_format(Options *opts, DataDesc *desc, StateData *state);

/* from pgclient.c */
extern bool pg_exec_query(Options *opts, char *query, RowBucketType *rb, PrintDataDesc *pdesc, const char **err);

/* from args.c */
extern char **buildargv(const char *input, int *argc, char *appname);
extern bool readargs(char **argv, int argc, Options *opts, StateData *state);
extern bool args_are_consistent(Options *opts, StateData *state);

/* from infra.c */
extern void log_row(const char *fmt, ...);
extern void leave(const char *fmt, ...)  __attribute__ ((noreturn));
extern void format_error(const char *fmt, ...);

extern void *smalloc(int size);
extern void *smalloc2(int size, char *debugstr);
extern void *srealloc(void *ptr, int size);
extern char *sstrdup(const char *str);
extern char *sstrdup2(const char *str, char *debugstr);
extern char *sstrndup(const char *str, int bytes);

extern int charlen(const char *str);
extern int dsplen(const char *str);
extern char *trim_str(const char *str, int *size);
extern char *trim_quoted_str(const char *str, int *size);
extern void InitExtStr(ExtStr *estr);
extern void ResetExtStr(ExtStr *estr);
extern void ExtStrAppendNewLine(ExtStr *estr, char *str);
extern void ExtStrAppendLine(ExtStr *estr, char *str, int size, char linestyle, bool continuation_mark, bool continuation_mark2);
extern int ExtStrTrimEnd(ExtStr *estr, bool replace_nl);

extern int rwe_popen(char *command, int *fin, int *fout, int *ferr);

extern char *tilde(char *dest, const char *path);

/* from table.c */
extern bool readfile(Options *opts, DataDesc *desc, StateData *state);
extern bool translate_headline(DataDesc *desc);
extern void multilines_detection(DataDesc *desc);

extern void update_order_map(ScrDesc *scrdesc, DataDesc *desc, int sbcn, bool desc_sort);

/* from string.c */
extern const char *nstrstr(const char *haystack, const char *needle);
extern const char *nstrstr_ignore_lower_case(const char *haystack, const char *needle);
extern bool nstreq(const char *str1, const char *str2);
extern const char *nstrstr_with_sizes(const char *haystack, const int haystack_size,
				   const char *needle, int needle_size);
extern bool nstarts_with_with_sizes(const char *str, int str_size, const char *pattern, int pattern_size);

/* from export.c */
extern bool export_data(Options *opts, ScrDesc *scrdesc, DataDesc *desc,
						int cursor_row, int cursor_column,
						FILE *fp,
						int rows, double percent, char *table_name,
						PspgCommand cmd, ClipboardFormat format);

/* from linebuffer.c */
extern void init_lbi(LineBufferIter *lbi, LineBuffer *lb, MappedLine *order_map, int order_map_items, int init_pos);
extern void init_lbi_ddesc(LineBufferIter *lbi, DataDesc *desc, int init_pos);
extern bool lbi_set_lineno(LineBufferIter *lbi, int pos);
extern void lbi_set_mark(LineBufferIter *lbi, LineBufferMark *lbm);
extern bool lbi_set_mark_next(LineBufferIter *lbi, LineBufferMark *lbm);
extern bool lbm_get_line(LineBufferMark *lbm, char **line, LineInfo **linfo, int *lineno);
extern bool lbi_get_line(LineBufferIter *lbi, char **line, LineInfo **linfo, int *lineno);
extern bool lbi_get_line_prev(LineBufferIter *lbi, char **line, LineInfo **linfo, int *lineno);
extern bool lbi_get_line_next(LineBufferIter *lbi, char **line, LineInfo **linfo, int *lineno);
extern bool lbi_prev(LineBufferIter *lbi);
extern bool lbi_next(LineBufferIter *lbi);
extern SimpleLineBufferIter *init_slbi_ddesc(SimpleLineBufferIter *slbi, DataDesc *desc);
extern SimpleLineBufferIter *slbi_get_line_next(SimpleLineBufferIter *slbi, char **line, LineInfo **linfo);
extern bool ddesc_set_mark(LineBufferMark *lbm, DataDesc *desc, int pos);
extern void lbm_xor_mask(LineBufferMark *lbm, char mask);
extern void lbm_recno_offset(LineBufferMark *lbm, short int recno_offset);
extern void lb_free(DataDesc *desc);
extern void lb_print_all_ddesc(DataDesc *desc, FILE *f);
extern const char *getline_ddesc(DataDesc *desc, int pos);

/* from bscommands.c */
extern const char *get_token(const char *instr, const char **token, int *n);
extern const char *parse_and_eval_bscommand(const char *cmdline, Options *opts, ScrDesc *scrdesc, DataDesc *desc,
											int *next_command, long *long_argument, bool *long_argument_is_valid,
											char **string_argument, bool *string_argument_is_valid, bool *refresh_clear);

#define IS_TOKEN(str, n, token)		((n == strlen(token)) && (strncmp(str, token, n) == 0))

/* from readline.c */
extern void pspg_init_readline(const char *histfile);
extern void pspg_save_history(const char *histfile, Options *opts);
extern bool get_string(char *prompt, char *buffer, int maxsize, char *defstr, char tabcomplete_mode);

/*
 * Global constants
 */
#define PSPG_ESC_CODE							27

/*
 * Global setting
 */
extern bool use_utf8;
extern bool quiet_mode;

/*
 * Global variables
 */
extern bool	handle_sigint;
extern bool handle_sigwinch;

extern StateData *current_state;
extern WINDOW *prompt_window;
extern attr_t prompt_window_input_attr;
extern attr_t prompt_window_error_attr;
extern attr_t prompt_window_info_attr;
extern FILE *logfile;

/*
 * REMOVE THIS COMMENT FOR DEBUG OUTPUT
 * and modify a path.
 *
#define DEBUG_PIPE				"/home/pavel/debug"
 */

#ifdef DEBUG_PIPE

extern FILE *debug_pipe;
extern int	debug_eventno;

#endif

#endif
