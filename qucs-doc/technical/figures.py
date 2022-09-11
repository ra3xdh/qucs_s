#
# Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
# Copyright (C) 2020 Guilherme Brondani Torri <guitorri@gmail.com>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.
#

'''
Generate figures used on the Technical Papers document.
'''

import numpy as np
import matplotlib.pyplot as plt
from numpy import log, power, pi, exp, sqrt

# constants
Z0 = 376.73031346958504364963

def fig_radius(show=False):
    '''Chapter 3
    Fig curve radius of exponential diode curve and its derivative.
    '''
    Is = 1e-13
    Ut = 25e-3

    u  = np.linspace(0.6,0.7,100)
    i0 = Is * (exp(u/Ut) - 1)
    i1 = Is/Ut * exp(u/Ut)
    i2 = Is/Ut/Ut * exp(u/Ut)
    i3 = Is/Ut/Ut/Ut * exp(u/Ut)

    z0 = np.linspace(0,0,100)

    # eq [3.50]
    r0 = power(1 + power(i1, 2), 3/2) / i2

    # eq [3.51]
    r1 = 3/2*power( 1+power(i1, 2), 1/2 )*2*i1 / i2
    r1 = r1 +  power( 1+power(i1, 2), 3/2 ) / power(i2, 2)
    r1 = i2 * 3/2*power( 1+power(i1, 2), 1/2 )*2*i1*i2 - power(1+power(i1, 2), 3/2) * i3
    r1 = r1 / power(i2, 2)
    r1 = r1/20

    plt.figure()
    plt.plot(u, r0, "r-", label = "curve radius", linewidth = 3)
    plt.plot(u, r1, "g--", label = "derivative of curve radius", linewidth = 3)
    plt.plot(u, z0, "b", linewidth = 3)
    plt.xlim(0.6, 0.7)
    plt.ylim(-0.4, 1)
    plt.legend()
    plt.grid(True, which="both", color="k", linestyle="--", linewidth=0.5)
    plt.tight_layout()
    if show:
        plt.show()
    name = "radius.pdf"
    print('Save', name)
    plt.savefig(name)


def zpoly (b, a, z):
    '''The function computes the region of absolute stability based upon the
    coefficients 'a' and 'b' characterizing an integration method.  The
    given complex vector 'z' should be the unity circle.
    '''
    # go through complex vectorz
    area = np.zeros(len(z), dtype=complex)
    for k in range(len(z)):
      bsum = 0
      asum = 0
      # sum denominator polynomial
      for i in range(len(b)):
        bsum = bsum + b[i] * power(z[k], -i)
      # sum numerator polynomial
      for i in range(len(a)):
        asum = asum + a[i] * power(z[k], -i)
      if (bsum == 0):
        bsum = 1
      area[k] = - asum / bsum
    return(area)

def econst(b, a):
    '''The function computes the error constants, order and steps for the
    given linear multistep integration method characterized by the 'a'
    and 'b' coefficients.
    '''
    constants = np.zeros(len(a)+2)
    steps = 0
    for i in range(len(a)): # determine number of steps
      if (b[i] != 0 or a[i] != 0):
        steps += 1
    p = 0
    for i in range(len(a)): # determine highest coefficient index
      if (b[len(a)-1-i] != 0 or
          a[len(a)-1-i] != 0):
        p = len(a)-1 - i;
        break;

    coeff = 0          # check order of method
    q = 0
    while (abs(coeff) <= 1e-12):
      coeff = 0
      if (q == 0):
        # special C0 constant
        for i in np.arange(-1, p):
          coeff = coeff - a[i+1]

      elif (q == 1):
        # special C1 constant
        n = p - 1
        for i in np.arange(-1, n+1):
          coeff = coeff - b[i+1] * (n - i)**(q-1)
        for i in np.arange(-1, n+1):
          coeff = coeff - a[i+1] * (n - i)**(q)
      else:
        # usual Cq coefficient
        f = 1
        for i in np.arange(1, q):
          f = f * i
        n = p - 1
        for i in np.arange(-1, n):
          coeff = coeff - 1 / f * b[i+1] * (n - i)**(q-1)
        f = f * q
        for i in np.arange(-1, n):
          coeff = coeff - 1 / f * a[i+1] * (n - i)**(q)
      constants[q] = coeff
      q += 1
    print("order %i, error constant %.4f" %(q-2, coeff))
    return constants[0:-1]


