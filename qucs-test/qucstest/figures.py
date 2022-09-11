"""
Module to handle plots and figures.
"""
import os
import platform
import matplotlib.pyplot as plt
from matplotlib.ticker import OldScalarFormatter

from mpl_toolkits.mplot3d.axes3d import Axes3D
from matplotlib.ticker import EngFormatter

from .misc import timestamp
from .qucsdata import QucsData


def plot_error(reference, test, variables, save=True, show=False):
    """
    Plot the traces that failed numerical check along with the error.

    :param reference: reference dataset.
    :param test: test dataset.
    :param variables: list of traces that will be plotted.
    :param save: save image.
    :param show: show image on interactive window.
    """
    gold  = QucsData(reference)
    check = QucsData(test)
    for label in variables:
        a =  gold.data[label]
        b = check.data[label]

        f, axarr = plt.subplots(2, sharex=True)
        ax1 = axarr[0]
        ax2 = axarr[1]

        ax1.set_title('[%s vs %s] [%s]'
                     %(gold.version, check.version,
                       platform.platform()) )

        ax1.plot(a, 'b-+',  markersize=10, label='gold:'+label)
        ax1.plot(b, 'g--x', markersize=10, label='test:'+label)
        ax1.set_ylabel(label)
        ax1.grid(True)
        ax1.legend(loc=2, fancybox=True, framealpha=0.5)


        ax2.plot(abs(a-b),   'r.-', label='abs(g-t)')
        ax2.set_ylabel('abs(gold - test)', color='r')
        ax2.set_xlabel('step')
        ax2.grid(True)
        ax3 = ax2.twinx()
        ax3.plot(abs(a-b)/a, 'm.-', label='abs(g-t)/g')
        ax3.set_ylabel('abs(gold - test)/gold', color='m')
        for tl in ax2.get_yticklabels():
            tl.set_color('r')
        for tl in ax3.get_yticklabels():
            tl.set_color('m')
        ax1.yaxis.set_major_formatter(OldScalarFormatter())
        ax2.yaxis.set_major_formatter(OldScalarFormatter())
        ax3.yaxis.set_major_formatter(OldScalarFormatter())

        h1, l1 = ax2.get_legend_handles_labels()
        h2, l2 = ax3.get_legend_handles_labels()
        ax2.legend(h1+h2, l1+l2, loc=2, fancybox=True, framealpha=0.5)
        plt.tight_layout()

        if show:
            plt.show()

        if save:
            proj_dir = os.path.dirname(reference)
            figname = '%s_%s_%s.png' %(os.path.basename(proj_dir), platform.system(), label)
            figname = os.path.join(proj_dir, figname)
            print('saving', figname)
            plt.savefig(figname)

        plt.close()


def plot_variable(dataset, variable=''):
    """
    Plot one or all dependent variables from a Qucs Dataset

    :param dataset: Qucs Dataset
    :param variable"
    """
    d = QucsData(dataset)

    # plot single independent or dependent variable
    if variable:
        n = variable
        fig, ax = plt.subplots(figsize=(8,6))
        ax.plot(d.data[n] )
        ax.set_xlabel('step')
        ax.set_ylabel(n)
        ax.grid(True)
        plt.show()
        return

    # plot all dependent variables
    for n in list(d.dependent.keys()):
      dep = len(d.dependent[n])
      if dep == 1:
          a =  d.dependent[n][0]
          fig, ax = plt.subplots(figsize=(8,6))
          x = d.data[a]
          y = d.data[n]
          ax.plot(x, y)
          ax.set_xlabel(a)
          ax.set_ylabel(n)
          ax.grid(True)
          ax.set_xlim((min(x), max(x)))
          formatter = EngFormatter(places=1)
          ax.xaxis.set_major_formatter(formatter)
          plt.show()

      if dep == 2:
          a,b = d.dependent[n]
          x = d.data[a]
          y = d.data[b]
          z = d.data[n]

          xs, ys = np.meshgrid(x, y)

          fig = plt.figure()
          ax = Axes3D(fig)
          ax.view_init(azim=-120)
          ax.plot_wireframe(xs, ys, z.transpose())
          ax.set_xlabel(a)
          ax.set_ylabel(b)
          ax.set_zlabel(n)
          plt.show()



if __name__ == '__main__':

  # simple demo

  gold = '../testsuite/TR_sawtooth-discreet_prj/sawtooth-discreet.dat'
  test = '../testsuite/TR_sawtooth-discreet_prj/test_sawtooth-discreet.dat'

  dir = os.path.dirname(__file__)
  gold = os.path.join(dir, gold)
  test = os.path.join(dir, test)

  import sys
  sys.path.append("..")
  from run import compare_datasets

  numerical_diff = compare_datasets(gold, test)
  print(numerical_diff)

  plot_error(gold, test, numerical_diff)

  plot_variable(gold)
  plot_variable(gold,'time')


