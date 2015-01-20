// Main interface control

/*

# include "qf_common.h"
# include "qf_poly.h"
# include "qf_comp.h"
# include "qf_filter.h"
# include "qf_cauer.h"
# include "qf_zigzag.h"
# include "qf_icheb.h"
# include "qf_bessel.h"
# include "qf_legendre.h"

void  ask_optim (bool & opt_ok, struct qf_optim * opt) {
  char c;

  while (true) {
    std::cout << "Optimize? [y/n] ";
    std::cin >> c;
    c = tolower (c);

    if (c == 'n') {
      opt_ok = false;
      return;
    }

    if (c == 'y') break;
  }

  opt_ok = true;

  while (true) {
    std::cout << "Optimize c(min)? [y/n] ";
    std::cin >> c;
    c = tolower (c);

    if (c == 'y') {
      opt -> do_cmin = true;
      std::cout << "c(min)? ";
      std::cin >> opt -> cmin;
      break;
    }
    else if (c == 'n') {
      opt -> do_cmin = false;
      break;
    }
  }

  while (true) {
    std::cout << "Optimize c(max)? [y/n] ";
    std::cin >> c;
    c = tolower (c);

    if (c == 'y') {
      opt -> do_cmax = true;
      std::cout << "c(max)? ";
      std::cin >> opt -> cmax;
      break;
    }
    else if (c == 'n') {
      opt -> do_cmax = false;
      break;
    }
  }

  while (true) {
    std::cout << "Optimize l(min)? [y/n] ";
    std::cin >> c;
    c = tolower (c);

    if (c == 'y') {
      opt -> do_lmin = true;
      std::cout << "l(min)? ";
      std::cin >> opt -> lmin;
      break;
    }
    else if (c == 'n') {
      opt -> do_lmin = false;
      break;
    }
  }

  while (true) {
    std::cout << "Optimize l(max)? [y/n] ";
    std::cin >> c;
    c = tolower (c);

    if (c == 'y') {
      opt -> do_lmax = true;
      std::cout << "l(max)? ";
      std::cin >> opt -> lmax;
      break;
    }
    else if (c == 'n') {
      opt -> do_lmax = false;
      break;
    }
  }
  return;
}

int main (void) {

  while (true) {
  qf_double_t	  amin, amax, f, fs, bw, r1, r2;
  char		  c;
  bool		  dual;
  bool		  pole;
  bool		  optf;
  struct qf_optim opts;

    std::cout << 
      "Choose filter type (c/C, h/H, l/L, p/P, s/S, z, i, b, g) q to quit: ";
    std::cin >> c ;

    dual = (c <= 'Z');
  
    if (dual) c = c - 'A' + 'a';

    switch (c)
    {
      case 'i': {
	std::cout << "Cutoff (Hz): ";
	std::cin >> f;
	std::cout << "Ripple (dB): ";
	std::cin >> amin;
	std::cout << "Stopband corner (Hz): ";
	std::cin >> fs;
	std::cout << "Stopband attenuation (dB): ";
	std::cin >> amax;
	std::cout << "Termination (Ohms): ";
	std::cin >> r1;

	qf_icheb (f, fs, amin, amax, r1, 0, LOWPASS);
//	qf_icheb (1, fs, amin, amax, r1, 0, LOWPASS);
	break;}

      case 'l': {
	std::cout << "Cutoff (Hz): ";
	std::cin >> f;
	std::cout << "Ripple (dB): ";
	std::cin >> amin;

	while (true) {
	  std::cout << "Pole form? [y/n] ";
	  std::cin >> c;
	  if (c < 'Z') c = c - 'A' + 'a';
	  if (c == 'y') {pole = true; break;}
	  if (c == 'n') {pole = false; break;}
	}

	std::cout << "Pole/Stopband corner (Hz): ";
	std::cin >> fs;
	std::cout << "Stopband attenuation (dB): ";
	std::cin >> amax;
	std::cout << "Termination 1 (Ohms): ";
	std::cin >> r1;
	std::cout << "Termination 2 (Ohms): ";
	std::cin >> r2;

	ask_optim (optf, & opts);

	qf_cauer F(amin, amax, f, fs, r1, r2, 0, LOWPASS, dual, pole,
		  optf, & opts);
	F. dump_cout ();
	break;}

      case 'h': {
	std::cout << "Cutoff (Hz): ";
	std::cin >> f;
	std::cout << "Ripple (dB): ";
	std::cin >> amin;
	
	while (true) {
	  std::cout << "Pole form? [y/n] ";
	  std::cin >> c;
	  if (c < 'Z') c = c - 'A' + 'a';
	  if (c == 'y') {pole = true; break;}
	  if (c == 'n') {pole = false; break;}
	}

	std::cout << "Pole/Stopband corner (Hz): ";
	std::cin >> fs;
	std::cout << "Stopband attenuation (dB): ";
	std::cin >> amax;
	std::cout << "Termination 1 (Ohms): ";
	std::cin >> r1;
	std::cout << "Termination 2 (Ohms): ";
	std::cin >> r2;

	ask_optim (optf, & opts);

	qf_cauer F(amin, amax, f, fs, r1, r2, 0, HIGHPASS, dual, pole,
		  optf, & opts);
	F. dump_cout ();
	break;}

      case 'p': {
	std::cout << "Center (Hz): ";
	std::cin >> f;
	std::cout << "Bandwidth (Hz): ";
	std::cin >> bw;
	std::cout << "Passband ripple (dB): ";
	std::cin >> amin;
	
	while (true) {
	  std::cout << "Pole form? [y/n] ";
	  std::cin >> c;
	  if (c < 'Z') c = c - 'A' + 'a';
	  if (c == 'y') {pole = true; break;}
	  if (c == 'n') {pole = false; break;}
	}

	std::cout << "Pole/Stopband corner (Hz): ";
	std::cin >> fs;
	std::cout << "Stopband attenuation (dB): ";
	std::cin >> amax;
	std::cout << "Termination 1 (Ohms): ";
	std::cin >> r1;
	std::cout << "Termination 2 (Ohms): ";
	std::cin >> r2;

	ask_optim (optf, & opts);

	qf_cauer F(amin, amax, f, fs, r1, r2, bw, BANDPASS, dual, pole,
		  optf, & opts);
	F. dump_cout ();
	break;}

      case 's': {
	std::cout << "Center (Hz): ";
	std::cin >> f;
	std::cout << "Bandwidth (Hz): ";
	std::cin >> bw;
	std::cout << "Stopband attenuation (dB): ";
	std::cin >> amax;
	
	while (true) {
	  std::cout << "Pole form? [y/n] ";
	  std::cin >> c;
	  if (c < 'Z') c = c - 'A' + 'a';
	  if (c == 'y') {pole = true; break;}
	  if (c == 'n') {pole = false; break;}
	}

	std::cout << "Pole/Passband corner (Hz): ";
	std::cin >> fs;
	std::cout << "Passband attenuation (dB): ";
	std::cin >> amin;
	std::cout << "Termination 1 (Ohms): ";
	std::cin >> r1;
	std::cout << "Termination 2 (Ohms): ";
	std::cin >> r2;

	ask_optim (optf, & opts);

	qf_cauer F(amin, amax, f, fs, r1, r2, bw, BANDSTOP, dual, pole,
		  optf, & opts);
	F. dump_cout ();
	break;}

      case 'z' : {
	std::cout << "Center (Hz): ";
	std::cin >> f;
	std::cout << "Bandwidth (Hz): ";
	std::cin >> bw;
	std::cout << "Passband ripple (dB): ";
	std::cin >> amin;
	std::cout << "Stopband corner (Hz): ";
	std::cin >> fs;
	std::cout << "Stopband attenuation (dB): ";
	std::cin >> amax;
	std::cout << "Termination (Ohms): ";
	std::cin >> r1;

	ask_optim (optf, & opts);

	qf_zigzag F (amin, amax, f, fs, r1, bw, optf, & opts);
	break;}

      case 'c' : {
	unsigned o;

	std::cout << "Order: ";
	std::cin >> o ;
	  if (o == 0)
	    break ;

	std::cout << "Rho (%): ";
	std::cin >> amin;
	amin /= 100;

	std::cout << "Theta (deg): ";
	std::cin >> fs;
	fs = pi * fs / 180;

	if (o % 2 == 0)
	{
	  while (true) {
	    std::cout << "Type (b/c): ";
	    std::cin >> c;
	    if ((c == 'b') || (c == 'c'))
	      break;
	  }
	}

	qf_cauer (o, amin, fs, c, dual);
	break;}

      case 'q':
	return 0;

      case 'b': {
	unsigned n;

	std::cout << "Order: ";
	std::cin >> n;

	std::cout << "Cutoff (Hz): ";
	std::cin >> f;

	std::cout << "Termination (Ohms): ";
	std::cin >> r1;
	
	qf_bessel (n, r1, f);
	break;}

      case 'g': {
	unsigned n;

	std::cout << "Order: ";
	std::cin >> n;

	std::cout << "Cutoff (Hz): ";
	std::cin >> f;

	std::cout << "Termination (Ohms): ";
	std::cin >> r1;

	qf_lgndr (n, r1, f);
	break;}
    
      default: ;
    }
  }
}
*/