def fig_stabgears(show=False):
    '''Chapter 6 - Fig 6.2 Gears
    Areas of absolute stability for order 1 to 6
    '''
    fig = plt.figure()
    ax = fig.add_subplot(111,aspect='equal')

    p = np.linspace (0, 2*pi, 200)
    z = exp (1j * p)

    G1 = np.array([[1, 0],[-1, 1]]) / 1
    G2 = np.array([[2, 0, 0],[-3, 4, -1]]) / 3
    G3 = np.array([[6, 0, 0, 0],[-11, 18, -9, 2]]) / 11
    G4 = np.array([[12, 0, 0, 0, 0],[-25, 48, -36, 16, -3]]) / 25
    G5 = np.array([[60, 0, 0, 0, 0, 0],[-137, 300, -300, 200, -75, 12]]) / 137
    G6 = np.array([[60, 0, 0, 0, 0, 0, 0],[-147, 360, -450, 400, -225, 72, -10]]) / 1

    Polynomials = [G1, G2, G3, G4, G5, G6]
    for i, G in enumerate(Polynomials):
        area = zpoly (G[0,:], G[1,:], z)
        print(f"GEARS{i+1} error constants:")
        np.set_printoptions(precision=5)
        print(econst(G[0,:], G[1,:]))
        ax.plot(np.real(area), np.imag(area))

    # setup figure
    ax.set_xlabel('Re(lh)')
    ax.set_ylabel('Im(lh)')
    ax.set_xlim(-8, 8)
    ax.set_ylim(-5, 5)
    ax.set_aspect('equal')
    for line in ax.get_lines(): line.set_color("blue")
    ax.axhline(linewidth=2, color='k')
    ax.axvline(linewidth=2, color='k')

    from matplotlib.ticker import (MultipleLocator, FormatStrFormatter,
                                   AutoMinorLocator)

    ax.xaxis.set_ticks_position('both')
    ax.xaxis.set_major_locator(MultipleLocator(base=1))
    ax.xaxis.set_minor_locator(AutoMinorLocator())

    ax.xaxis.set_ticks_position('both')
    ax.yaxis.set_major_locator(MultipleLocator(base=1))
    ax.tick_params(which='minor', length=2, color='k')

    plt.grid(True, which="major", color="k", linestyle="--", linewidth=0.5)
    plt.tight_layout()
    if(show):
        plt.show()
    name = "stabgear.pdf"
    print('Save', name)
    plt.savefig(name)

def fib_stabgearex(show=False):
    '''Stability regions of explicit Gear formulae.
    Not yet icluded in the technical manual
    '''
    fig = plt.figure()
    ax = fig.add_subplot(111,aspect='equal')

    p = np.linspace (0, 2*pi, 200)
    z = exp (1j * p)

    G1 = np.array([[0, 0, 0], [-1, 2, -1]])
    G2 = np.array([[0, 0, 0, 0], [-1, 3, -3, 1]])
    G3 = np.array([[0, 0, 0, 0, 0], [-1, 4, -6, 4, -1]])
    G4 = np.array([[0, 0, 0, 0, 0, 0], [-1, 5, -10, 10, -5, 1]])
    G5 = np.array([[0, 0, 0, 0, 0, 0, 0], [-1, 6, -15, 20, -15, 6, -1]])
    G6 = np.array([[0, 0, 0, 0, 0, 0, 0, 0], [-1, 7, -21, 35, -35, 21, -7, 1]])

    Polynomials = [G1, G2, G3, G4, G5, G6]
    for i, G in enumerate(Polynomials):
        area = zpoly (G[0,:], G[1,:], z)
        print(f"GEARS{i+1} error constants:")
        np.set_printoptions(precision=5)
        print(econst(G[0,:], G[1,:]))
        ax.plot(np.real(area), np.imag(area))

    # setup figure
    ax.set_xlabel('Re(lh)')
    ax.set_ylabel('Im(lh)')
    ax.set_xlim(-8, 8)
    ax.set_ylim(-5, 5)
    ax.set_aspect('equal')
    for line in ax.get_lines(): line.set_color("blue")
    ax.axhline(linewidth=2, color='k')
    ax.axvline(linewidth=2, color='k')

    from matplotlib.ticker import (MultipleLocator, FormatStrFormatter,
                                   AutoMinorLocator)

    ax.xaxis.set_ticks_position('both')
    ax.xaxis.set_major_locator(MultipleLocator(base=1))
    ax.xaxis.set_minor_locator(AutoMinorLocator())

    ax.xaxis.set_ticks_position('both')
    ax.yaxis.set_major_locator(MultipleLocator(base=1))
    ax.tick_params(which='minor', length=2, color='k')

    plt.grid(True, which="major", color="k", linestyle="--", linewidth=0.5)
    plt.tight_layout()
    if(show):
        plt.show()
    name = "stabgearex.pdf"
    print('Save', name)
    plt.savefig(name)

