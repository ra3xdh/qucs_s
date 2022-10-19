% taken from Qucs-s docs
clear;
Data="testTwoStageBJT.dat.xyce"; qdset=loadQucsDataSet(Data);

%extract transient sim data
[time]=getQucsVariable(qdset,"TIME");
[tran_ipr1]  = getQucsVariable(qdset,"tran.I(PR1)");
[tran_vpr2]  = getQucsVariable(qdset,"tran.V(PR2)");
[tran_vnin]  = getQucsVariable(qdset,"tran.V(NIN)");
[tran_vnout] = getQucsVariable(qdset,"tran.V(NOUT)");
%
% Extract HB data
[hbfrequency]  = getQucsVariable(qdset, "hbfrequency");
[pr1_ib]   = getQucsVariable(qdset, "PR1.Ib");
[pr2_vb]   = getQucsVariable(qdset, "PR2.Vb");
[nin_vb]   = getQucsVariable(qdset, "NIN.Vb");
[nout_vb]  = getQucsVariable(qdset, "NOUT.Vb");
%
showQucsDataSet(qdset);
figure(1);
Xstart=0; Xstop=0.001;
plotCartesian2D2V("plot",time,"Time(s)",Xstart,Xstop,
                  tran_vnin, "V(nin)(V)", "blue",
                  tran_vnout, "V(nout)(V)", "blue", 3);
%
figure(2);
plotCartesian2D2V("plot",time,"Time(s)",Xstart,Xstop,
                  tran_ipr1, "I(PR1)(A)", "blue",
                  tran_vpr2, "V(PR2)(V)", "blue", 3);
%
figure(3);
plotFD2V("Stem",hbfrequency,"Frequency(Hz)",
          abs(nin_vb),"V(nin)(V)","blue",
          abs(nout_vb),"V(nout)(V)","blue",3);
%
figure(4);
plotFD2V("Stem",hbfrequency,"Frequency(Hz)",
          abs(pr1_ib),"I(PR1)(A)","blue",
          abs(pr2_vb),"V(PR2)(V)","blue",3);
