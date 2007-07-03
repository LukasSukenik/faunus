#include "widommod.h"

Widommod::Widommod(int gin, int in){
  cnt=0;
  ghostin = in;
  gspec = gin;
  chel.resize(gspec);
  chhc.resize(gspec);
  chex.resize(gspec);
  chtot.resize(gspec);
  ewden.resize(gspec);
  ewnom.resize(gspec);
  chint.resize(gspec);
  expuw.resize(gspec);
  chexw.resize(gspec);
  ihc.resize(gspec);
  irej.resize(gspec);


  for(int i=0;i<gspec;i++){
    chel[i]=0.;
    chhc[i]=0.;
    chex[i]=0.;
    chtot[i]=0.;
    ihc[i]=0.;
    ewden[i].resize(11);
    ewnom[i].resize(11);
    chint[i].resize(11);
    for(int j=0; j<11; j++) {
      ewden[i][j]=0.;
      ewnom[i][j]=0.;
      chint[i][j]=0.;
    };
  };
  
};

void Widommod::ghostInsert(vector<Particle> &p, vector<Group> &g, Simbox &s, Interact &ip) {
  Particle ghost;
  double u,cu;
  
  cnt++;
  for(int i=0; i < ghostin; i++) {
    ghost.randomPos(s);
    int goverlap=0;
    for(int k=0; k < gspec; k++) {
      ghost.radius = g[k].radius;
      irej[k]=0;
      int j=0;
      while(!ghost.overlap(p[j],s) && j<p.size()){
        j++;
      };
      if(j != p.size()) {
        ihc[k]++;
        irej[k]=1;
        goverlap++;
      };
    };
   
    
    if(goverlap!=gspec) {
      cu=0.;
      u=0.;
      for(int l=0; l < p.size(); l++) {
        double invdi;
        invdi=ghost.invdist(p[l],s);
        cu+=invdi;
        u+=invdi*p[l].charge;
      };

      
      cu=cu*ip.lB;
      u=u*ip.lB;
      double ew,ewla,ewd;

      for(int k=0; k < gspec; k++) {
        if(irej[k]==0) {
          expuw[k]+=exp(-u*g[k].charge);
          for(int cint=0; cint<11; cint++) {
            ew=g[k].charge*(u-double(cint)*0.1*g[k].charge*cu/double(p.size()));
            ewla = ew*double(cint)*0.1;
            ewd=exp(-ewla);
            ewden[k][cint]+=ewd;
            ewnom[k][cint]+=ew*ewd;
          };
        };        
      };
      
      
    };
  };
};

void Widommod::getWidomResults() {
  double aint4,aint2,aint1;
  for(int i=0; i<gspec; i++) {
    for(int cint=0; cint<11; cint++) {
      if(ewden[i][cint]==0) {
        cout << "Widom denominator equals zero" << endl;
      } else {
        chint[i][cint]=ewnom[i][cint]/ewden[i][cint];
      };
    };
    aint4=chint[i][1]+chint[i][3]+chint[i][5]+chint[i][7]+chint[i][9];
    aint2=chint[i][2]+chint[i][4]+chint[i][6]+chint[i][8];
    aint1=chint[i][0]+chint[i][10];  
    chel[i]=1./30.*(aint1+2*aint2+4*aint4);
  };
  
  double cnttot;
  cnttot=cnt*ghostin;
  cout << "#" << endl;
  cout << "#WIDOM: " << endl;
  cout << "#" << endl;
  cout << "#cnttot: " << cnttot << endl;
  for(int i=0; i < gspec; i++) {
    chhc[i]=-log(double(cnttot-ihc[i])/cnttot);
    chexw[i]=-log(expuw[i]);
    chex[i]=chhc[i]+chel[i];
    cout << "#Widom excess:  " << chex[i] << "    for species: " << i << endl;
    cout << "#Widom electro: " << chel[i] << "    for species: " << i << endl;
    cout << "#Widom hc     : " << chhc[i] << "    for species: " << i << endl;
    cout << "#" << endl;
  };
  
  
};