def fig_stabmoulton(show=False):
    '''Chapter 6 - Fig 6.3 Adams-Moulton
    Areas of absolute stability for order 1 to 6
    '''
    fig = plt.figure()
    ax = fig.add_subplot(111,aspect='equal')

    p = np.linspace (0, 2*pi, 200)
    z = exp (1j * p)

    M1 = np.array([[1, 0],[-1, 1]]) / 1
    M2 = np.array([[1, 1], [-2, 2]]) / 2
    M3 = np.array([[5, 8, -1], [-12, 12, 0]]) / 12
    M4 = np.array([[9, 19, -5, 1], [-24, 24, 0, 0]]) / 24
    M5 = np.array([[251, 646, -264, 106, -19], [-720, 720, 0, 0, 0]]) / 720
    M6 = np.array([[475, 1427, -798, 482, -173, 27], [-1440, 1440, 0, 0, 0, 0]]) / 1440

    Polynomials = [M1, M2, M3, M4, M5, M6]
    for i, M in enumerate(Polynomials):
        area = zpoly (M[0,:], M[1,:], z)
        print(f"AM{i+1} error constants:")
        np.set_printoptions(precision=5)
        print(econst(M[0,:], M[1,:]))
        ax.plot(np.real(area), np.imag(area))

    # setup figure
    ax.set_xlabel('Re(lh)')
    ax.set_ylabel('Im(lh)')
    ax.set_xlim(-5.5, 4.6)
    ax.set_ylim(-3, 3)
    ax.set_aspect('equal')
    for line in ax.get_lines(): line.set_color("blue")
    ax.axhline(linewidth=2, color='k')
    ax.axvline(linewidth=2, color='k')

    from matplotlib.ticker import (MultipleLocator, FormatStrFormatter,
                                   AutoMinorLocator)

    ax.xaxis.set_ticks_position('both')
    ax.xaxis.set_major_locator(MultipleLocator(base=1))
    ax.xaxis.set_minor_locator(AutoMinorLocator())

    ax.xaxis.set_ticks_position('both')
    ax.yaxis.set_major_locator(MultipleLocator(base=1))
    ax.tick_params(which='minor', length=2, color='k')

    plt.grid(True, which="major", color="k", linestyle="--", linewidth=0.5)
    plt.tight_layout()
    if(show):
        plt.show()
    name = "stabmoulton.pdf"
    print('Save', name)
    plt.savefig(name)


