/*
 * qucsconv.cpp - main converter program implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "logging.h"
#include "precision.h"
#include "check_spice.h"
#include "check_vcd.h"
#include "check_citi.h"
#include "check_touchstone.h"
#include "check_csv.h"
#include "check_zvr.h"
#include "check_mdl.h"
#include "check_dataset.h"
#include "qucs_producer.h"
#include "csv_producer.h"
#include "touchstone_producer.h"
#include "matlab_producer.h"
#include "dataset.h"

using namespace qucs;

/* structure defining a conversion */
struct actionset_t {
  const char * in;  /* -if parameter */
  const char * out; /* -of parameter */

  /* callback for the logic, return error code of application */
  int (* execute) (struct actionset_t *, char * infile, char * outfile);
};

/* data variable specification */
char * data_var = NULL;

/* required forward declarations */
int spice2qucs (struct actionset_t *, char *, char *);
int vcd2qucs   (struct actionset_t *, char *, char *);
int qucs2csv   (struct actionset_t *, char *, char *);
int qucs2touch (struct actionset_t *, char *, char *);
int citi2qucs  (struct actionset_t *, char *, char *);
int touch2qucs (struct actionset_t *, char *, char *);
int csv2qucs   (struct actionset_t *, char *, char *);
int zvr2qucs   (struct actionset_t *, char *, char *);
int mdl2qucs   (struct actionset_t *, char *, char *);
int qucs2mat   (struct actionset_t *, char *, char *);

/* conversion definitions */
struct actionset_t actionset[] = {
  { "spice",      "qucs",       spice2qucs },
  { "spice",      "qucslib",    spice2qucs },
  { "vcd",        "qucsdata",   vcd2qucs   },
  { "qucsdata",   "csv",        qucs2csv   },
  { "qucsdata",   "touchstone", qucs2touch },
  { "citi",       "qucsdata",   citi2qucs  },
  { "touchstone", "qucsdata",   touch2qucs },
  { "csv",        "qucsdata",   csv2qucs   },
  { "zvr",        "qucsdata",   zvr2qucs   },
  { "mdl",        "qucsdata",   mdl2qucs   },
  { "qucsdata",   "matlab",     qucs2mat   },
  { NULL, NULL, NULL}
};

/* opens the given file, fallback to stdin/stdout */
FILE * open_file (char * file, const char * flag) {
  FILE * fd = NULL;
  if (file) {
    if ((fd = fopen (file, flag)) == NULL) {
      fprintf (stderr, "cannot open file `%s': %s, using %s instead\n",
	       file, strerror (errno), flag[0] == 'r' ? "stdin" : "stdout");
      fd = flag[0] == 'r' ? stdin : stdout;
    }
  }
  else {
    fd = flag[0] == 'r' ? stdin : stdout;
  }
  return fd;
}

