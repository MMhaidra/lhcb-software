
#include "CrudeSampler.h"


boost::shared_ptr<GenRingF::GenericResults> CrudeSampler::fit(const GenRingF::GenericInput & input) throw (CouldNotFit)
{
  try
  {
    Lester::Data data;
    for (GenRingF::GenericInput::GenericHits::const_iterator it = input.hits.begin();
         it != input.hits.end();
         ++it) {
      data.hits.push_back(Lester::Hit(it->x(), it->y()));
    };

    RectilinearCPQuantizer rcpq;
    Lester::ThreePointCircleProposerB p(data,
                                        rcpq,
                                        Lester::Constants::circleMeanRadiusParameter*0.1,
                                        Lester::Constants::circleMeanRadiusParameter*0.1);

    const Lester::RichParams initialPoint;
    std::cout << "Created initial point " << initialPoint << std::endl;
    Lester::MyRichMetropolisSampler sampler(initialPoint, data);

    Lester::RichParams currentPoint = initialPoint;
    double currentLogProb = sampler.logTargetDistribution(currentPoint);
    std::cout << "Current point has logProb " << currentLogProb << std::endl;

#ifdef LESTER_USE_GRAPHICS
    data.draw(*GraphicsObjects::wc2,true);
    GraphicsObjects::wc2->update();

    data.draw(*GraphicsObjects::wc);
    GraphicsObjects::wc->update();
#endif


    clock_t startTime = clock();
    unsigned int nIts = 0;
    while(clock()-startTime<Lester::Constants::defaultThinkTimeSecs*CLOCKS_PER_SEC)
    {
      ++nIts;
      // decide what to do
      doTheWork(currentPoint,currentLogProb,p,sampler,data);
      //std::cout << "CurrentPoint " << currentPoint << std::endl;
    };

    std::cout << "NUMITS " << nIts << " NUMHITS " << input.hits.size() << std::endl;

    boost::shared_ptr<GenRingF::GenericResults> ansP(new GenRingF::GenericResults);
    GenRingF::GenericResults & ans = *ansP;
    currentPoint.fill(ans, input);
    return ansP;
  } catch (...) {
    throw CouldNotFit();
  };

};

void CrudeSampler::doTheWork(Lester::RichParams & currentPoint,
                             double & currentLogProb,
                             Lester::ThreePointCircleProposerB & p,
                             Lester::MyRichMetropolisSampler & sampler,
                             const Lester::Data & data)
{
  data.doNothing();

  // suggest insert/remove/jitter each with prob 1/3:
  const double insProb = 0.4;
  const double remProb = 0.1;
  //const double jitProb = 1.-(insProb+remProb);

  const double d = RandFlat::shoot();
  const bool proposeInsert = d<insProb;
  const bool proposeRemove = (proposeInsert?false:(d<insProb+remProb));
  const bool proposeJitter = !(proposeInsert||proposeRemove);


  Lester::RichParams proposal = currentPoint;
  double qReverseOverQForward = 1;
  bool keepForSure=false;

  if (proposeInsert) {
    //std::cout << "Proposing insertion" << std::endl;
    // what shall we propose adding?
    const Lester::CircleParams & c = p.sample();
    proposal.circs.push_back(c);
    const double qForward = insProb*(p.probabilityOf(c));
    const double qReverse = remProb;
    qReverseOverQForward = qReverse/qForward;
  } else if (proposeRemove) {
    const unsigned int siz = currentPoint.circs.size();
    if (siz==0) {
      //std::cout << "Proposing crummy removal" << std::endl;
      // leave as we are}
      keepForSure = true;
    } else {
      //std::cout << "Proposing removal" << std::endl;
      const int toGo = RandFlat::shootInt(siz);
      proposal.circs[toGo] = proposal.circs[siz-1];
      proposal.circs.pop_back();
      const double qForward = remProb;
      const double qReverse = insProb*p.probabilityOf(currentPoint.circs[toGo]);
      qReverseOverQForward = qReverse/qForward;
    };
  } else {
    assert(proposeJitter);
    const unsigned int siz = currentPoint.circs.size();
    if (siz==0) {
      //std::cout << "Proposing crummy jitter" << std::endl;
      // leave as we are!
      keepForSure = true;
    } else {
      //std::cout << "Proposing jitter" << std::endl;
      //qReverseOverQForward = 1;
      const int toJitter = RandFlat::shootInt(siz);
      proposal.circs[toJitter] = proposal.circs[toJitter].jitterSymm1();
    };
  };

  bool acceptedProposal = false;
  try {
    if (keepForSure) {
      // do nothing -- treat as a failed proposal.
    } else {
      const double proposedLogProb = sampler.logTargetDistribution(proposal);
      const double rhoMax = exp(proposedLogProb-currentLogProb) * qReverseOverQForward;
      acceptedProposal = finite(rhoMax) && (RandFlat::shoot()<rhoMax);

      if (acceptedProposal) {
        currentPoint = proposal;
        currentLogProb = proposedLogProb;
        //if (proposeJitter) {
        //  std::cout <<"Ooooh"<< std::endl;
        //  pressAnyKey();
        //};
      };
    };
  } catch (...) {
    // infinitely unlikely point: Reject!
  };

  if (acceptedProposal) {

    static unsigned int count=0;

#ifdef LESTER_USE_GRAPHICS
    if (count ==0) {

      GraphicsObjects::globalCanvas->clear();
      Lester::Colour::kBlack().issue();
      data.draw(*GraphicsObjects::wc);

      //    Lester::Colour::kRed().issue();
      currentPoint.draw(*GraphicsObjects::wc);
      GraphicsObjects::wc->update();
      //std::cout << "Sample made and drawn" << std::endl;
      //pressAnyKey();
      //const double prob = p.probabilityOf(c);
      //std::cout << "Probability determined to be " << prob << std::endl;
      //pressAnyKeyQQuit();

      GraphicsObjects::wc2->clear();
      data.draw(*GraphicsObjects::wc2,true);
      GraphicsObjects::wc2->update();

    };
#endif

    ++count;
    count %=30;
  };

  //std::cout << "CurrentPoint " << currentPoint << std::endl;
}