def fig_stabbashford(show=False):
    '''Chapter 6 - Fig 6.4 Adams-Bashford
    Areas of absolute stability for order 1 to 6
    '''
    fig = plt.figure()

    ax = fig.add_subplot(111,aspect='equal')

    p = np.linspace (0, 2*pi, 200)
    z = exp (1j * p)

    B1 = np.array([[0, 1], [-1, 1]]) / 1
    B2 = np.array([[0, 3, -1], [-2, 2, 0]]) / 2
    B3 = np.array([[0, 23, -16, 5], [-12, 12, 0, 0]]) / 12
    B4 = np.array([[0, 55, -59, 37, -9], [-24, 24, 0, 0, 0]]) / 24
    B5 = np.array([[0, 1901, -2774, 2616, -1274, 251], [-720, 720, 0, 0, 0, 0]]) / 720
    B6 = np.array([[0, 4277, -7923, 9982, -7298, 2877, -475], [-1440, 1440, 0, 0, 0, 0, 0]]) / 1440

    Polynomials = [B1, B2, B3, B4, B5, B6]
    for i, B in enumerate(Polynomials):
        area = zpoly (B[0,:], B[1,:], z)
        print(f"AB{i+1} error constants:")
        np.set_printoptions(precision=5)
        print(econst(B[0,:], B[1,:]))
        ax.plot(np.real(area), np.imag(area))

    # setup figure
    ax.set_xlabel('Re(lh)')
    ax.set_ylabel('Im(lh)')
    ax.set_xlim(-2.75, 2.7)
    ax.set_ylim(-1.5,1.5)
    ax.set_aspect('equal')
    for line in ax.get_lines(): line.set_color("blue")
    ax.axhline(linewidth=2, color='k')
    ax.axvline(linewidth=2, color='k')

    from matplotlib.ticker import (MultipleLocator, FormatStrFormatter,
                                   AutoMinorLocator)

    ax.xaxis.set_ticks_position('both')
    ax.xaxis.set_major_locator(MultipleLocator(base=0.5))
    ax.xaxis.set_minor_locator(AutoMinorLocator())

    ax.xaxis.set_ticks_position('both')
    ax.yaxis.set_minor_locator(AutoMinorLocator())
    ax.tick_params(which='minor', length=2, color='k')

    plt.grid(True, which="major", color="k", linestyle="--", linewidth=0.5)
    plt.tight_layout()
    if(show):
        plt.show()
    name = "stabbashford.pdf"
    print('Save', name)
    plt.savefig(name, bbox_inches='tight')

def zl_hamm_jen(u, er):
    '''Chapter 11
    Hammerstad and Jensen impedance formula
    '''
    # eq [11.16]
    a = 1 + 1 / 49 * log ( (power(u, 4) + power(u/52, 2)) / (power(u, 4) + 0.432) )
    a = a + 1 / 18.7 * log (1 + power(u/18.1, 3))
    # eq [11.17]
    b = 0.564 * power((er - 0.9) / (er + 3), 0.053)
    # eq [11.15]
    g = (er + 1) / 2 + (er - 1) / 2 * power( 1 + 10 / u, (-a * b))
    # eq [11.6]
    f = 6 + (2 * pi - 6) * exp ( -power( 30.666/u, 0.7528) )
    # eq [11.5]
    d = Z0 / 2 / pi * log ( f / u + sqrt(1 + power( 2/u, 2)) )
    zl = d / sqrt (g)
    return(zl)

def zl_schneider(u, er):
    '''Chapter 11
    Schneider impedance formula
    '''
    zl = np.zeros(len(u))
    # eq [11.14]
    g = (er + 1) / 2 + (er - 1) / 2 * power(1 + 10/u, -0.5)
    # eq [11.3]
    for i in range(len(u)):
        if (u[i] <= 1) :
            z = Z0 / pi / 2 * log (8/u[i] + u[i]/4)
        else:
            z = Z0 / (u[i] + 2.42 - 0.44 / u[i] + power(1 - 1/u[i], 6))
        zl[i] = z / sqrt(g[i])
    return(zl)

def zl_wheeler(u, er):
    '''Chapter 11
    Wheeler impedance formula
    '''
    zl = np.zeros(len(u))
    for i in range(len(u)):
      if (u[i] <= 3.3) :
          # eq [11.2]
          c = log ( 4 / u[i] + sqrt( power(4 / u[i], 2) + 2) )
          z = c - 0.5 * (er - 1) / (er + 1) * ( log (pi/2) + log (4/pi) / er )
          z = z * Z0 / pi / sqrt (2 * (er + 1))
      else :
          # eq [11.1]
          c = u[i] / 2 + log (4) / pi
          c = c + (er + 1) / 2 / pi / er * log (pi/2 * np.e * (u[i] / 2 + 0.94))
          c = c + (er - 1) / 2 / pi / power(er, 2) * log (np.e * power(pi, 2) / 16)
          z = Z0 / 2 / sqrt (er) / c
          #z = z + 0.098456; % minimal correction
      zl[i] = z
    return(zl)