/* main entry point */
int main (int argc, char ** argv) {

  char * infile = NULL, * outfile = NULL, * input = NULL, * output = NULL;

  loginit ();

  // check program arguments
  for (int i = 1; i < argc; i++) {
    if (!strcmp (argv[i], "-v") || !strcmp (argv[i], "--version")) {
      fprintf (stdout,
#ifdef GIT_REVISION
	"QucsConverter " PACKAGE_VERSION " (" GIT_REVISION ") \n"
#else
	"QucsConverter " PACKAGE_VERSION "\n"
#endif

	"Copyright (C) 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>\n"
	"\nThis is free software; see the source for copying "
	"conditions.  There is NO\n"
	"warranty; not even for MERCHANTABILITY or FITNESS FOR A "
	"PARTICULAR PURPOSE.\n");
      return 0;
    }
    if (!strcmp (argv[i], "-h") || !strcmp (argv[i], "--help")) {
      fprintf (stdout,
	"Usage: %s [OPTION]...\n\n"
	"  -h, --help      display this help and exit\n"
	"  -v, --version   display version information and exit\n"
	"  -i  FILENAME    use file as input file (default stdin)\n"
	"  -o  FILENAME    use file as output file (default stdout)\n"
	"  -if FORMAT      input data specification (see FORMAT below)\n"
	"  -of FORMAT      output data specification (see FORMAT below)\n"
	"  -a, --noaction  do not include netlist actions in the output\n"
	"  -g  GNDNODE     replace ground node\n"
	"  -d  DATANAME    data variable specification\n"
	"  -c, --correct   enable node correction\n"
  "\nFORMAT: The input - output format pair should be one of the following:\n"
  "  inputformat - outputformat\n"
  "  spice       - qucs\n"
  "  spice       - qucslib\n"
  "  vcd         - qucsdata\n"
  "  qucsdata    - csv\n"
  "  qucsdata    - touchstone\n"
  "  citi        - qucsdata\n"
  "  touchstone  - qucsdata\n"
  "  csv         - qucsdata\n"
  "  zvr         - qucsdata\n"
  "  mdl         - qucsdata\n"
  "  qucsdata    - matlab\n"
	"\nReport bugs to <" PACKAGE_BUGREPORT ">.\n", argv[0]);
      return 0;
    }
    else if (!strcmp (argv[i], "-i")) {
      infile = argv[++i];
    }
    else if (!strcmp (argv[i], "-o")) {
      outfile = argv[++i];
    }
    else if (!strcmp (argv[i], "-if")) {
      input = argv[++i];
    }
    else if (!strcmp (argv[i], "-of")) {
      output = argv[++i];
    }
    else if (!strcmp (argv[i], "-a") || !strcmp (argv[i], "--noaction")) {
      qucs_actions = 0;
    }
    else if (!strcmp (argv[i], "-g")) {
      if (argv[++i]) qucs_gnd = argv[i];
    }
    else if (!strcmp (argv[i], "-d")) {
      if (argv[++i]) data_var = argv[i];
    }
    else if (!strcmp (argv[i], "-c") || !strcmp (argv[i], "--correct")) {
      vcd_correct = 1;
    }
  }

  // check input/output formats
  int infound = 0;
  int outfound = 0;
  for (int j = 0; actionset[j].in != NULL; j++) {
    int in = 0, out = 0;
    if (input && !strcmp (input, actionset[j].in)) {
      in = infound = 1;
    }
    if (output && !strcmp (output, actionset[j].out)) {
      out = outfound = 1;
    }
    if (in && out) {
      return actionset[j].execute (&actionset[j], infile, outfile);
    }
  }

  // no appropriate conversion found
  if (!infound) {
    fprintf (stderr, "invalid input data specification `%s'\n",
	     input ? input : "not given");
  }
  if (!outfound) {
    fprintf (stderr, "invalid output data specification `%s'\n",
	     output ? output : "not given");
  }
  fprintf (stderr, "invalid input/output data specification `%s->%s'\n",
	   input ? input : "not given", output ? output : "not given");
  return -1;
}

