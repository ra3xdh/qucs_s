"""
Module to handle a Qucs Schematic file.
"""

import os

def get_sch_version(schematic):
    '''
    :return: version of qucs that created the schematic.
    '''
    with open(schematic, 'r', encoding='iso-8859-1') as fp:
        for line in fp:
            if 'Qucs Schematic' in line:
                qucs_version = line.split(' ')[-1][:-2]
                return qucs_version


def get_sch_dataset(schematic):
    '''
    :return: DataSet fiel, name used to save the data file.
    '''
    with open(schematic, 'r', encoding='iso-8859-1') as fp:
        for line in fp:
            if 'DataSet' in line:
                ref_dataset = line.split('=')[-1][:-2]
                return ref_dataset


def get_sch_simulations(sch):
    '''
    :return: list of simulations found on the schematic.
    '''
    # supported simulations
    sim_types = ['.DC', '.AC', '.TR', '.SP', '.SW']

    sim_used = []
    schematic = open(sch).read()
    for sim in sim_types:
        if sim in schematic:
            sim_used.append(sim)
    return sim_used


def get_sch_subcircuits(sch):
    '''
    Return list of subcircuit files. Search recursively for all sub-circuit files.

    :param sch: input schematic
    :return: list of sub-circuits referenced from top schematics
    '''

    # use a crawler to figure out all the sub-circuits necessary
    # Similar: http://stackoverflow.com/questions/13658863/simple-web-crawler
    def union(p,q):
        for e in q:
            if e not in p:
                p.append(e)

    def crawl(sch):
        sub_files = []
        print(sch)
        with open(sch) as fp:
            for line in fp:
                # <Sub Curtice1 1 210 120 20 -46 0 0 "curtice_1.sch" [other params] >
                if '<Sub ' in line:
                    # subcircuit filename, no quotes
                    sub_file = line.strip().split(' ')[9][1:-1]
                    sub_file = os.path.join(os.path.dirname(sch), sub_file)
                    sub_files.append(sub_file)
        return sub_files

    def crawler(sch):
        tocrawl=[sch]
        crawled=[]
        while tocrawl:
            page=tocrawl.pop()
            subs=crawl(page)
            if page not in crawled:
                union(tocrawl,subs)
                crawled.append(page)
        return crawled

    return crawler(sch)[1:] # skip seed sch

