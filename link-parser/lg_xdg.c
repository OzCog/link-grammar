/***************************************************************************/
/* Amir Plivatsky                                                          */
/*                                                                         */
/* Use of the link grammar parsing system is subject to the terms of the   */
/* license set forth in the LICENSE file included with this software.      */
/* This license allows free redistribution and use in source and binary    */
/* forms, with or without modification, subject to certain conditions.     */
/*                                                                         */
/***************************************************************************/

/**
 * This file includes minimal implementation of the XDG
 * specification version 0.8. See:
 * https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
 *
 * It is supports just what needed to support the history file location.
 */

#if HAVE_WIDECHAR_EDITLINE
#include <ctype.h>
#include <stdarg.h>

#include "link-grammar/link-includes.h"
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>                     // malloc etc.
#include <string.h>
#include <errno.h>

#include "lg_xdg.h"
#include "parser-utilities.h"

typedef struct
{
	const char *rel_path;
	const char *env_var;
} xdg_definition;;

xdg_definition xdg_def[] =
{
	{ "/.local/state", "XDG_STATE_HOME"  },
	// Add more definitions if needed.
};

static bool is_empty(const char *path)
{
	return path == NULL || path[0] == '\0';
}

static bool is_absolute_path(const char *path)
{
	if (is_empty(path)) return false; // Empty path is not absolute

#if defined(_WIN32) || defined(__CYGWIN__)
	// Check for Windows absolute paths.
	if ((isalpha((unsigned char)path[0]) && path[1] == ':' &&
	     (path[2] == '\\' || path[2] == '/')) ||
	    (path[0] == '\\' && path[1] == '\\'))
	{
		return true;
	}
#endif

	// Check for POSIX absolute path.
	if (path[0] == '/') return true;

	return false; // Path is not absolute
}

static bool is_sep(int c)
{
#if defined(_WIN32) || defined(__CYGWIN__)
	if (c == '\\') return true;
#endif
	if (c == '/') return true;

	return false;
}

/**
 * Make directories for each directory component of \p path.
 * If the last component is not ending with "/", it is considered
 * a file name.
 */
static bool make_dirpath(const char *path)
{
	char *dir = strdup(path);
	struct stat sb;

#if defined(_WIN32) || defined(__CYGWIN__)
	// Skip Windows UNC path \\X
	if (is_sep(dir[0]) && is_sep(dir[1]))
	{
		// Start from the root or network share
		for (p = dir + 2; *p != '\0'; p++)
			if (is_sep(*p)) break;
		if (*p == '\0') return true;  // No further subdirectories
	}
#endif

	for (char *p = dir+1; '\0' != *p; p++)
	{
		char sep = *p;
		if (is_sep(*p))
		{
			*p = '\0'; // Now dir is the path up to this point
			//prt_error("DEBUG: mkdir: '%s'\n", dir);
			mkdir(dir, S_IRWXU); // Try to create the directory
			if (stat(dir, &sb) != 0 || !S_ISDIR(sb.st_mode))
			{
				prt_error("Error: Cannot create directory '%s'\n", dir);
				free(dir);
				return false;
			}
			*p = sep;
		}
	}

	free(dir);
	return true;
}

const char *xdg_get_home(xdg_basedir_type bd_type)
{
	const char *def_suffix = xdg_def[bd_type].rel_path;
	const char *evars[] = { xdg_def[bd_type].env_var, "HOME",
#if defined(_WIN32) || defined(__CYGWIN__)
		"USERPROFILE"
#endif
	};
	char *dir;

	size_t num_evars = sizeof(evars)/sizeof(evars[0]);
	size_t i;
	for (i = 0; i < num_evars; i++)
	{
		dir = getenv(evars[i]);
		if (is_empty(dir)) continue;

		if (is_absolute_path(dir)) break;
		if (num_evars - 1 != i) // Avoid a double notification
		{
			prt_error("Warning: %s is not an absolute path (ignored).\n",
			          evars[i]);
		}
	}

	if (!is_absolute_path(dir))
	{
		prt_error("Error: %s is not set or is not an absolute path.\n",
		          evars[num_evars - 1]);
		return NULL;
	}

	char *def_dir;
	// def_suffix is a directory - append '/' so make_dirpath() will create it.
	int n = asprintf(&def_dir, "%s%s/", dir, (i > 0) ? def_suffix : "");
	if (!make_dirpath(def_dir))
	{
		free(def_dir);
		return NULL;
	}
	def_dir[n-1] = '\0'; // Remove the last '/'

	return def_dir;
}

const char *xdg_get_program_name(const char *argv0)
{
	if ((NULL == argv0) || ('\0' == argv0[0])) return NULL;

	const char *basename = argv0;
	for (const char *p = argv0; *p != '\0'; p++)
		if (is_sep(*p)) basename = p + 1;

	if (0 == strcmp(basename, "..")) return NULL;

	return basename;
}

const char *xdg_make_path(xdg_basedir_type bd_typec, const char *fmt, ...)
{
	const char *xdg_home = xdg_get_home(bd_typec);

	char *xdg_fmt;
	asprintf(&xdg_fmt, "%s/%s", xdg_home, fmt);

	va_list filename_components;
	va_start(filename_components, fmt);
	char *xdg_filepath;
	vasprintf(&xdg_filepath, xdg_fmt, filename_components);
	va_end(filename_components);

	free((void *)xdg_home);
	free(xdg_fmt);
	if (!make_dirpath(xdg_filepath))
	{
		free(xdg_filepath);
		return NULL;
	}
	return xdg_filepath;
}
#endif /* HAVE_WIDECHAR_EDITLINE */