// SPICE to Qucs conversion.
int spice2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  if ((spice_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (spice_parse () != 0) {
    ret = -1;
  } else if (spice_checker () != 0) {
    ret = -1;
  }
  spice_lex_destroy ();
  if (spice_in)
    fclose (spice_in);
  if (ret) {
    spice_destroy ();
    return -1;
  }

  if ((qucs_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->out, "qucs"))
    qucs_producer ();
  else /* "qucslib" */
    qucslib_producer ();
  fclose (qucs_out);
  spice_destroy ();
  return 0;
}

// VCD to Qucs conversion.
int vcd2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  vcd_init ();
  if ((vcd_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (vcd_parse () != 0) {
    ret = -1;
  } else if (vcd_checker () != 0) {
    ret = -1;
  }
  vcd_lex_destroy ();
  if (vcd_in)
    fclose (vcd_in);
  if (ret) {
    vcd_destroy ();
    return -1;
  }

  if ((qucs_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->out, "qucsdata"))
    qucsdata_producer_vcd ();
  fclose (qucs_out);
  vcd_destroy ();
  return 0;
}

// Qucs dataset to CSV conversion.
int qucs2csv (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  if ((dataset_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (dataset_parse () != 0) {
    ret = -1;
  } else if (dataset_result == NULL) {
    ret = -1;
  } else if (dataset_check (dataset_result) != 0) {
    delete dataset_result;
    dataset_result = NULL;
    ret = -1;
  }
  qucs_data = dataset_result;
  dataset_result = NULL;
  dataset_lex_destroy ();
  if (dataset_in)
    fclose (dataset_in);
  if (ret)
    return -1;

  if ((csv_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->out, "csv")) {
    if (data_var != NULL)
      csv_producer (data_var, ";");
    else {
      fprintf (stderr, "no data variable given (passed by -d option)\n");
      ret = -1;
    }
    fclose (csv_out);
    return ret;
  }
  return -1;
}

// Qucs dataset to Touchstone conversion.
int qucs2touch (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  if ((dataset_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (dataset_parse () != 0) {
    ret = -1;
  } else if (dataset_result == NULL) {
    ret = -1;
  } else if (dataset_check (dataset_result) != 0) {
    delete dataset_result;
    dataset_result = NULL;
    ret = -1;
  }
  qucs_data = dataset_result;
  dataset_result = NULL;
  dataset_lex_destroy ();
  if (dataset_in)
    fclose (dataset_in);
  if (ret)
    return -1;

  if ((touchstone_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->out, "touchstone")) {
    touchstone_producer (data_var);
    fclose (touchstone_out);
    return ret;
  }
  return -1;
}

// Qucs dataset to Matlab conversion.
int qucs2mat (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  if ((dataset_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (dataset_parse () != 0) {
    ret = -1;
  } else if (dataset_result == NULL) {
    ret = -1;
  } else if (dataset_check (dataset_result) != 0) {
    delete dataset_result;
    dataset_result = NULL;
    ret = -1;
  }
  qucs_data = dataset_result;
  dataset_result = NULL;
  dataset_lex_destroy ();
  if (dataset_in)
    fclose (dataset_in);
  if (ret)
    return -1;

  if ((matlab_out = open_file (outfile, "wb")) == NULL)
    return -1;
  if (!strcmp (action->out, "matlab")) {
    matlab_producer ();
    fclose (matlab_out);
    return ret;
  }
  return -1;
}

// CITIfile to Qucs conversion.
int citi2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  citi_init ();
  if ((citi_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (citi_parse () != 0) {
    ret = -1;
  } else if (citi_check () != 0) {
    ret = -1;
  }
  citi_lex_destroy ();
  if (citi_in)
    fclose (citi_in);
  if (ret) {
    citi_destroy ();
    return -1;
  }

  if (!strcmp (action->out, "qucsdata")) {
    citi_result->setFile (outfile);
    qucsdata_producer (citi_result);
  }
  citi_destroy ();
  return 0;
}

// Touchstone to Qucs conversion.
int touch2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  touchstone_init ();
  if ((touchstone_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (touchstone_parse () != 0) {
    ret = -1;
  } else if (touchstone_check () != 0) {
    ret = -1;
  }
  touchstone_lex_destroy ();
  if (touchstone_in)
    fclose (touchstone_in);
  if (ret) {
    touchstone_destroy ();
    return -1;
  }

  if (!strcmp (action->out, "qucsdata")) {
    touchstone_result->setFile (outfile);
    qucsdata_producer (touchstone_result);
  }
  touchstone_destroy ();
  return 0;
}

// CSV to Qucs conversion.
int csv2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  csv_init ();
  if ((csv_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (csv_parse () != 0) {
    ret = -1;
  } else if (csv_check () != 0) {
    ret = -1;
  }
  csv_lex_destroy ();
  if (csv_in)
    fclose (csv_in);
  if (ret) {
    csv_destroy ();
    return -1;
  }

  if (!strcmp (action->out, "qucsdata")) {
    csv_result->setFile (outfile);
    qucsdata_producer (csv_result);
  }
  csv_destroy ();
  return 0;
}

// ZVR to Qucs conversion.
int zvr2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  zvr_init ();
  if ((zvr_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (zvr_parse () != 0) {
    ret = -1;
  } else if (zvr_check () != 0) {
    ret = -1;
  }
  zvr_lex_destroy ();
  if (zvr_in)
    fclose (zvr_in);
  if (ret) {
    zvr_destroy ();
    return -1;
  }
  if (!strcmp (action->out, "qucsdata")) {
    zvr_result->setFile (outfile);
    qucsdata_producer (zvr_result);
  }
  zvr_destroy ();
  return 0;
}

// MDL to Qucs conversion.
int mdl2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  mdl_init ();
  if ((mdl_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (mdl_parse () != 0) {
    ret = -1;
  } else if (mdl_check () != 0) {
    ret = -1;
  }
  mdl_lex_destroy ();
  if (mdl_in)
    fclose (mdl_in);
  if (ret) {
    mdl_destroy ();
    return -1;
  }
  if (!strcmp (action->out, "qucsdata")) {
    mdl_result->setFile (outfile);
    qucsdata_producer (mdl_result);
  }
  mdl_destroy ();
  return 0;
}

