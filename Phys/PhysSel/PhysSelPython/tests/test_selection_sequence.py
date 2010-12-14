#!/usr/bin/env python
#$Id: test_selection_sequence.py,v 1.22 2010-05-26 14:31:29 jpalac Exp $
'''
Test suite for SelectionSequence class.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

import sys
from py.test import raises

sys.path.append('../python')

from PhysSelPython.Wrappers import ( Selection,
                                     SelectionSequence,
                                     AutomaticData,
                                     NameError         )
from SelPy.configurabloids import ( DummyAlgorithm,
                                    DummySequencer  )

from Configurables import FilterDesktop

class SelectionTree(object) :
    sel000 = Selection('0.00000', Algorithm = DummyAlgorithm('Alg0.00000'),
                       RequiredSelections = [])
    sel001 = Selection('0.00001', Algorithm = DummyAlgorithm('Alg0.00001'),
                       RequiredSelections = [])
    sel002 = Selection('0.00002', Algorithm = DummyAlgorithm('Alg0.00002'),
                       RequiredSelections = [])
    sel003 = Selection('0.00003', Algorithm = DummyAlgorithm('Alg0.00003'),
                       RequiredSelections = [])
    
    sel100 = Selection('1.00000', Algorithm = DummyAlgorithm('Alg1.00000'),
                       RequiredSelections = [sel000, sel001])
    
    sel101 = Selection('1.00001', Algorithm = DummyAlgorithm('Alg1.00001'),
                       RequiredSelections = [sel002, sel003])
    
    sel200 = Selection('2.00000', Algorithm = DummyAlgorithm('Alg2.00000'),
                       RequiredSelections = [sel100, sel101])

    selSeq = SelectionSequence('Seq06', TopSelection = sel200)
    algs = selSeq.algos
    alg_names = [a.name() for a in algs]

def test_instantiate_sequencer() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    alg0 = DummyAlgorithm('Alg000')
    sel = Selection('00011', Algorithm = alg0,
                    RequiredSelections = [sel00, sel01])
    seq = SelectionSequence('Seq00', TopSelection = sel)

def test_instantiate_dataondemand_sequencer() :
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    seq = SelectionSequence('Seq00DOD', TopSelection = sel00)
    assert seq.outputLocation() == 'Phys/Sel00/Particles'
    
def test_sequencer_algos() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')

    sel01 = Selection('00100', Algorithm = DummyAlgorithm('Alg000'),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('00101', Algorithm = DummyAlgorithm('Alg001'),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('00102', Algorithm = DummyAlgorithm('Alg003'),
                      RequiredSelections = [ sel01, sel02])
    seq = SelectionSequence('Seq01',
                            TopSelection = sel03)

    seqAlgos = seq.algos

    assert len(seqAlgos) == 7
    for sel in [sel01, sel02, sel03] :
        assert sel.algorithm() in seqAlgos

def test_sequencer_sequence() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')

    sel01 = Selection('000110', Algorithm = DummyAlgorithm('Alg000'),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('000111', Algorithm = DummyAlgorithm('Alg001'),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('000112', Algorithm = DummyAlgorithm('Alg003'),
                      RequiredSelections = [ sel01, sel02])

    presel0 = DummyAlgorithm('Presel0')
    presel1 = DummyAlgorithm('Presel1')
    postsel0 = DummyAlgorithm('Postsel0')
    postsel1 = DummyAlgorithm('Postsel1')

    presels =  [presel0, presel1]
    postsels = [postsel0, postsel1]
    seq = SelectionSequence('Seq0002',
                            TopSelection = sel03,
                            EventPreSelector = presels,
                            PostSelectionAlgs = postsels)

    seqAlgos = seq.sequence(sequencerType=DummySequencer).Members

    ref_algos = [presel0,
                 presel1,
                 _sel00.algorithm(),
                 _sel01.algorithm(),
                 _sel02.algorithm(),
                 _sel03.algorithm(),
                 sel02.algorithm(),
                 sel01.algorithm(),
                 sel03.algorithm(),
                 postsel0,
                 postsel1]

    assert len(seqAlgos) == 11

    assert presels == ref_algos[:len(presels)]

    assert postsels == ref_algos[len(ref_algos)-len(postsels):]

    for sel in [sel01, sel02, sel03]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

def test_clone_sequence() :
    _sel00 = AutomaticData(Location = 'Phys/Sel00')
    _sel01 = AutomaticData(Location = 'Phys/Sel01')
    _sel02 = AutomaticData(Location = 'Phys/Sel02')
    _sel03 = AutomaticData(Location = 'Phys/Sel03')

    sel01 = Selection('00120', Algorithm = DummyAlgorithm('Alg000'),
                      RequiredSelections = [_sel00, _sel01])
    sel02 = Selection('00121', Algorithm = DummyAlgorithm('Alg001'),
                      RequiredSelections = [_sel02, _sel03])
    sel03 = Selection('00122', Algorithm = DummyAlgorithm('Alg003'),
                      RequiredSelections = [ sel01, sel02])

    presel0 = DummyAlgorithm('Presel00')
    presel1 = DummyAlgorithm('Presel01')
    postsel0 = DummyAlgorithm('Postsel00')
    postsel1 = DummyAlgorithm('Postsel01')

    presels =  [presel0, presel1]
    postsels = [postsel0, postsel1]

    seq = SelectionSequence('Seq03',
                            TopSelection = sel03,
                            EventPreSelector = presels,
                            PostSelectionAlgs = postsels)
    
    clone = seq.clone('clone')

    seqAlgos = clone.sequence(sequencerType=DummySequencer).Members

    ref_algos = [presel0,
                 presel1,
                 _sel00.algorithm(),
                 _sel01.algorithm(),
                 _sel02.algorithm(),
                 _sel03.algorithm(),
                 sel02.algorithm(),
                 sel01.algorithm(),
                 sel03.algorithm(),
                 postsel0,
                 postsel1]

    assert len(seqAlgos) == len(ref_algos)
    assert presels == seqAlgos[:len(presels)]
    assert postsels == seqAlgos[len(ref_algos)-len(postsels):]
    # sel03 must come just before the post selection algos.
    assert sel03.algorithm() == ref_algos[len(ref_algos)-(len(presels)+1)]
    # order of sel01 and sel02 doesn't matter.
    for sel in [sel01, sel02]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

    seqAlgos = seq.algos

    assert len(seqAlgos) == len(ref_algos)
    assert presels == seqAlgos[:len(presels)]
    assert postsels == seqAlgos[len(ref_algos)-len(postsels):]

    for sel in [sel01, sel02, sel03]:
        assert sel.algorithm() in ref_algos[len(presels):len(ref_algos)-len(postsels)]

def test_order_line() :

    
    sel000 = Selection('0.0000', Algorithm = DummyAlgorithm('Alg0.0000'),
                      RequiredSelections = [])
    sel001 = Selection('0.0001', Algorithm = DummyAlgorithm('Alg0.0001'),
                      RequiredSelections = [sel000])
    sel101 = Selection('1.0000', Algorithm = DummyAlgorithm('Alg1.0000'),
                      RequiredSelections = [sel001])
    sel102 = Selection('1.0001', Algorithm = DummyAlgorithm('Alg1.0001'),
                      RequiredSelections = [sel101])
    sel203 = Selection('2.0000', Algorithm = DummyAlgorithm('Alg2.000'),
                      RequiredSelections = [sel102])

    selSeq = SelectionSequence('Seq04', TopSelection = sel203)
    algs = selSeq.algos
    assert [a.name() for a in algs] == ['0.0000', '0.0001', '1.0000', '1.0001', '2.0000']

def test_order_tree() :

    alg_names = SelectionTree.alg_names
    
    assert alg_names.index('0.00000') < alg_names.index('1.00000')
    assert alg_names.index('0.00001') < alg_names.index('1.00000')
    assert alg_names.index('0.00002') < alg_names.index('1.00001')
    assert alg_names.index('0.00003') < alg_names.index('1.00001')
    assert alg_names.index('1.00000') < alg_names.index('2.00000')
    assert alg_names.index('1.00001') < alg_names.index('2.00000')


def test_remove_duplicates() :
    
    sel000 = SelectionTree.sel000
    sel001 = SelectionTree.sel001
    sel002 = SelectionTree.sel002
    sel003 = SelectionTree.sel003

    sel100 = Selection('1.10000', Algorithm = DummyAlgorithm('Alg1.10000'),
                       RequiredSelections = [sel000, sel001, sel002])
    
    sel101 = Selection('1.10001', Algorithm = DummyAlgorithm('Alg1.10001'),
                       RequiredSelections = [sel001, sel002, sel003])
    
    sel200 = Selection('2.10000', Algorithm = DummyAlgorithm('Alg2.10000'),
                       RequiredSelections = [sel100, sel101])

    selSeq =  SelectionSequence('Seq07', TopSelection = sel200)
    algs = selSeq.algos
    alg_names = [a.name() for a in algs]

    assert len(algs) == 7
    assert alg_names.count('0.00000') == 1
    assert alg_names.count('0.00001') == 1
    assert alg_names.count('0.00002') == 1
    assert alg_names.count('0.00003') == 1
    assert alg_names.count('1.10000') == 1
    assert alg_names.count('1.10001') == 1
    assert alg_names.count('2.10000') == 1
    
    assert alg_names.index('0.00000') < alg_names.index('1.10000')
    assert alg_names.index('0.00001') < alg_names.index('1.10000')
    assert alg_names.index('0.00002') < alg_names.index('1.10000')
    assert alg_names.index('0.00001') < alg_names.index('1.10001')
    assert alg_names.index('0.00002') < alg_names.index('1.10001')
    assert alg_names.index('0.00003') < alg_names.index('1.10001')
    assert alg_names.index('1.10000') < alg_names.index('2.10000')
    assert alg_names.index('1.10001') < alg_names.index('2.10000')

def test_selectionsequence_with_existing_configurable_name_raises() :

    fd = FilterDesktop('SelSeq00')
    
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    raises(NameError, SelectionSequence, 'SelSeq00', TopSelection = sel00 )

if '__main__' == __name__ :

    import sys

    test_names = filter(lambda k : k.count('test_') > 0, locals().keys())

    __tests = filter( lambda x : x[0] in test_names, locals().items())
    

    message = ''
    summary = '\n'
    length = len(sorted(test_names,
                        cmp = lambda x,y : cmp(len(y),len(x)))[0]) +2
    
    for test in __tests :
        try :
            test[1]()
            message = 'PASS'
        except :
            message = "FAIL"
        summary += test[0].ljust(length) + ':' + message.rjust(10) + '\n'

    if summary.count('FAIL') > 0 :
        message = 'FAIL'
        wr = sys.stderr.write
    else :
        message = 'PASS'
        wr = sys.stdout.write

    summary += 'Global'.ljust(length) + ':' + message.rjust(10) + '\n\n'
    wr(summary)
        