def fig_mszl(show=False):
    '''Chapter 11
    Characteristic impedance as approximated by Hammerstad and Jensen
    '''
    u = np.linspace(0.1, 10.1, 101)
    k = np.array([1, 2.1, 2.5, 3.78, 9.8, 12.9, 16])
    k = np.array([1, 3.78, 9.5])
    plt.figure()
    for n in range(len(k)):
        er = k[n]
        zl = zl_hamm_jen(u, er)
        plt.semilogx(u, zl, color="k", linewidth=3)

    plt.xlabel("normalised strip width W/h")
    plt.ylabel("impedance ZL in Ohm")
    plt.grid(True, which="both", color="k", linestyle="--", linewidth=0.5)
    plt.xlim(1e-1, 1e1)
    plt.ylim(0, 250)
    plt.tight_layout()
    if show:
        plt.show()
    name = "mszl.pdf"
    print('Save', name)
    plt.savefig(name)

def fig_mscomparezl(show=False):
    '''Chapter 11
    Comparizon of impedance formulas
    '''
    u = np.linspace(0.01, 5.01, 101)
    er = 9.8

    plt.figure()

    zl = zl_hamm_jen(u, er)
    zlref = zl
    lbl = "Hammerstad and Jensen"
    plt.plot(u, (zl - zlref)/zlref * 100, '.', label=lbl, color="g", linewidth=2)

    zl2 = zl_schneider(u, er)
    lbl="Schneider"
    plt.plot(u, (zl2 - zlref)/zlref * 100, '--', label=lbl, color="b", linewidth=2)

    zl3 = zl_wheeler(u, er)
    lbl="Wheeler"
    plt.plot(u, (zl3 - zlref)/zlref * 100, label=lbl , color="r", linewidth=2)

    plt.xlabel("normalised strip width W/h")
    plt.ylabel("deviation of impedance ZL [%]")
    plt.xlim(0, 5)
    plt.ylim(-2, 1.5)
    plt.legend()
    plt.grid(True, which="both", color="k", linestyle="--", linewidth=1)
    plt.tight_layout()
    if show:
        plt.show()
    name = "mscomparezl.pdf"
    print('Save', name)
    plt.savefig(name)

def Kapprox(x):
    '''The function computes the K/K' approximation in the argument
    interval [0;1].  The interval is splitted at 1/sqrt(2).
    '''
    k = np.zeros(len(x))
    for n in range(len(x)):
        if (x[n] < 1 / sqrt(2)):
            a = sqrt (1 - x[n]**2)
            k[n] = pi / (log (2 * (1 + sqrt(a)) / (1 - sqrt(a))))
        else:
            a = x[n]
            k[n] = (log (2 * (1 + sqrt(a)) / (1 - sqrt(a)))) / pi
    return(k)

def fig_coplanarzl(show=False):
    u = np.linspace(0.1,10.1,101)
    k = np.array([1, 2.1, 2.5, 3.78, 9.8, 12.9, 16])
    k = np.array([1, 3.78, 9.5])

    plt.figure()
    for n in range(len(k)):
        er = (k[n] + 1) / 2
        a  = 1 / (1 + 2 / u)
        d  = Z0 / 4 / Kapprox(a)
        zl = d / sqrt (er)
        plt.semilogx(u, zl, color="k", linewidth=3)

    plt.xlabel("normalised strip width W/h")
    plt.ylabel("impedance ZL in Ohm")
    plt.grid(True, which="both", color="k", linestyle="--", linewidth=0.5)
    plt.xlim(1e-1, 1e1)
    plt.ylim(0, 250)
    plt.tight_layout()
    if show:
        plt.show()
    name = "coplanarzl.pdf"
    print('Save', name)
    plt.savefig(name)


# Generate and figures
if __name__ == "__main__":
  show = False

  # Chapter 3
  fig_radius(show)
  # Chapter 6
  fig_stabgears(show)
  fib_stabgearex(show)# not yet used in technical.pdf
  fig_stabmoulton(show)
  fig_stabbashford(show)
  # Chapter 11
  fig_mszl(show)
  fig_mscomparezl(show)
  # Chapter 12
  fig_coplanarzl(show)

